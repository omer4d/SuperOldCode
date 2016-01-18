#include <allegro.h>
#include <math.h>
#include "pixel.h"

BITMAP *buffer;

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 //install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void resize_bitmap(BITMAP *dest, BITMAP *source, int cx, int cy, int dw, int dh)
{
 int x, y, c;
 float dx = (float)source->w / (float)dw, dy = (float)source->h / (float)dh;

 for(y = 0; y < dh; y++)
  for(x = 0; x < dw; x++)
   {
    //c = sample_4(source, x * dx, y * dy, linear_interp);
    c = sample_16(source, x * dx, y * dy, cubic_interp);
    //c = sample_16(source, x * dx, y * dy, hermite_interp);
    PIXEL(dest, cx + x, cy + y) = c;
   }
}

int main()
{
 BITMAP *bitmap, *dest;

 init();

 bitmap = load_bitmap("wall.bmp", NULL);

 clear_to_color(buffer, 0);
 resize_bitmap(buffer, bitmap, 0, 0, 512, 512);
 blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

 save_bitmap("scaled.bmp", buffer, NULL);
 destroy_bitmap(bitmap);
 destroy_bitmap(buffer);
 destroy_bitmap(dest);

 readkey();
 return 0;
}
END_OF_MAIN()
