#include <allegro.h>
#include "common.h"
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "rendering.h"
#include "model.h"
#include "gsphere.h"

#define MOTION_SPEED 0.02

BITMAP *buffer;
BITMAP *colormap, *cloudmap, *normalmap;

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

#define RI_TEXEL(bitmap, x, y) ((long *)bitmap->line[BIT_WRAP(y, bitmap->h - 1)])[BIT_WRAP(x, bitmap->w - 1)]

int scroll = 0;

#define NORMAP_W 2048 
#define NORMAP_H 1024

#define TRIG_TABLE_SIZE 1024

float cos_table[TRIG_TABLE_SIZE];
float sin_table[TRIG_TABLE_SIZE];

#define COS(x) cos_table[BIT_WRAP(x, TRIG_TABLE_SIZE - 1)]
#define SIN(x) sin_table[BIT_WRAP(x, TRIG_TABLE_SIZE - 1)]

float alk1 = 1.0 / (float)(NORMAP_W - 1) * (float)TRIG_TABLE_SIZE;
float alk2 = 0.5 / (float)(NORMAP_H - 1) * (float)TRIG_TABLE_SIZE;

VEC3F texcoord_to_vec3f(int i, int j)
{
 VEC3F v;
 int alpha = (int)(i * alk1);
 int beta = (int)(j * alk2 + 256);

 v.x = COS(beta) * COS(alpha);
 v.y = SIN(beta);
 v.z = COS(beta) * SIN(alpha);

 return v;
}

VEC3F local_light_dir;

inline void tex_hline(int y, float x2, float interp1[], float didx[])
{
 int x, minx, maxx, iu, iv;
 float curr[3], k, u, v;
 int col;
 float r, g, b;
 int ir, ig, ib;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 //curr[0] = interp1[1] + (minx - interp1[0]) * didx[0];
 curr[1] = interp1[2] + (minx - interp1[0]) * didx[1];
 curr[2] = interp1[3] + (minx - interp1[0]) * didx[2];

 for(x = minx; x < maxx + 1; x++)
  {
   //k = 1.0 / curr[0];
   u = curr[1];// * k;
   v = curr[2];// * k;
   FAST_ROUND(u * colormap->w, iu);
   FAST_ROUND(v * colormap->h, iv);

   VEC3F n, p;
   int nc = RI_TEXEL(normalmap, iu, iv);
   n.x = getr32(nc) * 0.007843137 - 1.0;
   n.y = getg32(nc) * 0.007843137 - 1.0;
   n.z = getb32(nc) * 0.007843137 - 1.0;

   int alpha, beta;
   FAST_ROUND(iu * alk1, alpha);
   FAST_ROUND(iv * alk2 + 256, beta);

   p.x = COS(beta) * COS(alpha);
   p.y = SIN(beta);
   p.z = COS(beta) * SIN(alpha);

   float br = -VEC3F_DOT_PRODUCT(n, local_light_dir);
   float cloud_br = VEC3F_DOT_PRODUCT(p, local_light_dir) * 0.7;
   if(br < 0.0) { br = 0.0; }
   if(cloud_br < 0.0) { cloud_br = 0.0; }

   col = RI_TEXEL(colormap, iu, iv);
   r = getr32(col) * br;
   g = getg32(col) * br;
   b = getb32(col) * br;

   br = 1.0 - br;
   float lm = geta32(nc) * 0.00392156 * br * br * br * br;

   k = RI_TEXEL(cloudmap, iu + scroll, iv) * 0.00392156 * cloud_br;

   r = (255.0 - r) * k + r + 255.0 * lm;
   g = (255.0 - g) * k + g + 255.0 * lm;
   b = (255.0 - b) * k + b + 200.0 * lm;

   FAST_ROUND(r, ir);
   FAST_ROUND(g, ig);
   FAST_ROUND(b, ib);

   if(ir > 255) { ir = 255; }
   if(ig > 255) { ig = 255; }
   if(ib > 255) { ib = 255; }

   col = makecol32(ir, ig, ib);
   RI_PIXEL(x, y) = col;

   //curr[0] += didx[0];
   curr[1] += didx[1];
   curr[2] += didx[2];
  }
}

void tex_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   tex_hline(y, *accum2, accum1, didx);
   accum1[0] += didy1[0];
   //accum1[1] += didy1[1];
   accum1[2] += didy1[2];
   accum1[3] += didy1[3];
   (*accum2) += dxdy2;
  }
}

void init()
{
 int i, j;

 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);
 set_znear(0.05);
 init_clipper();
 set_rasterizer_function(tex_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
 colormap = load_bitmap("data/color.tga", NULL);
 cloudmap = create_bitmap(colormap->w, colormap->h);
 normalmap = load_bitmap("data/normal.tga", NULL);

 for(j = 0; j < cloudmap->h; j++)
  for(i = 0; i < cloudmap->w; i++)
   {
    int alpha = geta32(getpixel(colormap, i, j));
    putpixel(cloudmap, i, j, alpha);
   }

 for(i = 0; i < TRIG_TABLE_SIZE; i++)
  {
   cos_table[i] = cos(i * 2.0 * M_PI / (float)TRIG_TABLE_SIZE);
   sin_table[i] = sin(i * 2.0 * M_PI / (float)TRIG_TABLE_SIZE);
  }
}

