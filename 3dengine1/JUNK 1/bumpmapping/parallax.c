#include <math.h>
#include <allegro.h>
#include "vec3f.h"
#include "vec2f.h"

#define FLOAT_TO_INT(in, out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st");
#define PIXEL(bitmap, x, y) ((long *)bitmap->line[y])[x]
#define GETR(c) getr32(c)
#define GETG(c) getg32(c)
#define GETB(c) getb32(c)
#define MAKECOL(r, g, b) makecol32(r, g, b)

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

void init()
{
 allegro_init();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();
}

float inv_sqrt(float x)
{
 float xhalf = 0.5f * x;
 int i = *(int*)&x;
 i = 0x5f3759df - (i >> 1);
 x = *(float*)&i;
 x = x * (1.5f - xhalf * x * x);
 return x;
}

void do_bumpmapping(BITMAP *dest, BITMAP *normalmap, BITMAP *colormap, int light_x, int light_y, VEC3F v)
{
 int x, y, c, b;
 VEC3F n, lw = vec3f(light_x, light_y, -200.0), p, l;
 float k, s;

 for(y = 0; y < dest->h; y++)
  for(x = 0; x < dest->w; x++)
   {
    c = PIXEL(normalmap, x, y);
    n = vec3f(0.0, 0.0, -1.0);
    n = vec3f((GETR(c) - 128) * 0.00390625, (GETG(c) - 128) * 0.00390625, -GETB(c) * 0.00390625);
    p = vec3f(x, y, 0.0);

    l = VEC3F_DIFF(lw, p);
    k = inv_sqrt(VEC3F_DOT_PRODUCT(l, l));
    l = USCALE_VEC3F(l, k);

    k = VEC3F_DOT_PRODUCT(l, n);

    VEC3F h = USCALE_VEC3F(VEC3F_SUM(l, v), 0.5);
    if(k > 0.0)
     {
      s = VEC3F_DOT_PRODUCT(h, n);
      s = s * s * s * s * s * s * s * s * s * s * s * s * s * s * s * s * s;
     }

    if(k < 0.0) { k = 0.0; }
    if(k > 1.0) { k = 1.0; }
    
    k = k * k * k;
    
    k = 0.05 + k * 0.95;

    int pix = PIXEL(colormap, x, y);
    int r = (GETR(pix) * k) * (1.0 - s) + 255 * s;
    int g = (GETG(pix) * k) * (1.0 - s) + 255 * s;
    int b = (GETB(pix) * k) * (1.0 - s) + 255 * s;
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;

    //k *= 255.0;

    //FLOAT_TO_INT(k, b);
    PIXEL(dest, x, y) = MAKECOL(r, g, b);
   }
}

int inbound(float k, float max)
{
 if(k > 0 && k < max)
  return 1;
 return 0;
}

int my_getpixel(BITMAP *bitmap, int x, int y)
{
 if(x > 0 && x < bitmap->w && y > 0 && y < bitmap->w)
  return PIXEL(bitmap, x, y);
}

void do_parallax(BITMAP *dest, BITMAP *bumpmap, BITMAP *colormap, VEC3F v)
{
 int x, y;
 float h, sx, sy;
 float m1 = v.x / v.z, m2 = v.y / v.z;
 float s = 0.2, k = 0.0, hr;
 float dx, dy;
 int i;

 for(y = 1; y < bumpmap->h - 1; y++)
  for(x = 1; x < bumpmap->w - 1; x++)
   {    
    h = (float)my_getpixel(bumpmap, x, y);
    hr = h * s + k;
    dx = hr * m1;
    dy = hr * m2;
    sx = x + dx;
    sy = y + dy;

    for(i = 0; i < 3; i++)
     {
      h = (h + (float)my_getpixel(bumpmap, (int)sx, (int)sy)) * 0.5;
      hr = h * s + k;
      dx = hr * m1;
      dy = hr * m2;
      sx = x + dx;
      sy = y + dy;
     }

    if(inbound((int)sx, dest->w) && inbound((int)sy, dest->h))
     PIXEL(dest, x, y) = PIXEL(colormap, (int)sx, (int)sy);
    else
     PIXEL(dest, x, y) = 0;
  }
}

int main()
{
 BITMAP *normalmap, *heightmap, *color, *dest, *dest2;

 init();

 normalmap = load_bitmap("normalmap.bmp", NULL);
 heightmap = load_bitmap("height.bmp", NULL);
 color = load_bitmap("color.bmp", NULL);
 dest = create_bitmap(normalmap->w, normalmap->h);
 dest2 = create_bitmap(normalmap->w, normalmap->h);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 position_mouse(10, 10);

 VEC2F center2f = vec2f(normalmap->w * 0.5, normalmap->h * 0.5), light2f, view2f;
 VEC3F light3f, view3f;
 
 float alpha = 0.0;

 while(!keypressed())
  {
   light2f = vec2f((200.0 + 100.0 * cos(2.0 * alpha)) * cos(alpha), (200.0 + 100.0 * sin(3.0 * alpha)) * sin(alpha));
   view2f = VEC2F_DIFF(vec2f(mouse_x, mouse_y), center2f);
   light3f = NORMALIZED_VEC3F(vec3f(light2f.x, light2f.y, -100.0));
   view3f = NORMALIZED_VEC3F(vec3f(-view2f.x, -view2f.y, -512.0));

   clear_to_color(dest2, 0);

   do_bumpmapping(dest, normalmap, color, mouse_x, mouse_y, view3f);
   do_parallax(dest2, heightmap, dest, view3f);
   textprintf_ex(dest2, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);

   //circlefill(dest2, center2f.x + light2f.x, center2f.y + light2f.y, 3, makecol(255, 255, 255));

   blit(dest2, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
   alpha += 0.05;
  }

 destroy_bitmap(dest2);
 destroy_bitmap(dest);
 destroy_bitmap(color);
 destroy_bitmap(heightmap);
 destroy_bitmap(normalmap);

 return 0;
}
END_OF_MAIN()
