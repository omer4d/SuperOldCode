#include "vector.h"
#include "graphics.h"
#include "map.h"
#include <allegro.h>

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

int convex_polygon(VECTOR *p, int n)
{
 int i, j, k;
 int flag = 0;
 float z;

 if(n < 3)
  return 0;

 for(i = 0; i < n; i++)
  {
   j = (i + 1) % n;
   k = (i + 2) % n;
   z = (p[j].x - p[i].x) * (p[k].y - p[j].y);
   z -= (p[j].y - p[i].y) * (p[k].x - p[j].x);

   if(z < 0) { flag |= 1; }
   else if(z > 0) { flag |= 2; }
   if(flag == 3) { return -1; }
  }

 if(flag != 0) { return 1; }
 return 0;
}

float signed_polygon_area(VECTOR *p, int n)
{
 int i, j;
 float area = 0;

 for(i = 0; i < n; i++)
  {
   j = (i + 1) % n;
   area += p[i].x * p[j].y;
   area -= p[i].y * p[j].x;
  }

 area *= 0.5;
 return area;
}

void reverse_polygon_winding(VECTOR *p, int n)
{
 int i;
 VECTOR temp;

 for(i = 0; i < n / 2; i++)
  {
   temp = p[i];
   p[i] = p[n - 1- i];
   p[n - 1 - i] = temp;
  }
}

int main()
{
 int exit = 0;

 init();

 BITMAP *cursor_bmp = load_bitmap("cursor.bmp", NULL);

 VECTOR temp_shape[100];
 int n = 0;
 int timeout = 0;

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
    }

   VECTOR cursor;
   cursor.x = (mouse_x / 5) * 5;
   cursor.y = (mouse_y / 5) * 5;
   timeout--;

   if(mouse_b == 1 && timeout < 1 && n < 100)
    {
     int error_flag = 0;
     timeout = 100;

     if(n > 0)
      if(sq_dist(cursor, temp_shape[n - 1]) < 25)
       error_flag = 1;

     if(n > 1)
      if(fabs(VECTOR_SLOPE(VECTOR_DIFF(temp_shape[n - 1], temp_shape[n - 2])) -
              VECTOR_SLOPE(VECTOR_DIFF(temp_shape[n - 1], cursor))) < 0.2 ||
         (temp_shape[n - 1].x == temp_shape[n - 2].x && temp_shape[n - 2].x == cursor.x))
       error_flag = 1;

     if(!error_flag)
      {
       temp_shape[n] = cursor;
       n++;
      }
    }

   clear_to_color(buffer, makecol(128, 128, 128));

   vector_loop(buffer, temp_shape, n, -2, 0);
 
   if(n > 1)
    {
     vector_loop(buffer, temp_shape, n, 0, 0);
     vector_line(buffer, temp_shape[n - 1], temp_shape[0], makecol(128, 0, 0));
    }

   if(n > 0)
    {
     vector_line(buffer, temp_shape[n - 1], cursor, makecol(0, 128, 128));
     vector_line(buffer, cursor, temp_shape[0], makecol(0, 128, 128));
    }

   draw_sprite(buffer, cursor_bmp, cursor.x - 4, cursor.y - 5);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 clear_keybuf();
 clear_to_color(buffer, makecol(128, 128, 128));

 printf("%d, %f", convex_polygon(temp_shape, n), signed_polygon_area(temp_shape, n));
 reverse_polygon_winding(temp_shape, n);
 printf(", %f", signed_polygon_area(temp_shape, n));

 blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 readkey();

 destroy_bitmap(cursor_bmp);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
