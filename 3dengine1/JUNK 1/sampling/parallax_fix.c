#include <math.h>
#include <allegro.h>
#include "pixel.h"
#include "vec3f.h"

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

int inbound(float k, float max)
{
 if(k > 0 && k < max)
  return 1;
 return 0;
}

void do_parallax(BITMAP *dest, BITMAP *bumpmap, BITMAP *colormap, int view_x, int view_y)
{
 int x, y;
 float h, sx, sy;
 VEC3F vw = vec3f(0.0, view_y, 512.0), p = vec3f(256.0, 256.0, 0.0), v = VEC3F_DIFF(vw, p);
 float m1 = v.x / v.z, m2 = v.y / v.z;
 float s = (512.0 / 25.0) / 256.0, k = s * -0.5, hr;

 float h0 = 256.0;
 y = 256;

  for(x = 1; x < bumpmap->w - 2; x++)
   {
    h = (float)PIXEL(bumpmap, x, y) * 0.2;
    float h2 = (float)PIXEL(bumpmap, x + 1, y) * 0.2;

    PIXEL(dest, x, h0) = makecol(255, 255, 255);
    line(dest, x, h0 + h, x + 1, h0 + h2, makecol(255, 0, 0));
  }

 float tx = mouse_x;
 h = PIXEL(bumpmap, tx, 256) * 0.2;
 sx = tx + h * m1;

 int i;
 
 for(i = 0; i < 1000; i++)
  {
    h = (h + PIXEL(bumpmap, sx, 256) * 0.2) * 0.5;
    sx = tx + h * m1;
   }

 circlefill(dest, tx, h0, 2, makecol(0, 128, 0));
 circlefill(dest, sx, h0 + h, 2, makecol(128, 128, 0));
 line(dest, tx, h0, tx + v.x, h0 + v.z, makecol(0, 128, 0));
 //line(dest, tx, h0, tx, h0 + h, makecol(128, 128, 0));
}

int main()
{
 BITMAP *bumpmap, *colormap, *dest, *dest2;

 init();
 generate_envmap();

 bumpmap = load_bitmap("bumpmap.bmp", NULL);
 colormap = load_bitmap("colormap.bmp", NULL);
 dest = create_bitmap(bumpmap->w, bumpmap->h);
 dest2 = create_bitmap(bumpmap->w, bumpmap->h);

 while(!keypressed())
  {
   clear_to_color(dest2, 0);
   //do_bumpmapping(dest, bumpmap, colormap, mouse_x, mouse_y);
   do_parallax(dest2, bumpmap, colormap, mouse_x, mouse_y);
   blit(dest2, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(dest2);
 destroy_bitmap(dest);
 destroy_bitmap(colormap);
 destroy_bitmap(bumpmap);

 return 0;
}
END_OF_MAIN()
