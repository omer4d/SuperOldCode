#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "base.h"

void (*rasterize_sides)(int, int, float [], float [], float, float *, float []);
int REND_INT_flat_col;

void set_rasterizer_function(void func(int, int, float [], float [], float, float *, float []))
{
 rasterize_sides = func;
}

void set_flat_color(int col)
{
 REND_INT_flat_col = col;
}

void rend_int_point_hsort(REND_INT_POINT v[], int n)
{
 int i, j;
 REND_INT_POINT temp;

 for(i = n - 1; i >= 0; i--)
  for(j = 1; j <= i; j++)
   if(v[j - 1].y > v[j].y)
    {
     temp = v[j - 1];
     v[j - 1] = v[j];
     v[j] = temp;
    }
}

void rasterize_triangle(REND_INT_POINT tri[3])
{
 int i, l, r, miny, maxy;
 float didy1[MAX_AFFINE_INTERP], accum1[MAX_AFFINE_INTERP];
 float dxdy2, accum2;
 float didx[MAX_AFFINE_INTERP - 1];
 float k1, k2;
 float temp_x;
 float magic[6];

 if(fabs(tri[0].y - tri[1].y) + fabs(tri[0].y - tri[2].y) < FLOAT_EPSILON)
  return;

 rend_int_point_hsort(tri, 3);

 if(fabs(tri[0].y - tri[1].y) < FLOAT_EPSILON)
  {
   if(tri[0].affine_interp[0] < tri[1].affine_interp[0]) { l = 0; r = 1; }
   else { l = 1; r = 0; }

   miny = (int)ceil(tri[0].y);
   maxy = (int)ceil(tri[2].y) - 1;
   k1 = 1.0 / (tri[2].y - tri[l].y);
   k2 = 1.0 / (tri[2].y - tri[r].y);

   for(i = MAX_AFFINE_INTERP - 1; i > -1; i--)
    {
     didy1[i] = (tri[2].affine_interp[i] - tri[l].affine_interp[i]) * k1;
     accum1[i] = tri[2].affine_interp[i] + (miny - tri[2].y) * didy1[i];
    }

   dxdy2 = (tri[2].affine_interp[0] - tri[r].affine_interp[0]) * k2;
   accum2 = tri[2].affine_interp[0] + (miny - tri[2].y) * dxdy2;
   k1 = 1.0 / (didy1[0] - dxdy2);

   for(i = 0; i < MAX_AFFINE_INTERP - 1; i++)
    didx[i] = (didy1[1 + i] - (tri[2].affine_interp[1 + i] - tri[r].affine_interp[1 + i]) * k2) * k1;

   rasterize_sides(miny, maxy, didy1, accum1, dxdy2, &accum2, didx);
   return;
  }

 temp_x = (tri[1].y * (tri[0].affine_interp[0] - tri[2].affine_interp[0]) +
           tri[0].y * tri[2].affine_interp[0] - tri[0].affine_interp[0] * tri[2].y) /
          (tri[0].y - tri[2].y);

 if(tri[1].affine_interp[0] < temp_x) { l = 1; r = 2; }
 else { l = 2; r = 1; }

 miny = (int)ceil(tri[0].y);
 maxy = (int)ceil(tri[1].y) - 1;
 k1 = 1.0 / (tri[l].y - tri[0].y);
 k2 = 1.0 / (tri[r].y - tri[0].y);

 for(i = MAX_AFFINE_INTERP - 1; i > -1; i--)
  {
   didy1[i] = (tri[l].affine_interp[i] - tri[0].affine_interp[i]) * k1;
   accum1[i] = tri[0].affine_interp[i] + (miny - tri[0].y) * didy1[i];
  }

 dxdy2 = (tri[r].affine_interp[0] - tri[0].affine_interp[0]) * k2;
 accum2 = tri[0].affine_interp[0] + (miny - tri[0].y) * dxdy2;

 if(fabs(didy1[0] - dxdy2) < FLOAT_EPSILON)
  return;

 k1 = 1.0 / (didy1[0] - dxdy2);

 for(i = 0; i < MAX_AFFINE_INTERP - 1; i++)
  didx[i] = (didy1[1 + i] - (tri[r].affine_interp[1 + i] - tri[0].affine_interp[1 + i]) * k2) * k1;

 rasterize_sides(miny, maxy, didy1, accum1, dxdy2, &accum2, didx);

 miny = (int)ceil(tri[1].y), maxy = (int)ceil(tri[2].y) - 1;

 if(fabs(tri[2].y - tri[1].y) < FLOAT_EPSILON)
  return;
 k1 = 1.0 / (tri[2].y - tri[1].y);

 if(r == 2)
  for(i = MAX_AFFINE_INTERP - 1; i > -1; i--)
   {
    didy1[i] = (tri[2].affine_interp[i] - tri[1].affine_interp[i]) * k1;
    accum1[i] = tri[1].affine_interp[i] + (miny - tri[1].y) * didy1[i];
   }

 else
  {
   dxdy2 = (tri[2].affine_interp[0] - tri[1].affine_interp[0]) * k1;
   accum2 = tri[1].affine_interp[0] + (miny - tri[1].y) * dxdy2;
  }

 rasterize_sides(miny, maxy, didy1, accum1, dxdy2, &accum2, didx);
}

void flat_hline(int y, float x2, float interp1[], float didx[])
{    
 int x, minx, maxx;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;
 
 int orig;

 for(x = minx; x < maxx + 1; x++)
  {
   //orig = ((long *)BASE_INT_color_buffer->line[y])[x];
   ((long *)BASE_INT_color_buffer->line[y])[x] = REND_INT_flat_col;//additive_blend(orig, REND_INT_flat_col);
  }
}

void flat_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   flat_hline(y, *accum2, accum1, didx);
   accum1[0] += didy1[0];
   (*accum2) += dxdy2;
  }
}

void flat_zbuff_hline(int y, float x2, float interp1[], float didx[])
{
 int x, minx, maxx;
 float curr;
 long z;
 float rz;

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 curr = interp1[1] + (minx - interp1[0]) * didx[0];

 for(x = minx; x < maxx + 1; x++)
  {
   rz = 1.0 / curr;
   z = (long)(rz * BASE_INT_z_buffer_precision);
   if(z < RI_PIXEL_Z(x, y))
    {
     //rz =;
     RI_PIXEL(x, y) = REND_INT_flat_col;//fade_to_col(makecol(255, 255, 255), rz, 0);
     RI_PIXEL_Z(x, y) = z;
    }
   curr += didx[0];
  }
}

void flat_zbuff_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   flat_zbuff_hline(y, *accum2, accum1, didx);
   accum1[0] += didy1[0];
   accum1[1] += didy1[1];
   (*accum2) += dxdy2;
  }
}

#endif
