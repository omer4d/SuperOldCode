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

int color_blend(int c1, int c2, float a)
{
 int r = getr(c1) + getr(c2) * a;// * (1.0 - a);
 int g = getg(c1) + getg(c2) * a;// * (1.0 - a);
 int b = getb(c1) + getb(c2) * a;// * (1.0 - a);

 if(r > 255)
  r = 255;
 if(g > 255)
  g = 255;
 if(b > 255)
  b = 255;
 
 return makecol(r, g, b);
}

int alpha_blend(int c1, int c2, float a)
{
 int r = getr(c1) * a + getr(c2) * (1.0 - a);
 int g = getg(c1) * a + getg(c2) * (1.0 - a);
 int b = getb(c1) * a + getb(c2) * (1.0 - a);
 return makecol(r, g, b);
}

void draw_brush(BITMAP *buffer, BITMAP *brush, int x, int y, int c)
{
 int i, j, c1, c2;

 for(j = 0; j < brush->h; j++)
  for(i = 0; i < brush->w; i++)
   {
    c1 = getpixel(buffer, i + x, j + y);
    c2 = alpha_blend(c, 0, getr(getpixel(brush, i, j)) / 255.0);
    putpixel(buffer, i + x, j + y, color_blend(c1, c2, 0.1));
   }
}

int main()
{
 int exit = 0;
 BITMAP *brush;

 init();

 brush = load_bitmap("small_brush.bmp", NULL);

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

 int c = makecol(255, 255, 255);
 

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
       draw_brush(buffer, brush, center.x + p1.x - brush->w/2, center.y + p1.y - brush->h/2, c);
       draw_brush(buffer, brush, center.x + p2.x - brush->w/2, center.y + p2.y - brush->h/2, c);
       /*
       putpixel(buffer, center.x + p1.x, center.y + p1.y, makecol(255, 255, 255));
       putpixel(buffer, center.x + p2.x, center.y + p2.y, makecol(255, 255, 255));
       */
      }
    }
    
    if(mouse_b == 2)
     {
      c = makecol(rand()%255, rand()%255, rand()%255);
      mouse_b = 0;
     }

   //clear_to_color(buffer, 0);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   
   //for(i = 0; i < ARM_NUMBER; i++)
    //line(screen, center.x, center.y, center.x + arm[i].x * 100.0, center.y + arm[i].y * 100.0, makecol(255, 0, 0));
   
   textprintf_ex(screen, font, 20, 10, makecol(255, 255, 255), -1, "Left mouse button: draw stuff.");
   textprintf_ex(screen, font, 20, 20, makecol(255, 255, 255), -1, "Right mouse button: get a new, random color.");
   circlefill(screen, mouse_x, mouse_y, 2, c);
  }

 destroy_bitmap(brush);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
