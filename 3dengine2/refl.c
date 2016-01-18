#include <allegro.h>
#include "vec2f.h"

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

 VEC2F a = vec2f(100.0, 130.0), b = vec2f(520.0, 370.0), v;

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }

   v = vec2f(mouse_x, mouse_y);

   VEC2F n = NORMALIZED_NORMAL_VEC2F(a, b);
   VEC2F nq = VEC2F_SUM(USCALE_VEC2F(n, 20.0), a);

   v = VEC2F_DIFF(v, a);
   VEC2F vr = VEC2F_DIFF(USCALE_VEC2F(n, 2.0 * VEC2F_DOT_PRODUCT(v, n)), v);
   vr = VEC2F_SUM(vr, a);

   clear_to_color(buffer, 0);
   circlefill(buffer, mouse_x, mouse_y, 2, makecol(255, 0, 0));
   line(buffer, a.x, a.y, b.x, b.y, makecol(255, 255, 255));
   line(buffer, a.x, a.y, nq.x, nq.y, makecol(255, 255, 255));
   line(buffer, a.x, a.y, mouse_x, mouse_y, makecol(255, 255, 255));
   line(buffer, a.x, a.y, vr.x, vr.y, makecol(255, 255, 255));
   
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
