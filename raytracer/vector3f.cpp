#include "vector3f.h"

cVector3f::cVector3f()
{
 x = 0.0f;
 y = 0.0f;
 z = 0.0f;
}

cVector3f cVector3f::operator + (cVector3f param)
{
 cVector3f temp;

 temp.x = x + param.x;
 temp.y = y + param.y;
 temp.z = z + param.z;

 return temp;
}

cVector3f cVector3f::operator - (cVector3f param)
{
 cVector3f temp;

 temp.x = x - param.x;
 temp.y = y - param.y;
 temp.z = z - param.z;

 return temp;
}

cVector3f cVector3f::operator * (float param)
{
 cVector3f temp;

 temp.x = x * param;
 temp.y = y * param;
 temp.z = z * param;

 return temp;
}

float cVector3f::length()
{
 return sqrt(x * x + y * y + z * z);
}

void cVector3f::normalize()
{
 float inv_l = 1.0 / length();

 x *= inv_l;
 y *= inv_l;
 z *= inv_l;
}

cVector3f vector3f(float a, float b, float c)
{
 cVector3f temp;

 temp.x = a;
 temp.y = b;
 temp.z = c;

 return temp;
}
