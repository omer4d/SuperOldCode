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

void clamp(int *x, int min, int max)
{
 if(*x < min) *x = min;
 if(*x > max) *x = max;
}

float linear_interp(float a, float b, float k)
{
 return a * k + b * (1.0 - k);
}

float cubic_interp(float y0, float y1, float y2, float y3, float mu)
{
 float a0, a1, a2, a3, mu2;

 mu2 = mu * mu;
 a0 = y3 - y2 - y0 + y1;
 a1 = y0 - y1 - a0;
 a2 = y2 - y0;
 a3 = y1;

 return a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3;
}

 float tension, bias;

float hermite_interp(float y0, float y1, float y2, float y3, float mu)
{
 float m0, m1, mu2, mu3;
 float a0, a1, a2, a3;

 mu2 = mu * mu;
 mu3 = mu2 * mu;
 m0  = (y1 - y0) * (1.0 + bias) * (1.0 - tension) * 0.5;
 m0 += (y2 - y1) * (1.0 - bias) * (1.0 - tension) * 0.5;
 m1  = (y2 - y1) * (1.0 + bias) * (1.0 - tension) * 0.5;
 m1 += (y3 - y2) * (1.0 - bias) * (1.0 - tension) * 0.5;
 a0 = 2.0 * mu3 - 3.0 * mu2 + 1.0;
 a1 = mu3 - 2.0 * mu2 + mu;
 a2 = mu3 - mu2;
 a3 = -2.0 * mu3 + 3.0 * mu2;

 return a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2;
}


int bilinear_sample(BITMAP *bitmap, float x, float y)
{
 int ix = (int)x, iy = (int)y, r[5], g[5], b[5];
 float k1 = y - iy, k2 = x - ix;

 getrgb(PIXEL(bitmap, ix, iy), &r[1], &g[1], &b[1]);
 getrgb(PIXEL(bitmap, ix + 1, iy), &r[2], &g[2], &b[2]);
 getrgb(PIXEL(bitmap, ix + 1, iy + 1), &r[3], &g[3], &b[3]);
 getrgb(PIXEL(bitmap, ix, iy + 1), &r[4], &g[4], &b[4]);

 r[0] = linear_interp(linear_interp(r[3], r[2], k1), linear_interp(r[4], r[1], k1), k2);
 g[0] = linear_interp(linear_interp(g[3], g[2], k1), linear_interp(g[4], g[1], k1), k2);
 b[0] = linear_interp(linear_interp(b[3], b[2], k1), linear_interp(b[4], b[1], k1), k2);

 return MAKECOL(r[0], g[0], b[0]);
}

int bicubic_sample(BITMAP *bitmap, float x, float y)
{
 int ix = (int)x, iy = (int)y, r[4], g[4], b[4], rt[5], gt[5], bt[5], i;
 float k1 = y - iy, k2 = x - ix;
 float tc = 0.0, bc = 0.0;
 tension = -1.0 * 1.0, bias = 0.0;

 for(i = 0; i < 4; i++)
  {
   getrgb(PIXEL(bitmap, ix - 1 + i, iy - 1), &r[0], &g[0], &b[0]);
   getrgb(PIXEL(bitmap, ix - 1 + i, iy),     &r[1], &g[1], &b[1]);
   getrgb(PIXEL(bitmap, ix - 1 + i, iy + 1), &r[2], &g[2], &b[2]);
   getrgb(PIXEL(bitmap, ix - 1 + i, iy + 2), &r[3], &g[3], &b[3]);
   rt[i] = cubic_interp(r[0], r[1], r[2], r[3], k1);
   gt[i] = cubic_interp(g[0], g[1], g[2], g[3], k1);
   bt[i] = cubic_interp(b[0], b[1], b[2], b[3], k1);
  }

 rt[4] = cubic_interp(rt[0], rt[1], rt[2], rt[3], k2);
 gt[4] = cubic_interp(gt[0], gt[1], gt[2], gt[3], k2);
 bt[4] = cubic_interp(bt[0], bt[1], bt[2], bt[3], k2);

 clamp(&rt[4], 0, 255);
 clamp(&gt[4], 0, 255);
 clamp(&bt[4], 0, 255);

 return MAKECOL(rt[4], gt[4], bt[4]);
}

int area_sample(BITMAP *bitmap, float x, float y, float sx, float sy)
{
 int minx = (int)(x - sx + 0.5), miny = (int)(y - sy + 0.5), maxx = (int)(x + sx + 0.5), maxy = (int)(y + sy + 0.5);
 int c, r = 0, g = 0, b = 0, i, j, k = 0;

 for(j = miny; j < maxy + 1; j++)
  for(i = minx; i < maxx + 1; i++)
   {
    c = PIXEL(bitmap, i, j);
    r += GETR(c);
    g += GETG(c);
    b += GETB(c);
    k++;
   }

 return makecol(r / k, g / k, b / k);
}

void resize_bitmap(BITMAP *dest, BITMAP *source, int cx, int cy, int dw, int dh)
{
 int x, y, c;

 for(y = 0; y < dh; y++)
  for(x = 0; x < dw; x++)
   {
    float k1 = (float)x / (float)dw;
    float k2 = (float)y / (float)dh;
    c = bicubic_sample(source, source->w * k1, source->h * k2);
    PIXEL(dest, cx + x, cy + y) = c;
   }
}

int main()
{
 BITMAP *bitmap, *dest;

 init();

 bitmap = load_bitmap("a.bmp", NULL);

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
