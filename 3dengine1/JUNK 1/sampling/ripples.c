#include <math.h>
#include <allegro.h>
#include "pixel.h"

float cos_table[256];
#define COS(x) cos_table[abs((int)x&255)]

void init();
void process_image(BITMAP *dest, BITMAP *source, int t, int m, int f);

int main()
{
 BITMAP *source, *dest;
 int t = 0;

 init();
 source = load_bitmap("lenna.bmp", NULL);
 dest = create_bitmap(source->w, source->h);

 while(!keypressed())
  {
   t += 5;
   process_image(dest, source, t, 10, 2);
   blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(dest);
 destroy_bitmap(source);
 return 0;
}
END_OF_MAIN()

void init()
{
 int d;

 allegro_init();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();

 for(d = 0; d < 256; d++)
  cos_table[d] = cos(d * M_PI / 128.0);
}

void process_image(BITMAP *dest, BITMAP *source, int t, int m, int f)
{
 register int x, y, a = 0, l = source->h - m;

 for(x = source->w; x--;)
  {
   a += f;
   for(y = m; y < l; y++)
    PIXEL(dest, x, y) = sample_16(source, x, y + m * COS(t + a), hermite_interp);
  }
}
