#ifndef _MAT16F_H_
#define _MAT16F_H_

#include <math.h>
#include "vec3f.h"

typedef float MAT16F[4][4];

void copy_mat16f(MAT16F dest, MAT16F source)
{
 int i, j;

 for(i = 0; i < 4; i++)
  for(j = 0; j < 4; j++)
   dest[i][j] = source[i][j];
}

void mat16f_product(MAT16F dest, MAT16F mat1, MAT16F mat2)
{
 int i, j;

 for(i = 0; i < 4; i++)
  for(j = 0; j < 4; j++)
   dest[i][j] = mat1[i][0] * mat2[0][j] +
                mat1[i][1] * mat2[1][j] +
                mat1[i][2] * mat2[2][j] +
                mat1[i][3] * mat2[3][j];
}

void transform_vec3f(VEC3F *dest, VEC3F source, MAT16F mat)
{
 dest->x = source.x * mat[0][0] +
           source.y * mat[1][0] +
           source.z * mat[2][0] +
                      mat[3][0];
 dest->y = source.x * mat[0][1] +
           source.y * mat[1][1] +
           source.z * mat[2][1] +
                      mat[3][1];
 dest->z = source.x * mat[0][2] +
           source.y * mat[1][2] +
           source.z * mat[2][2] +
                      mat[3][2];
}

void reset_mat16f(MAT16F mat)
{
 mat[0][0] = 1.0; mat[0][1] = 0.0; mat[0][2] = 0.0; mat[0][3] = 0.0;
 mat[1][0] = 0.0; mat[1][1] = 1.0; mat[1][2] = 0.0; mat[1][3] = 0.0;
 mat[2][0] = 0.0; mat[2][1] = 0.0; mat[2][2] = 1.0; mat[2][3] = 0.0;
 mat[3][0] = 0.0; mat[3][1] = 0.0; mat[3][2] = 0.0; mat[3][3] = 1.0;
}

void translate_mat16f(MAT16F matrix, float tx, float ty, float tz)
{
 MAT16F tmat, temp;

 tmat[0][0] = 1.0; tmat[0][1] = 0.0; tmat[0][2] = 0.0; tmat[0][3] = 0.0;
 tmat[1][0] = 0.0; tmat[1][1] = 1.0; tmat[1][2] = 0.0; tmat[1][3] = 0.0;
 tmat[2][0] = 0.0; tmat[2][1] = 0.0; tmat[2][2] = 1.0; tmat[2][3] = 0.0;
 tmat[3][0] = tx;  tmat[3][1] = ty;  tmat[3][2] = tz;  tmat[3][3] = 1.0;

 mat16f_product(temp, matrix, tmat);
 copy_mat16f(matrix, temp);
}

void rotate_mat16f(MAT16F matrix, float ax, float ay, float az)
{
 MAT16F xmat, ymat, zmat, mat1, mat2;

 xmat[0][0] = 1.0; xmat[0][1] = 0.0;      xmat[0][2] = 0.0;     xmat[0][3] = 0.0;
 xmat[1][0] = 0.0; xmat[1][1] = cos(ax);  xmat[1][2] = sin(ax); xmat[1][3] = 0.0;
 xmat[2][0] = 0.0; xmat[2][1] = -sin(ax); xmat[2][2] = cos(ax); xmat[2][3] = 0.0;
 xmat[3][0] = 0.0; xmat[3][1] = 0.0;      xmat[3][2] = 0.0;     xmat[3][3] = 1.0;
 
 ymat[0][0] = cos(ay);  ymat[0][1] = 0.0; ymat[0][2] = sin(ay);  ymat[0][3] = 0.0;
 ymat[1][0] = 0.0;      ymat[1][1] = 1.0; ymat[1][2] = 0.0;      ymat[1][3] = 0.0;
 ymat[2][0] = -sin(ay); ymat[2][1] = 0.0; ymat[2][2] = cos(ay);  ymat[2][3] = 0.0;
 ymat[3][0] = 0.0;      ymat[3][1] = 0.0; ymat[3][2] = 0.0;      ymat[3][3] = 1.0;

 zmat[0][0] = cos(az);  zmat[0][1] = sin(az); zmat[0][2] = 0.0; zmat[0][3] = 0.0;
 zmat[1][0] = -sin(az); zmat[1][1] = cos(az); zmat[1][2] = 0.0; zmat[1][3] = 0.0;
 zmat[2][0] = 0.0;      zmat[2][1] = 0.0;     zmat[2][2] = 1.0; zmat[2][3] = 0.0;
 zmat[3][0] = 0.0;      zmat[3][1] = 0.0;     zmat[3][2] = 0.0; zmat[3][3] = 1.0;

 mat16f_product(mat1, matrix, zmat);
 mat16f_product(mat2, mat1, ymat);
 mat16f_product(matrix, mat2, xmat);
}

#endif
