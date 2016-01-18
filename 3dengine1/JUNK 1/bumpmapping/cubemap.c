#include <math.h>
#include <allegro.h>
#include "vec3f.h"

#define FLOAT_TO_INT(in, out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st");
#define PIXEL(bitmap, x, y) ((long *)bitmap->line[y])[x]
#define GETR(c) getr32(c)
#define GETG(c) getg32(c)
#define GETB(c) getb32(c)
#define MAKECOL(r, g, b) makecol32(r, g, b)

#define CUBEMAP_RES 256
#define HALF_CUBEMAP_RES 127.5

BITMAP *cubemap[6];

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

void init()
{
 int i, j;

 allegro_init();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();

 for(i = 0; i < 6; i++)
  cubemap[i] = create_bitmap(CUBEMAP_RES, CUBEMAP_RES);
}

void gen_normalization_cubemap()
{
 int i, j;
 VEC3F p;
 float h = CUBEMAP_RES * 0.5;

 void gen(VEC3F p, int n, int i, int j)
  {
   VEC3F v = NORMALIZED_VEC3F(p);
   int nx = (int)((v.x + 1.0) * 127.5 + 0.5);
   int ny = (int)((v.y + 1.0) * 127.5 + 0.5);
   int nz = (int)((v.z + 1.0) * 127.5 + 0.5);
   putpixel(cubemap[n], i, j, makecol(nx, ny, nz));
  }

 for(j = 0; j < CUBEMAP_RES; j++)
  for(i = 0; i < CUBEMAP_RES; i++)
   {
    p = vec3f(i - h, j - h, -h); gen(p, 0, i, j);
    p = vec3f(i - h, j - h, h); gen(p, 1, i, j);
    p = vec3f(h, j - h, i - h); gen(p, 2, i, j);
    p = vec3f(-h, j - h, i - h); gen(p, 3, i, j);
    p = vec3f(i - h, -h, j - h); gen(p, 4, i, j);
    p = vec3f(i - h, h, j - h); gen(p, 5, i, j);
   }
}

VEC3F cubemap_normalize(VEC3F t)
{
 VEC3F r;
 int n, cx, cy, c;
 float k, h = HALF_CUBEMAP_RES, q = 0.0078431372549019607843137254901961;

 if(fabs(t.z) > fabs(t.x) && fabs(t.z) > fabs(t.y))
  {
   if(t.z < 0.0) { n = 0; k = -h / t.z; }
   else          { n = 1; k = h / t.z; }

   cx = (int)(t.x * k + h + 0.5), cy = (int)(t.y * k + h + 0.5);
   c = PIXEL(cubemap[n], cx, cy);
   r = vec3f(GETR(c) * q - 1.0, GETG(c) * q - 1.0, GETB(c) * q - 1.0);
   return r;
  }

 else if(fabs(t.x) > fabs(t.y))
  {
   if(t.x < 0.0) { n = 3; k = -h / t.x; }
   else          { n = 2; k = h / t.x; }

   cx = (int)(t.z * k + h + 0.5), cy = (int)(t.y * k + h + 0.5);
   c = PIXEL(cubemap[n], cx, cy);
   r = vec3f(GETR(c) * q - 1.0, GETG(c) * q - 1.0, GETB(c) * q - 1.0);
   return r;
  }

 else
  {
   int n, cx, cy, c;
   float k;

   if(t.y < 0.0) { n = 4; k = -h / t.y; }
   else          { n = 5; k = h / t.y; }

   cx = (int)(t.x * k + h + 0.5), cy = (int)(t.z * k + h + 0.5);
   c = PIXEL(cubemap[n], cx, cy);
   r = vec3f(GETR(c) * q - 1.0, GETG(c) * q - 1.0, GETB(c) * q - 1.0);
  }

 return r;
}

void do_bumpmapping(BITMAP *dest, BITMAP *normalmap, int light_x, int light_y)
{
 int x, y, v, b;
 VEC3F n, lw = vec3f(light_x, light_y, -20.0), p, l;
 float k;

 for(y = 0; y < dest->h; y++)
  for(x = 0; x < dest->w; x++)
   {
    v = PIXEL(normalmap, x, y);
    //n = vec3f(0.0, 0.0, -1.0);
    n = vec3f((GETR(v) - 128) * 0.00390625, (GETG(v) - 128) * 0.00390625, -GETB(v) * 0.00390625);
    p = vec3f(x, y, 0.0);

    l = VEC3F_DIFF(lw, p);
    l = cubemap_normalize(l);

    k = VEC3F_DOT_PRODUCT(l, n);
    if(k < 0.0) { k = 0.0; }
    if(k > 1.0) { k = 1.0; }
    k *= 255.0;
    FLOAT_TO_INT(k, b);
    PIXEL(dest, x, y) = MAKECOL(b, b, b);
   }
}

int main()
{
 int i;
 BITMAP *normalmap, *dest;

 init();

 normalmap = load_bitmap("normalmap.bmp", NULL);
 dest = create_bitmap(normalmap->w, normalmap->h);

 gen_normalization_cubemap();

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 while(!key[KEY_ESC])
  {
   do_bumpmapping(dest, normalmap, mouse_x, mouse_y);
   textprintf_ex(dest, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 for(i = 0; i < 6; i++)
  destroy_bitmap(cubemap[i]);
 destroy_bitmap(dest);
 destroy_bitmap(normalmap);

 return 0;
}
END_OF_MAIN()
