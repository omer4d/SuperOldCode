#include <allegro.h>
#include "rendering.h"

BITMAP *buffer;

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

int main()
{
 int exit = 0, n, i;

 REND_INT_POINT arr0[10], arr1[10], arr2[10];

 arr0[0].sx = 550;
 arr0[0].sy = 200;
 arr0[1].sx = 50;
 arr0[1].sy = 200;
 arr0[2].sx = 300;
 arr0[2].sy = 400;
 arr1[0].sx = 400;
 arr1[0].sy = 200;
 arr1[1].sx = 200;
 arr1[1].sy = 200;
 arr1[2].sx = 300;
 arr1[2].sy = 400;

 init();

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_LEFT]) { arr0[0].sx -= 1.0; arr0[1].sx -= 1.0; arr0[2].sx -= 1.0; }
   if(key[KEY_RIGHT]) { arr0[0].sx += 1.0; arr0[1].sx += 1.0; arr0[2].sx += 1.0; }
   if(key[KEY_UP]) { arr0[0].sy -= 1.0; arr0[1].sy -= 1.0; arr0[2].sy -= 1.0; }
   if(key[KEY_DOWN]) { arr0[0].sy += 1.0; arr0[1].sy += 1.0; arr0[2].sy += 1.0; }

   arr1[0].sx = arr0[0].sx;
   arr1[0].sy = arr0[0].sy;
   arr1[1].sx = arr0[1].sx;
   arr1[1].sy = arr0[1].sy;
   arr1[2].sx = arr0[2].sx;
   arr1[2].sy = arr0[2].sy;

   float minx = 100, miny = 100, maxx = 540, maxy = 380;

   n = clip_x(arr1, arr2, 3, minx, 1);
   n = clip_x(arr2, arr1, n, maxx, -1);
   n = clip_y(arr1, arr2, n, miny, 1);
   n = clip_y(arr2, arr1, n, maxy, -1);

   clear_to_color(buffer, 0);

   for(i = 0; i < 2; i++)
    line(buffer, arr0[i].sx, arr0[i].sy, arr0[i + 1].sx, arr0[i + 1].sy, makecol(255, 0, 0));
   line(buffer, arr0[2].sx, arr0[2].sy, arr0[0].sx, arr0[0].sy, makecol(255, 0, 0));

   line(buffer, minx, miny, maxx, miny, makecol(0, 255, 0));
   line(buffer, minx, maxy, maxx, maxy, makecol(0, 255, 0));
   line(buffer, minx, miny, minx, maxy, makecol(0, 255, 0));
   line(buffer, maxx, miny, maxx, maxy, makecol(0, 255, 0));

    if(n > 0)
     for(i = 1; i < n - 1; i++)
      triangle(buffer, arr1[0].sx, arr1[0].sy, arr1[i].sx, arr1[i].sy, arr1[i+1].sx, arr1[i+1].sy, makecol(255, 255, 255));

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
