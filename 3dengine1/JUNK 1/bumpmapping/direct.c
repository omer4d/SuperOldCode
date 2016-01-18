#include <math.h>
#include <allegro.h>
#include "vec3f.h"

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

void do_bumpmapping(BITMAP *dest, BITMAP *normalmap, int light_x, int light_y)
{
 int x, y, v, b;
 VEC3F n, l = NORMALIZED_VEC3F(vec3f(light_x - 256, light_y - 256, -100.0));
 float k;

 for(y = 0; y < dest->h; y++)
  for(x = 0; x < dest->w; x++)
   {
    v = PIXEL(normalmap, x, y);
    //n = vec3f(0.0, 0.0, -1.0);
    n = vec3f((GETR(v) - 128) * 0.00390625, (GETG(v) - 128) * 0.00390625, -GETB(v) * 0.00390625);

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
 BITMAP *normalmap, *dest;

 init();

 normalmap = load_bitmap("normalmap.bmp", NULL);
 dest = create_bitmap(normalmap->w, normalmap->h);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 while(!keypressed())
  {
   do_bumpmapping(dest, normalmap, mouse_x, mouse_y);
   textprintf_ex(dest, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 destroy_bitmap(dest);
 destroy_bitmap(normalmap);

 return 0;
}
END_OF_MAIN()
