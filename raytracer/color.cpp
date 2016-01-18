#include "color.h"

int clamp_0_255(int k)
{
 if(k > 255) return 255;
 if(k < 0) return 0;
 return k;
}

cColor::cColor()
{
 r = 0.0f;
 g = 0.0f;
 b = 0.0f;
}

cColor cColor::operator + (cColor param)
{
 cColor temp;

 temp.r = r + param.r;
 temp.g = g + param.g;
 temp.b = b + param.b;

 return temp;
}

cColor cColor::operator - (cColor param)
{
 cColor temp;

 temp.r = r - param.r;
 temp.g = g - param.g;
 temp.b = b - param.b;

 return temp;
}

cColor cColor::operator * (float param)
{
 cColor temp;

 temp.r = r * param;
 temp.g = g * param;
 temp.b = b * param;

 return temp;
}

int cColor::c()
{
 return makecol(clamp_0_255((int)r), clamp_0_255((int)g), clamp_0_255((int)b));
}

cColor color(float a, float b, float c)
{
 cColor temp;

 temp.r = a;
 temp.g = b;
 temp.b = c;

 return temp;
}
