#include <allegro.h>
#include "vec2f.h"
#include "vec2f_graph.h"

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
 VEC2F a = vec2f(100.0, 100.0), b = vec2f(200.0, 120.0), c = vec2f(140.0, 300.0), p;

 init();

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
    }

   clear_to_color(buffer, 0);
   vec2f_line(buffer, a, b, makecol(255, 255, 255));
   vec2f_line(buffer, b, c, makecol(255, 255, 255));
   vec2f_line(buffer, c, a, makecol(255, 255, 255));

   p = vec2f(mouse_x, mouse_y);
   VEC2F vab = VEC2F_DIFF(b, a), vac = VEC2F_DIFF(c, a), vap = VEC2F_DIFF(p, a);
   VEC2F vbc = VEC2F_DIFF(c, b), vbp = VEC2F_DIFF(p, b);

   float ab = VEC2F_DOT_PRODUCT(vab, vab), ac = VEC2F_DOT_PRODUCT(vac, vac), bc = VEC2F_DOT_PRODUCT(vbc, vbc);
   float k1 = VEC2F_DOT_PRODUCT(vab, vap), k2 = VEC2F_DOT_PRODUCT(vac, vap), k3 = VEC2F_DOT_PRODUCT(vbc, vap);

   if(k1 > 0.0 && k1 < ab && k2 > 0.0 && k2 < ac && k3 > 0.0 && k3 < bc)
    vec2f_point(buffer, p, 3, makecol(0, 255, 0));
   else
    vec2f_point(buffer, p, 3, makecol(255, 0, 0));

   vec2f_point(buffer, b, 3, makecol(0, 0, 255));
   vec2f_point(buffer, c, 3, makecol(0, 0, 255));

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
