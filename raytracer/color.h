#ifndef _COLOR_H_
#define _COLOR_H_

#include <allegro.h>

class cColor {
 public:
  float r, g, b;

  cColor();
  cColor operator + (cColor);
  cColor operator - (cColor);
  cColor operator * (float);
  int c();
};

int clamp_0_255(int k);
cColor color(float, float, float);

#endif
