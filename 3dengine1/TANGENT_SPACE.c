#include <allegro.h>
#include "vec3f.h"

BITMAP *buffer;

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void get_axes(VEC3F wp1, VEC3F wp2, VEC3F wp3, VEC3F tp1, VEC3F tp2, VEC3F tp3, VEC3F *A, VEC3F *B, VEC3F *C)
{
 VEC3F wd1, wd2, wd3, td1, td2, td3;
 VEC3F wn = NORMALIZED_VEC3F(vec3f_normal(wp1, wp2, wp3)), tn = NORMALIZED_VEC3F(vec3f_normal(tp1, tp2, tp3));
 float k;

 wd1 = VEC3F_DIFF(wp1, wp2);
 wd2 = VEC3F_DIFF(wp2, wp3);
 wd3 = wn;

 td1 = VEC3F_DIFF(tp1, tp2);
 td2 = VEC3F_DIFF(tp2, tp3);
 td3 = tn;

 k = 1.0 / (wd1.x * (wd2.y * wd3.z - wd3.y * wd2.z) -
            wd2.x * (wd1.y * wd3.z - wd3.y * wd1.z) -
            wd3.x * (wd2.y * wd1.z - wd1.y * wd2.z));

 A->x =  (wd1.y * (wd2.z * td3.x - wd3.z * td2.x) +
          wd1.z * (wd3.y * td2.x - wd2.y * td3.x) +
          td1.x * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 A->y = -(wd2.x * (wd3.z * td1.x - wd1.z * td3.x) +
          wd2.z * (wd1.x * td3.x - wd3.x * td1.x) +
          td2.x * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 A->z =  (wd3.y * (wd2.x * td1.x - wd1.x * td2.x) +
          wd2.y * (wd1.x * td3.x - wd3.x * td1.x) -
          wd1.y * (wd2.x * td3.x - wd3.x * td2.x)) * k;

 B->x =  (wd1.y * (wd2.z * td3.y - wd3.z * td2.y) +
          wd1.z * (wd3.y * td2.y - wd2.y * td3.y) +
          td1.y * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 B->y = -(wd2.x * (wd3.z * td1.y - wd1.z * td3.y) +
          wd2.z * (wd1.x * td3.y - wd3.x * td1.y) +
          td2.y * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 B->z =  (wd3.y * (wd2.x * td1.y - wd1.x * td2.y) +
          wd2.y * (wd1.x * td3.y - wd3.x * td1.y) -
          wd1.y * (wd2.x * td3.y - wd3.x * td2.y)) * k;

 C->x =  (wd1.y * (wd2.z * td3.z - wd3.z * td2.z) +
          wd1.z * (wd3.y * td2.z - wd2.y * td3.z) +
          td1.z * (wd2.y * wd3.z - wd3.y * wd2.z)) * k;
 C->y = -(wd2.x * (wd3.z * td1.z - wd1.z * td3.z) +
          wd2.z * (wd1.x * td3.z - wd3.x * td1.z) +
          td2.z * (wd1.z * wd3.x - wd1.x * wd3.z)) * k;
 C->z =  (wd3.y * (wd2.x * td1.z - wd1.x * td2.z) +
          wd2.y * (wd1.x * td3.z - wd3.x * td1.z) -
          wd1.y * (wd2.x * td3.z - wd3.x * td2.z)) * k;
}

void transform_to_space(VEC3F A, VEC3F B, VEC3F C, VEC3F wp1, VEC3F tp1, VEC3F wp, VEC3F *tp)
{
 VEC3F v = VEC3F_DIFF(wp, wp1);
 tp->x = tp1.x + VEC3F_DOT_PRODUCT(v, A);
 tp->y = tp1.y + VEC3F_DOT_PRODUCT(v, B);
 tp->z = tp1.z + VEC3F_DOT_PRODUCT(v, C);
}

VEC3F bary(VEC3F a, VEC3F b, VEC3F c, float k1, float k2)
{
 return vec3f(k1 * a.x + k2 * b.x + (1.0 - k1 - k2) * c.x,
              k1 * a.y + k2 * b.y + (1.0 - k1 - k2) * c.y,
              k1 * a.z + k2 * b.z + (1.0 - k1 - k2) * c.z);
}

int main()
{
 int exit = 0;

 init();

 VEC3F wp1 = vec3f(100.0, 100.0, 100.0), wp2 = vec3f(200.0, 100.0, 100.0), wp3 = vec3f(200.0, 200.0, 100.0);
 VEC3F tp1 = vec3f(0.0, 0.0, 0.0), tp2 = vec3f(1.0, 0.0, 0.0), tp3 = vec3f(1.0, 1.0, 0.0);
 VEC3F U, V, W;
 VEC3F wp4, tp4, tp4_real;

 while(!exit)
  {
   float k1 = (float)rand() / RAND_MAX, k2 = (float)rand() / RAND_MAX;
   get_axes(wp1, wp2, wp3, tp1, tp2, tp3, &U, &V, &W);
   
   //wp4 = bary(wp1, wp2, wp3, k1, k2);
   //tp4_real = bary(tp1, tp2, tp3, k1, k2);
   VEC3F norm = USCALE_VEC3F(vec3f_normal(wp1, wp2, wp3), 10.0);
   wp4 = VEC3F_SUM(wp3, norm);

   transform_to_space(U, V, W, wp1, tp1, wp4, &tp4);
   printf("%f %f %f\n", tp4.x, tp4.y, tp4.z);
   
   //float error = fabs(tp4.x - tp4_real.x) + fabs(tp4.y - tp4_real.y) + fabs(tp4.z - tp4_real.z);
   //printf("%f\n", error);

   if(key[KEY_ESC]) { exit = 1; }

   clear_to_color(buffer, 0);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
