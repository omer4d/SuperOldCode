#include <allegro.h>
#include <stdio.h>
#include "vector.h"
#include "graphics.h"
#include "map.h"

BITMAP *buffer;

void init()
{
 allegro_init();
 set_color_depth(32);

 install_keyboard();
 install_mouse();

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

int main()
{
 int exit = 0;
 int t;
 LINE_LOOP *loop;

 init();

 loop = load_map("map.txt", &t);

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
    }

   clear_to_color(buffer, makecol32(128, 128, 128));
   draw_map_sketch(buffer, loop, t);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 free_map(loop, t);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
