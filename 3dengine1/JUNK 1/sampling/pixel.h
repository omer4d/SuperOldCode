#ifndef _PIXEL_H_
#define _PIXEL_H_

#define EPSILON (0.0001)

#define PIXEL_8(bitmap, x, y) bitmap->line[y][x]
#define PIXEL_16(bitmap, x, y) ((short *)bitmap->line[y])[x]
#define PIXEL_32(bitmap, x, y) ((long *)bitmap->line[y])[x]

int wrap(int x, int min, int max)
{
 int w;

 x -= min;
 max -= min;
 w = x % max;
 if(w < 0) w += max;
 w += min;

 return w;
}

void clamp(int *x, int min, int max)
{
 if(*x < min) *x = min;
 if(*x > max) *x = max;
}

#define PIXEL(bitmap, x, y) PIXEL_32(bitmap, wrap(x, 0, bitmap->w), wrap(y, 0, bitmap->h))
#define GETR(c) getr32(c)
#define GETG(c) getg32(c)
#define GETB(c) getb32(c)
#define MAKECOL(r, g, b) makecol32(r, g, b)

void getrgb(int c, int *r, int *g, int *b)
{
 *r = GETR(c);
 *g = GETG(c);
 *b = GETB(c);
}

float linear_interp(float y0, float y1, float mu)
{
 return y0 * mu + y1 * (1.0 - mu);
}

float cubic_interp(float y0, float y1, float y2, float y3, float mu, float mu2, float mu3)
{
 float a0, a1, a2, a3;

 a0 = y3 - y2 - y0 + y1;
 a1 = y0 - y1 - a0;
 a2 = y2 - y0;
 a3 = y1;

 return a0 * mu3 + a1 * mu2 + a2 * mu + a3;
}

float hermite_interp(float y0, float y1, float y2, float y3, float mu, float mu2, float mu3)
{
 float m0, m1;
 float a0, a1, a2, a3, t;

 m0  = (y2 - y0) * 0.5;
 m1  = (y3 - y1) * 0.5;
 t = 3.0 * mu2;
 a0 = 2.0 * mu3 - t + 1.0;
 a1 = mu3 + mu2 + mu - t;
 a2 = mu3 - mu2;
 a3 = 1.0 - a0;

 return a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2;
}

int sample_4(BITMAP *bitmap, float x, float y, float interp(float y0, float y1, float mu))
{
 int ix = (int)x, iy = (int)y, r[5], g[5], b[5];
 float k1 = y - iy, k2 = x - ix;

 if(k1 < EPSILON && k2 > EPSILON)
  {
   getrgb(PIXEL(bitmap, ix, iy), &r[1], &g[1], &b[1]);
   getrgb(PIXEL(bitmap, ix + 1, iy), &r[2], &g[2], &b[2]);
   r[0] = interp(r[2], r[1], k2);
   g[0] = interp(g[2], g[1], k2);
   b[0] = interp(b[2], b[1], k2);
  }

 else if(k2 < EPSILON && k1 > EPSILON)
  {
   getrgb(PIXEL(bitmap, ix, iy), &r[1], &g[1], &b[1]);
   getrgb(PIXEL(bitmap, ix, iy + 1), &r[2], &g[2], &b[2]);
   r[0] = interp(r[2], r[1], k1);
   g[0] = interp(g[2], g[1], k1);
   b[0] = interp(b[2], b[1], k1);
  }

 else if(k2 < EPSILON && k1 < EPSILON)
  return PIXEL(bitmap, ix, iy);

 else
  {
   getrgb(PIXEL(bitmap, ix, iy), &r[1], &g[1], &b[1]);
   getrgb(PIXEL(bitmap, ix + 1, iy), &r[2], &g[2], &b[2]);
   getrgb(PIXEL(bitmap, ix + 1, iy + 1), &r[3], &g[3], &b[3]);
   getrgb(PIXEL(bitmap, ix, iy + 1), &r[4], &g[4], &b[4]);

   r[0] = interp(interp(r[3], r[2], k1), interp(r[4], r[1], k1), k2);
   g[0] = interp(interp(g[3], g[2], k1), interp(g[4], g[1], k1), k2);
   b[0] = interp(interp(b[3], b[2], k1), interp(b[4], b[1], k1), k2);
  }

 return MAKECOL(r[0], g[0], b[0]);
}

int sample_16(BITMAP *bitmap, float x, float y, float interp(float y0, float y1, float y2, float y3, float mu, float mu2, float mu3))
{
 int ix = (int)x, iy = (int)y, r[4], g[4], b[4], rt[5], gt[5], bt[5], i;
 float k1 = y - iy, k2 = x - ix, k1sq = k1 * k1, k1cu = k1 * k1sq, k2sq = k2 * k2, k2cu = k2 * k2sq;
 float tc = 0.0, bc = 0.0;

 if(k1 < EPSILON && k2 > EPSILON)
  for(i = 0; i < 4; i++)
   getrgb(PIXEL(bitmap, ix - 1 + i, iy), &rt[i], &gt[i], &bt[i]);

 else if(k2 < EPSILON && k1 > EPSILON)
  {
   k2 = k1;
   k2sq = k1sq;
   k2cu = k1cu;
   for(i = 0; i < 4; i++)
    getrgb(PIXEL(bitmap, ix, iy - 1 + i), &rt[i], &gt[i], &bt[i]);
  }

 else if(k1 < EPSILON && k2 < EPSILON)
  return PIXEL(bitmap, ix, iy);

 else
  for(i = 0; i < 4; i++)
   {
    getrgb(PIXEL(bitmap, ix - 1 + i, iy - 1), &r[0], &g[0], &b[0]);
    getrgb(PIXEL(bitmap, ix - 1 + i, iy),     &r[1], &g[1], &b[1]);
    getrgb(PIXEL(bitmap, ix - 1 + i, iy + 1), &r[2], &g[2], &b[2]);
    getrgb(PIXEL(bitmap, ix - 1 + i, iy + 2), &r[3], &g[3], &b[3]);
    rt[i] = interp(r[0], r[1], r[2], r[3], k1, k1sq, k1cu);
    gt[i] = interp(g[0], g[1], g[2], g[3], k1, k1sq, k1cu);
    bt[i] = interp(b[0], b[1], b[2], b[3], k1, k1sq, k1cu);
   }

 rt[4] = interp(rt[0], rt[1], rt[2], rt[3], k2, k2sq, k2cu);
 gt[4] = interp(gt[0], gt[1], gt[2], gt[3], k2, k2sq, k2cu);
 bt[4] = interp(bt[0], bt[1], bt[2], bt[3], k2, k2sq, k2cu);

 clamp(&rt[4], 0, 255);
 clamp(&gt[4], 0, 255);
 clamp(&bt[4], 0, 255);

 return MAKECOL(rt[4], gt[4], bt[4]);
}

#endif