void fix_spherical_texcoord(VEC2F *a, VEC2F *b)
{
 if(a->x > 0.5 && b->x < 0.5)
  b->x += 1.0;
 if(a->x < 0.5 && b->x > 0.5)
  a->x += 1.0;
}

int main()
{
 int exit = 0, pause = 0, i;
 VEC3F pos = vec3f(0.0, 0.0, 2.0), ang = ZERO_VEC3F;
 MAT16F mat, mat2;

 VERTEX *sphere_vert;
 POLY3D *sphere_tri;
 VEC3F *sphere_vertnorm;
 VEC2F *sphere_texcoord;
 int vnum, tnum;

 printf("CONTROLS:\n");
 printf("---------\n");
 printf("ARROW KEYS - Move the planet around.\n");
 printf("SPACEBAR - Pause.\n");
 printf("MOUSE - Change light-source direction.\n");
 printf("ESC - Exit.\n");
 printf("Press any key to continue...");
 getch();

 init();
 position_mouse(0, 0);

 VEC3F local_light1 = vec3f(-100.0, 0.0, -200.0);

 generate_gsphere(&sphere_vert, &sphere_vertnorm, &sphere_texcoord, &sphere_tri, 4, &vnum, &tnum);
 printf("%d vertices - %f percents eliminated.\n", vnum, 100.0 - (float)vnum / (float)(tnum * 3) * 100.0);
 printf("%d triangles - %d after backface removal.\n", tnum, tnum / 2);

 for(i = 0; i < tnum; i++)
  {
   sphere_tri[i].texcoord[0] = sphere_texcoord[sphere_tri[i].vind[0]];
   sphere_tri[i].texcoord[1] = sphere_texcoord[sphere_tri[i].vind[1]];
   sphere_tri[i].texcoord[2] = sphere_texcoord[sphere_tri[i].vind[2]];
   fix_spherical_texcoord(&sphere_tri[i].texcoord[0], &sphere_tri[i].texcoord[1]);
   fix_spherical_texcoord(&sphere_tri[i].texcoord[1], &sphere_tri[i].texcoord[2]);
   fix_spherical_texcoord(&sphere_tri[i].texcoord[2], &sphere_tri[i].texcoord[0]);
  }

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 while(!exit)
  {
   VEC3F lightpos = vec3f(mouse_x, SCREEN_H - mouse_y, 50.0);
   VEC3F center = vec3f(SCREEN_W * 0.5, SCREEN_H * 0.5, 0.0);
   VEC3F local_light1 = NORMALIZED_VEC3F(VEC3F_DIFF(lightpos, center));

   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_LEFT]) { pos.x -= MOTION_SPEED; }
   if(key[KEY_RIGHT]) { pos.x += MOTION_SPEED; }
   if(key[KEY_UP]) { pos.z += MOTION_SPEED; }
   if(key[KEY_DOWN]) { pos.z -= MOTION_SPEED; }
   if(key[KEY_SPACE]) { pause = (pause == 0) ? 1 : 0; key[KEY_SPACE] = 0; }

   if(!pause)
    {
     ang.x = -0.2 * M_PI;
     ang.y += 0.01;
     ang.z = M_PI;
     scroll -= 2;
     scroll = scroll % (cloudmap->w - 1);
    }

   reset_mat16f(mat);
   rotate_z_mat16f(mat, ang.z);
   rotate_y_mat16f(mat, ang.y);
   rotate_x_mat16f(mat, ang.x);

   translate_mat16f(mat, pos.x, pos.y, pos.z);

   reset_mat16f(mat2);
   rotate_z_mat16f(mat2, ang.z);
   rotate_x_mat16f(mat2, -ang.x);
   rotate_y_mat16f(mat2, -ang.y);

   transform_vec3f(&local_light_dir, local_light1, mat2);

   for(i = 0; i < vnum; i++)
    {
     transform_vec3f(&sphere_vert[i].trans, sphere_vert[i].object, mat);
     project_vertex(&sphere_vert[i]);
    }

   clear_to_color(buffer, 0);

   for(i = 0; i < tnum; i++)
    {
     update_poly3d_normal(&sphere_tri[i], sphere_vert);
     if(!cull_backface(&sphere_tri[i], sphere_vert))
      render_poly3d(&sphere_tri[i], sphere_vert);
    }

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 destroy_bitmap(colormap);
 destroy_bitmap(cloudmap);
 destroy_bitmap(normalmap);

 free(sphere_vert);

 for(i = 0; i < tnum; i++)
  destroy_poly3d(&sphere_tri[i]);

 free(sphere_tri);
 free(sphere_texcoord);
 free(sphere_vertnorm);

 deinit_engine();
 return 0;
}
END_OF_MAIN()
