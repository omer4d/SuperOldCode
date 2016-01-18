#include <math.h>
#include <allegro.h>
#include "vec3f.h"

#define FLOAT_TO_INT(in, out) __asm__ __volatile__ ("fistpl %0" : "=m" (out) : "t" (in) : "st");
#define PIXEL(bitmap, x, y) ((long *)bitmap->line[y % bitmap->h])[x % bitmap->w]
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

void height2normal(BITMAP *height, BITMAP *normal)
{
 float s = 0.2;
 int x, y;
 float h0, h1, h2, h3;
 VEC3F quad[4], n;

 for(y = 0; y < height->h - 1; y++)
  for(x = 0; x < height->w - 1; x++)
   {
    h0 = (float)getb(getpixel(height, x, y)) * s;
    h1 = (float)getb(getpixel(height, x + 1, y)) * s;
    h2 = (float)getb(getpixel(height, x + 1, y + 1)) * s;
    h3 = (float)getb(getpixel(height, x, y + 1)) * s;

    quad[0] = vec3f((float)x, (float)y, h0);
    quad[1] = vec3f((float)x + 1.0, (float)y, h1);
    quad[2] = vec3f((float)x + 1.0, (float)y + 1.0, h2);
    quad[3] = vec3f((float)x, (float)y + 1.0, h3);

    n = NORMALIZED_VEC3F(VEC3F_BLEND(vec3f_normal(quad[0], quad[1], quad[2]), vec3f_normal(quad[2], quad[3], quad[0]), 0.5));
    float r = (n.x + 1.0) * 127.0, g = (n.y + 1.0) * 127.0, b = (n.z + 1.0) * 127.0;
    putpixel(normal, x, y, makecol(r, g, b));
   }
}

int main()
{
 BITMAP *height, *normal;

 init();

 height = load_bitmap("height.bmp", NULL);
 normal = create_bitmap(height->w, height->h);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 //while(!keypressed())
  //{
   height2normal(height, normal);
   //textprintf_ex(dest, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(normal, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  //}

 readkey();
 destroy_bitmap(height);
 destroy_bitmap(normal);

 return 0;
}
END_OF_MAIN()
