#include <allegro.h>
#include "vec3f.h"
#include "tgspace.h"

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


int main()
{
 int exit = 0;

 init();

 VEC3F wp1 = vec3f(100.0, 100.0, 10.0), wp2 = vec3f(130.0, 100.0, 100.0), wp3 = vec3f(200.0, 200.0, 80.0);
 VEC3F tp1 = vec3f(0.0, 0.0, 0.0), tp2 = vec3f(1.0, 0.0, 0.0), tp3 = vec3f(1.0, 1.0, 0.0);
 VEC3F U, V, W;

 get_axes(wp1, wp2, wp3, tp1, tp2, tp3, &U, &V, &W);

 while(!keypressed())
  {
   VEC3F lw = vec3f(10.0, 5.0, 17.0);

   wp1 = vec3f(rand() % 500, rand() % 500, rand() % 500);
   wp2 = vec3f(rand() % 500, rand() % 500, rand() % 500);
   wp3 = vec3f(rand() % 500, rand() % 500, rand() % 500);

   get_axes(wp1, wp2, wp3, tp1, tp2, tp3, &U, &V, &W);

   float k1 = (float)rand() / RAND_MAX * 0.5, k2 = (float)rand() / RAND_MAX * 0.5;
   VEC3F pw = bary(wp1, wp2, wp3, k1, k2);

   VEC3F n = NORMALIZED_VEC3F(vec3f_normal(wp1, wp2, wp3));
   float cos_alpha = VEC3F_DOT_PRODUCT(n, NORMALIZED_VEC3F(VEC3F_DIFF(lw, pw)));

   VEC3F lt, pt;
   transform_to_space(U, V, W, wp1, tp1, lw, &lt);
   transform_to_space(U, V, W, wp1, tp1, pw, &pt);

   VEC3F rpt = bary(tp1, tp2, tp3, k1, k2);
   float cos_alpha2 = VEC3F_DOT_PRODUCT(vec3f(0.0, 0.0, 1.0), NORMALIZED_VEC3F(VEC3F_DIFF(lt, pt)));

   //if(cos_alpha - cos_alpha2 > M_PI / 180.0)
   
   float error = (acos(fabs(cos_alpha)) - acos(fabs(cos_alpha2))) / (M_PI / 180.0);

   if(error > 50.0)
    {
     VEC3F wc = bary(wp1, wp2, wp3, 0.24, 0.32);
     lw = VEC3F_SUM(wc, USCALE_VEC3F(n, 10.0));
     
     pw = wp1;
     cos_alpha = VEC3F_DOT_PRODUCT(n, NORMALIZED_VEC3F(VEC3F_DIFF(lw, pw)));
     transform_to_space(U, V, W, wp1, tp1, lw, &lt);
     transform_to_space(U, V, W, wp1, tp1, pw, &pt);

     cos_alpha2 = VEC3F_DOT_PRODUCT(vec3f(0.0, 0.0, 1.0), NORMALIZED_VEC3F(VEC3F_DIFF(lt, pt)));
     printf("%f\n", (acos(fabs(cos_alpha)) - acos(fabs(cos_alpha2))) / (M_PI / 180.0)); 
    }
  }

 //readkey();
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
