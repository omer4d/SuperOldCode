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
 VEC3F vw = vec3f(view_x, view_y, 512.0), p = vec3f(256.0, 256.0, 0.0), v = NORMALIZED_VEC3F(VEC3F_DIFF(vw, p));
 float m1 = v.x / v.z, m2 = v.y / v.z;
 float s = 0.2, k = 0.0, hr;
 float dx, dy;

 for(y = 1; y < bumpmap->h - 1; y++)
  for(x = 1; x < bumpmap->w - 1; x++)
   {    
    h = (float)PIXEL(bumpmap, x, y);
    hr = h * s + k;
    dx = hr * m1;
    dy = hr * m2;
    sx = x + dx;
    sy = y + dy;

    h = (h + (float)PIXEL(bumpmap, sx, sy)) * 0.5;
    hr = h * s + k;
    dx = hr * m1;
    dy = hr * m2;
    sx = x + dx;
    sy = y + dy;

    h = (h + (float)PIXEL(bumpmap, sx, sy)) * 0.5;
    hr = h * s + k;
    dx = hr * m1;
    dy = hr * m2;
    sx = x + dx;
    sy = y + dy;

    if(inbound(sx, dest->w) && inbound(sy, dest->h))
     PIXEL(dest, x, y) = sample_4(colormap, sx, sy, linear_interp);
    else
     PIXEL(dest, x, y) = 0;
  }
}

int main()
{
 BITMAP *bumpmap, *colormap, *dest, *dest2;

 init();
 generate_envmap();

 bumpmap = load_bitmap("bumpmap3.bmp", NULL);
 colormap = load_bitmap("colormap.bmp", NULL);
 dest = create_bitmap(bumpmap->w, bumpmap->h);
 dest2 = create_bitmap(bumpmap->w, bumpmap->h);

 while(!keypressed())
  {
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
