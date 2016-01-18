#include <allegro.h>
#include "vector.h"
#include "graphics.h"
//#include "interface.h"
#include "map.h"

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

VECTOR make_wall(VECTOR a, VECTOR b, VECTOR c, float width)
{
 float m1, m2, n1, n2;
 VECTOR norm1, norm2, p, p1, p2;

 m1 = VECTOR_SLOPE(VECTOR_DIFF(a, b));
 m2 = VECTOR_SLOPE(VECTOR_DIFF(b, c));
 norm1 = USCALE_VECTOR(NORMALIZED_NORMAL(a, b), width);
 norm2 = USCALE_VECTOR(NORMALIZED_NORMAL(b, c), width);
 p1 = VECTOR_SUM(a, norm1);
 p2 = VECTOR_SUM(b, norm2);
 n1 = p1.y - m1 * p1.x;
 n2 = p2.y - m2 * p2.x;
 p.x = (n2 - n1) / (m1 - m2);
 p.y = m1 * p.x + n1;

 return p;
}

int main()
{
 int exit = 0;

 init();

 BITMAP *cursor_bmp = load_bitmap("cursor.bmp", NULL);

 VECTOR temp_shape[100];
 LINE_LOOP loop[1000];
 int n = 0, sn = 0, timeout = 100, i, j;

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
     if(key[KEY_C] && n > 2 && sn < 1000)
      {
       loop[sn].point = (VECTOR *)malloc(n * sizeof(VECTOR));
       for(i = 0; i < n; i++)
        loop[sn].point[i] = temp_shape[i];
       loop[sn].n = n;
       loop[sn].closed = 1;
       sn++;
       n = 0;
      }

     if(key[KEY_V] && n > 1 && sn < 1000)
      {
       loop[sn].point = (VECTOR *)malloc(n * sizeof(VECTOR));
       for(i = 0; i < n; i++)
        loop[sn].point[i] = temp_shape[i];
       loop[sn].n = n;
       loop[sn].closed = 0;
       sn++;
       n = 0;
      }
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

   if(sn > 0)
    for(j = 0; j < sn; j++)
     vector_loop(buffer, loop[j].point, loop[j].n, loop[j].closed, 0);
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

 float wall_width = 20.0;

 LINE_LOOP *final_shape;
 final_shape = (LINE_LOOP *)malloc(sn * sizeof(LINE_LOOP));

 if(sn > 0)
  for(j = 0; j < sn; j++)
   {
    if(loop[j].n > 2 && !loop[j].closed)
     {
      VECTOR norm;
      int pn = loop[j].n;
      final_shape[j] = new_shape(pn * 2, 0);

      for(i = 0; i < pn; i++)
       final_shape[j].point[i] = loop[j].point[i];

      norm = NORMALIZED_NORMAL(loop[j].point[0], loop[j].point[1]);
      final_shape[j].point[pn] = VECTOR_SUM(loop[j].point[0], USCALE_VECTOR(norm, wall_width));
      norm = NORMALIZED_NORMAL(loop[j].point[pn - 2], loop[j].point[pn - 1]);
      final_shape[j].point[pn * 2 - 1] = VECTOR_SUM(loop[j].point[pn - 1], USCALE_VECTOR(norm, wall_width));

      for(i = 1; i < pn - 1; i++)
       final_shape[j].point[pn + i] = make_wall(loop[j].point[i - 1], loop[j].point[i], loop[j].point[i + 1], wall_width);
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n;
      final_shape[j] = new_shape(pn, 1);
      for(i = 0; i < pn; i++)
       final_shape[j].point[i] = loop[j].point[i];
     }

    if(loop[j].n == 2)
     {
      final_shape[j] = new_shape(4, 0);
      VECTOR norm = USCALE_VECTOR(NORMALIZED_NORMAL(loop[j].point[0], loop[j].point[1]), wall_width);
      final_shape[j].point[0] = loop[j].point[0];
      final_shape[j].point[1] = loop[j].point[1];
      final_shape[j].point[2] = VECTOR_SUM(loop[j].point[0], norm);
      final_shape[j].point[3] = VECTOR_SUM(loop[j].point[1], norm);
     }
   }

 draw_map_sketch(buffer, final_shape, sn);
 save_map("map.txt", final_shape, sn);

 blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 readkey();

 free_map(final_shape, sn);
 for(i = 0; i < sn; i++)
  free(loop[i].point);
 destroy_bitmap(cursor_bmp);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
