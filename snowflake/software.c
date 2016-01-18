#include <allegro.h>
#include "vector.h"

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

#define ARM_NUMBER 6

int main()
{
 int exit = 0;

 init();

 VECTOR center = vector(320.0, 240.0), point;
 VECTOR v = vector(cos(0.0), sin(0.0)), n = VECTOR_NORMAL(v);

 VECTOR arm[ARM_NUMBER], arm_normal[ARM_NUMBER];
 int i;

 for(i = 0; i < ARM_NUMBER; i++)
  {
   arm[i].x = cos(i * 2.0 * M_PI / ARM_NUMBER);
   arm[i].y = sin(i * 2.0 * M_PI / ARM_NUMBER);
   arm_normal[i] = VECTOR_NORMAL(arm[i]);
  }

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
    }

   if(mouse_b == 1)
    {
     point = vector(center.x-mouse_x, center.y-mouse_y);
     //putpixel(buffer, mouse_x, mouse_y, makecol(255, 255, 255));

     for(i = 0; i < ARM_NUMBER; i++)
      {
       VECTOR p1 = VECTOR_SUM(USCALE_VECTOR(arm[i], point.x), USCALE_VECTOR(arm_normal[i], -point.y));
       VECTOR p2 = VECTOR_SUM(USCALE_VECTOR(arm[i], point.x), USCALE_VECTOR(arm_normal[i], point.y));
       putpixel(buffer, center.x + p1.x, center.y + p1.y, makecol(255, 255, 255));
       putpixel(buffer, center.x + p2.x, center.y + p2.y, makecol(255, 255, 255));
      }
    }

   //clear_to_color(buffer, 0);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   
   //for(i = 0; i < ARM_NUMBER; i++)
    //line(screen, center.x, center.y, center.x + arm[i].x * 100.0, center.y + arm[i].y * 100.0, makecol(255, 0, 0));

   circlefill(screen, mouse_x, mouse_y, 2, makecol(128, 0, 0));
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
