#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "md2.h"
#include "rendering.h"
#include "tgspace.h"

BITMAP *buffer, *texture;

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

VEC3F global_n;
VEC3F lw;
VEC3F A, B, C, V0, T0;

void tex_hline(int y, float x2, float interp1[], float didx[])
{
 int x, minx, maxx;
 float curr;
 long z;
 float rz;
 VEC3F w;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 curr = interp1[1] + (minx - interp1[0]) * didx[0];

 for(x = minx; x < maxx + 1; x++)
  {
   rz = 1.0 / curr;
   z = (long)(rz * BASE_INT_z_buffer_precision);

   if(z < RI_PIXEL_Z(x, y))
    {
     w.z = (rz - BASE_INT_cs_kz) / BASE_INT_cs_mz;
     float k = BASE_INT_focal_dist / w.z;
     w.x = (x - BASE_INT_cs_kx) / (k * BASE_INT_cs_mx);
     w.y = (y - BASE_INT_cs_ky) / (k * BASE_INT_cs_my);

     VEC3F w_tex, global_n_tex, lw_tex;

     transform_to_space(A, B, C, V0, T0, w, &w_tex);
     transform_to_space(A, B, C, V0, T0, lw, &lw_tex);
     float br;

     if(key[KEY_SPACE])
      {
       VEC3F l_tex = NORMALIZED_VEC3F(VEC3F_DIFF(lw_tex, w_tex));
       br = VEC3F_DOT_PRODUCT(l_tex, vec3f(0.0, 0.0, -1.0));
      }

     //printf("%f %f %f\n", l_tex.x, l_tex.y, l_tex.z);
    else
     {
      VEC3F l = NORMALIZED_VEC3F(VEC3F_DIFF(lw, w));
      br = VEC3F_DOT_PRODUCT(l, USCALE_VEC3F(global_n, -1.0));
      }

     if(br < 0.0) { br = 0.0; }
     if(br > 1.0) { br = 1.0; }

     RI_PIXEL(x, y) = makecol(br * 255, br * 255, br * 255);
     RI_PIXEL_Z(x, y) = z;
    }
   curr += didx[0];
  }
}

void tex_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   tex_hline(y, *accum2, accum1, didx);
   accum1[0] += didy1[0];
   accum1[1] += didy1[1];
   (*accum2) += dxdy2;
  }
}

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, 0.0, 10.0);
 point_at_color_buffer(&buffer);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);
 set_znear(0.1);
 init_clipper();
 set_rasterizer_function(tex_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
 texture = load_bitmap("data/babe_skin.bmp", NULL);
}

VEC3F to_vec3f(VEC2F v)
{
 return vec3f(v.x, v.y, 0.0);
}

int main()
{
 int exit_flag = 0, i;

 MAT16F tmat;
 VEC3F pos = vec3f(0.0, 0.0, 0.5), ang = vec3f(0.0, 0.0, 0.0);
 MD2_MODEL mdl;
 VERTEX *model_verts;
 POLY3D *model_tris;
 lw = ZERO_VEC3F;

 init();

 if(load_md2("data/babe.md2", &mdl))
  {
   allegro_message("Error: I need the MD2 model, stupid!");
   exit(1);
  }

 convert_md2_to_base(&model_verts, &model_tris, &mdl, 0.02);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 float frame = 0.0;

 while(!exit_flag)
  {
   if(key[KEY_ESC])   { exit_flag = 1; }
   if(key[KEY_LEFT])  { pos.x -= 0.01; }
   if(key[KEY_RIGHT]) { pos.x += 0.01; }
   if(key[KEY_UP])    { pos.z += 0.02; }
   if(key[KEY_DOWN])  { pos.z -= 0.02; }

   ang.x = M_PI * 1.5;
   ang.y = M_PI;

   convert_md2_frame_to_base(model_verts, &mdl, frame, 0.02);
   frame += 0.1;

   reset_mat16f(tmat);
   rotate_x_mat16f(tmat, ang.x);
   rotate_y_mat16f(tmat, ang.y);
   rotate_z_mat16f(tmat, ang.z);
   translate_mat16f(tmat, pos.x, pos.y, pos.z);

   for(i = 0; i < mdl.header.num_vertices; i++)
    {
     transform_vec3f(&model_verts[i].trans, model_verts[i].object, tmat);
     project_vertex(&model_verts[i]);
    }

   clear_bitmap(buffer);
   clear_to_color(BASE_INT_z_buffer, BASE_INT_z_buffer_precision);

   for(i = 0; i < mdl.header.num_tris; i++)
    {
     update_poly3d_normal(&model_tris[i], model_verts);
     global_n = USCALE_VEC3F(NORMALIZED_VEC3F(model_tris[i].normal), 1.0);

     model_tris[i].texcoord[0] = vec2f(0.0, 0.0);
     model_tris[i].texcoord[1] = vec2f(1.0, 0.0);
     model_tris[i].texcoord[2] = vec2f(1.0, 1.0);

     get_axes(model_verts[model_tris[i].vind[0]].trans,
              model_verts[model_tris[i].vind[1]].trans,
              model_verts[model_tris[i].vind[2]].trans,
              to_vec3f(model_tris[i].texcoord[0]),
              to_vec3f(model_tris[i].texcoord[1]),
              to_vec3f(model_tris[i].texcoord[2]), &A, &B, &C);
     V0 = model_verts[model_tris[i].vind[0]].trans;
     T0 = to_vec3f(model_tris[i].texcoord[0]);

     if(cull_backface(&model_tris[i], model_verts))
      render_poly3d(&model_tris[i], model_verts);
    }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 free_md2(&mdl);

 for(i = 0; i < mdl.header.num_tris; i++)
  {
   free(model_tris[i].vind);
   free(model_tris[i].texcoord);
  }

 free(model_tris);
 free(model_verts);
 destroy_bitmap(buffer);

 return 0;
}
END_OF_MAIN()
