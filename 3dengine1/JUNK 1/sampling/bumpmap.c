#include <math.h>
#include <allegro.h>
#include "pixel.h"

int envmap[256][256];

void generate_envmap()
{
 int x, y;
 float nx, ny, nz;

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    nx = (x - 128) * 0.0078125;
    ny = (y - 128) * 0.0078125;
    nz = 1.0 - sqrt(nx * nx + ny * ny);
    if(nz < 0) nz = 0;
    envmap[x][y] = (int)(nz * 255);
  }
}

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
 generate_envmap();
}

void do_bumpmapping(BITMAP *dest, BITMAP *bumpmap, BITMAP *colormap, int light_x, int light_y)
{
 int x, y, nx, ny, lx, ly, c;
 float k;

 for(y = 1; y < bumpmap->h - 1; y++)
  for(x = 1; x < bumpmap->w - 1; x++)
   {
    nx = PIXEL(bumpmap, x + 1, y) - PIXEL(bumpmap, x - 1, y);
    ny = PIXEL(bumpmap, x, y + 1) - PIXEL(bumpmap, x, y - 1);
    lx = x - light_x - 128;
    ly = y - light_y - 128;
    nx -= lx;
    ny -= ly;
    clamp(&nx, 0, 255);
    clamp(&ny, 0, 255);

    c = PIXEL(colormap, x, y);
    k = envmap[nx][ny] * 0.00390625;

    PIXEL(dest, x, y) = MAKECOL(GETR(c) * k, GETG(c) * k, GETB(c) * k);
  }
}

int main()
{
 BITMAP *bumpmap, *colormap, *dest;

 init();
 generate_envmap();

 bumpmap = load_bitmap("bumpmap.bmp", NULL);
 colormap = load_bitmap("colormap.bmp", NULL);
 dest = create_bitmap(bumpmap->w, bumpmap->h);

 while(!keypressed())
  {
   do_bumpmapping(dest, bumpmap, colormap, mouse_x, mouse_y);
   blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(dest);
 destroy_bitmap(colormap);
 destroy_bitmap(bumpmap);

 return 0;
}
END_OF_MAIN()
