#include <allegro.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "rendering.h"
#include <stdio.h>
#include "tgspace.h"

#define CAM_SPEED 0.8

BITMAP *buffer, *texture;

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

#define RI_TEXEL(x, y) ((long *)texture->line[BIT_WRAP(y, texture->h - 1)])[BIT_WRAP(x, texture->w - 1)]

void tex_hline(int y, float x2, float interp1[], float didx[])
{
 int x, minx, maxx, iu, iv;
 float curr[4], k, u, v;
 long z;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 curr[0] = interp1[1] + (minx - interp1[0]) * didx[0];
 curr[1] = interp1[2] + (minx - interp1[0]) * didx[1];
 curr[2] = interp1[3] + (minx - interp1[0]) * didx[2];
 curr[3] = interp1[4] + (minx - interp1[0]) * didx[3];

 for(x = minx; x < maxx + 1; x++)
  {
   z = (long)(BASE_INT_z_buffer_precision / curr[0]);

   if(z < RI_PIXEL_Z(x, y))
    {
     k = 1.0 / curr[1];
     u = curr[2] * k;
     v = curr[3] * k;
     FAST_ROUND(u * texture->w, iu);
     FAST_ROUND(v * texture->h, iv);
     RI_PIXEL(x, y) = RI_TEXEL(iu, iv);
     RI_PIXEL_Z(x, y) = z;
    }

   curr[0] += didx[0];
   curr[1] += didx[1];
   curr[2] += didx[2];
   curr[3] += didx[3];
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
   accum1[2] += didy1[2];
   accum1[3] += didy1[3];
   accum1[4] += didy1[4];
   (*accum2) += dxdy2;
  }
}

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);
 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, 0.0, 200.0);
 set_znear(1.0);
 init_clipper();
 set_rasterizer_function(tex_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
 texture = load_bitmap("data/babe_skin.bmp", NULL);
}

void load_map(char fn[], VERTEX **vertex, POLY3D **poly, int *vnum, int *pnum, float scale)
{
 FILE *stream;
 int v, p, i, j, t;
 VEC3F temp_vec3f;
 VEC2F temp_vec2f;
 VEC2F *texcoord;

 stream = fopen(fn, "r");

 if(stream == NULL)
  {
   allegro_message("Unable to load map file: %s.", fn);
   exit(1);
  }

 fscanf(stream, "%d", &v);
 *vertex = (VERTEX *)malloc(sizeof(VERTEX) * v);
 texcoord = (VEC2F *)malloc(sizeof(VEC2F) * v);

 for(i = 0; i < v; i++)
  {
   fscanf(stream, "%f", &temp_vec3f.z);
   fscanf(stream, "%f", &temp_vec3f.x);
   fscanf(stream, "%f", &temp_vec3f.y); 
   fscanf(stream, "%f", &temp_vec2f.x);
   fscanf(stream, "%f", &temp_vec2f.y);
   (*vertex)[i].object = USCALE_VEC3F(temp_vec3f, scale);
   texcoord[i].x = temp_vec2f.x * 0.01;
   texcoord[i].y = temp_vec2f.y * 0.01;
  }

 *vnum = v;

 fscanf(stream, "%d", &p);
 *poly = (POLY3D *)malloc(sizeof(POLY3D) * p);

 for(i = 0; i < p; i++)
  {
   fscanf(stream, "%d", &v);
   //fscanf(stream, "%f", &temp_vec3f.x);
   //fscanf(stream, "%f", &temp_vec3f.y);
   //fscanf(stream, "%f", &temp_vec3f.z); 
   //(*poly)[i].normal = temp_vec3f;

   (*poly)[i].vnum = v;
   (*poly)[i].vind = (long *)malloc(v * sizeof(long));
   (*poly)[i].texcoord = (VEC2F *)malloc(v * sizeof(VEC2F));

   for(j = 0; j < v; j++)
    {
     fscanf(stream, "%d", &t);
     (*poly)[i].vind[j] = t;
     (*poly)[i].texcoord[j] = texcoord[t];
    }

   (*poly)[i].normal = vec3f_normal((*vertex)[(*poly)[i].vind[0]].object,
                                    (*vertex)[(*poly)[i].vind[1]].object,
                                    (*vertex)[(*poly)[i].vind[2]].object);
  }

 free(texcoord);
 *pnum = p;
 fclose(stream);
}

VEC3F to_vec3f(VEC2F v)
{
 return vec3f(v.x, v.y, 0.0);
}


int main()
{
 int exit = 0, i, vnum, pnum;
 VEC3F pos = vec3f(0.0, 0.0, 0.3), ang = ZERO_VEC3F;
 MAT16F tmat;
 VERTEX *vertex;
 POLY3D *poly;

 init();

 load_map("quake_map2.txt", &vertex, &poly, &vnum, &pnum, 0.2);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 VEC3F cam_pos = vec3f(0.0, 3.0, 0.0), cam_dir, cam_dir_normal, cam_ang = vec3f(0.0, 0.0, 0.0);

 while(!exit)
  {
   int mx, my;
   get_mouse_mickeys(&mx, &my);
   position_mouse(SCREEN_W / 2, SCREEN_H / 2);

   cam_ang.x += my * 0.001;
   cam_ang.y -= mx * 0.001;
   cam_dir.x = CAM_SPEED * cos(0.5 * M_PI + cam_ang.y);
   cam_dir.y = CAM_SPEED * cos(0.5 * M_PI + cam_ang.x);
   cam_dir.z = CAM_SPEED * sin(0.5 * M_PI + cam_ang.y);
   cam_dir_normal = vec3f(-cam_dir.z, 0.0, cam_dir.x);

   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_W]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir); }
   if(key[KEY_S]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir); }
   if(key[KEY_A]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir_normal); }
   if(key[KEY_D]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir_normal); }

   set_fov(90.0);
   if(mouse_b > 1)
    set_fov(30.0);

   reset_mat16f(tmat);
   translate_mat16f(tmat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_z_mat16f(tmat, 0.0);
   rotate_y_mat16f(tmat, -cam_ang.y);
   rotate_x_mat16f(tmat, -cam_ang.x);

   for(i = 0; i < vnum; i++)
    {
     transform_vec3f(&vertex[i].trans, vertex[i].object, tmat);
     project_vertex(&vertex[i]);
    }

   clear_to_color(buffer, 0);
   clear_to_color(BASE_INT_z_buffer, BASE_INT_z_buffer_precision);

   for(i = 0; i < pnum; i++)
    {
     update_poly3d_normal(&poly[i], vertex);
     V0 = vertex[poly[i].vind[0]].trans;
     global_n = USCALE_VEC3F(NORMALIZED_VEC3F(poly[i].normal), -1.0);
     VEC3F N = global_n;
     DIST = -VEC3F_DOT_PRODUCT(V0, N);

     get_axes(vertex[poly[i].vind[0]].trans,
              vertex[poly[i].vind[1]].trans,
              vertex[poly[i].vind[2]].trans,
              to_vec3f(poly[i].texcoord[0]),
              to_vec3f(poly[i].texcoord[1]),
              to_vec3f(poly[i].texcoord[2]), &A, &B, &C);
     T0 = to_vec3f(poly[i].texcoord[0]);

     if(cull_backface(&poly[i], vertex))
     render_poly3d(&poly[i], vertex);
    }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 for(i = 0; i < pnum; i++)
  destroy_poly3d(&poly[i]);
 free(vertex);

 deinit_engine();
 return 0;
}
END_OF_MAIN()
