#include <allegro.h>
#include "vec2f.h"
#include "vec2f_graph.h"
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

VEC2F make_wall(VEC2F a, VEC2F b, VEC2F c, float width)
{
 float m1, m2, n1, n2;
 VEC2F norm1, norm2, p, p1, p2;

 m1 = VEC2F_SLOPE(VEC2F_DIFF(a, b));
 m2 = VEC2F_SLOPE(VEC2F_DIFF(b, c));
 norm1 = USCALE_VEC2F(NORMALIZED_NORMAL_VEC2F(a, b), width);
 norm2 = USCALE_VEC2F(NORMALIZED_NORMAL_VEC2F(b, c), width);
 p1 = VEC2F_SUM(a, norm1);
 p2 = VEC2F_SUM(b, norm2);
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

 VEC2F temp_shape[100];
 LINE_LOOP loop[1000];
 int n = 0, sn = 0, timeout = 100, i, j;
 int vertex_count = 0, segment_count = 0;

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
     if(key[KEY_C] && n > 2 && sn < 1000)
      {
       vertex_count += n;
       segment_count += n;
       loop[sn].point = (VEC2F *)malloc(n * sizeof(VEC2F));
       for(i = 0; i < n; i++)
        loop[sn].point[i] = temp_shape[i];
       loop[sn].n = n;
       loop[sn].closed = 1;
       sn++;
       n = 0;
      }

     if(key[KEY_V] && n > 1 && sn < 1000)
      {
       if(n > 2)
        {   
         vertex_count += 2 * n;
         segment_count += 2 * n;
        }
       else
        {
         vertex_count += 2;
         segment_count += 1;
        }
       
       loop[sn].point = (VEC2F *)malloc(n * sizeof(VEC2F));
       for(i = 0; i < n; i++)
        loop[sn].point[i] = temp_shape[i];
       loop[sn].n = n;
       loop[sn].closed = 0;
       sn++;
       n = 0;
      }
    }

   VEC2F cursor;
   cursor.x = (mouse_x / 5) * 5;
   cursor.y = (mouse_y / 5) * 5;
   timeout--;

   if(mouse_b == 1 && timeout < 1 && n < 100)
    {
     int error_flag = 0;
     timeout = 100;

     if(n > 0)
      if(vec2f_sq_dist(cursor, temp_shape[n - 1]) < 25)
       error_flag = 1;

     if(n > 1)
      if(fabs(VEC2F_SLOPE(VEC2F_DIFF(temp_shape[n - 1], temp_shape[n - 2])) -
              VEC2F_SLOPE(VEC2F_DIFF(temp_shape[n - 1], cursor))) < 0.2 ||
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
     vec2f_loop(buffer, loop[j].point, loop[j].n, loop[j].closed, 0);
    vec2f_loop(buffer, temp_shape, n, -2, 0);

   if(n > 1)
    {
     vec2f_loop(buffer, temp_shape, n, 0, 0);
     vec2f_line(buffer, temp_shape[n - 1], temp_shape[0], makecol(128, 0, 0));
    }

   if(n > 0)
    {
     vec2f_line(buffer, temp_shape[n - 1], cursor, makecol(0, 128, 128));
     vec2f_line(buffer, cursor, temp_shape[0], makecol(0, 128, 128));
    }

   draw_sprite(buffer, cursor_bmp, cursor.x - 4, cursor.y - 5);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 clear_keybuf();
 clear_to_color(buffer, makecol(128, 128, 128));

 float wall_width = 20.0;

 VEC2F *map_vertex;
 WALL_SEGMENT *map_wall_segment;

 map_vertex = (VEC2F *)malloc(vertex_count * sizeof(VEC2F));
 map_wall_segment = (WALL_SEGMENT *)malloc(segment_count * sizeof(WALL_SEGMENT));

 int k = 0, s = 0;

 if(sn > 0)
  for(j = 0; j < sn; j++)
   {
    if(loop[j].n > 2 && !loop[j].closed)
     {
      VEC2F norm;
      int pn = loop[j].n;
      int start = k;

      for(i = 0; i < pn; i++)
       map_vertex[k + i] = loop[j].point[i];

      norm = NORMALIZED_NORMAL_VEC2F(loop[j].point[0], loop[j].point[1]);
      map_vertex[k + pn * 2 - 1] = VEC2F_SUM(loop[j].point[0], USCALE_VEC2F(norm, wall_width));
      norm = NORMALIZED_NORMAL_VEC2F(loop[j].point[pn - 2], loop[j].point[pn - 1]);
      map_vertex[k + pn] = VEC2F_SUM(loop[j].point[pn - 1], USCALE_VEC2F(norm, wall_width));

      for(i = 1; i < pn - 1; i++)
       map_vertex[k + 2 * pn - 1 - i] = make_wall(loop[j].point[i - 1], loop[j].point[i], loop[j].point[i + 1], wall_width);
      k += pn * 2;

      for(i = 0; i < pn * 2 - 1; i++)
       {
        map_wall_segment[s].a = start + i;
        map_wall_segment[s].b = start + i + 1;
        s++;
       }

      map_wall_segment[s].a = start + i;
      map_wall_segment[s].b = start;
      s++; 
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n, start = k;

      for(i = 0; i < pn; i++)
       map_vertex[k + i] = loop[j].point[i];
      k += pn;

      for(i = 0; i < pn - 1; i++)
       {
        map_wall_segment[s].a = start + i;
        map_wall_segment[s].b = start + i + 1;
        s++;
       }

      map_wall_segment[s].a = start + i;
      map_wall_segment[s].b = start;
      s++;
     }

    if(loop[j].n == 2)
     {
      int start = k;
      VEC2F norm = USCALE_VEC2F(NORMALIZED_NORMAL_VEC2F(loop[j].point[0], loop[j].point[1]), wall_width);
      map_vertex[k] = loop[j].point[0];
      map_vertex[k + 1] = loop[j].point[1];
      //map_vertex[k + 1] = VEC2F_SUM(loop[j].point[0], norm);
      //map_vertex[k + 2] = VEC2F_SUM(loop[j].point[1], norm);
      //k += 4;
/*
      for(i = 0; i < 3; i++)
       {
        map_wall_segment[s].a = start + i;
        map_wall_segment[s].b = start + i + 1;
        s++;
       }

      map_wall_segment[s].a = start + i;
      map_wall_segment[s].b = start;
*/
      map_wall_segment[s].a = k;
      map_wall_segment[s].b = k + 1;
            k += 2;

      s++;
     }
   }

 //draw_map_sketch(buffer, final_shape, sn);

 VEC2F min, max, origin = ZERO_VEC2F;
 min = map_vertex[0];
 max = map_vertex[0];

 for(k = 1; k < vertex_count; k++)
  {
   if(map_vertex[k].x < min.x) { min.x = map_vertex[k].x; }
   if(map_vertex[k].y < min.y) { min.y = map_vertex[k].y; }
   if(map_vertex[k].x > max.x) { max.x = map_vertex[k].x; }
   if(map_vertex[k].y > max.y) { max.y = map_vertex[k].y; }
  }

 origin = USCALE_VEC2F(VEC2F_SUM(min, max), 0.5);

 //vec2f_line(buffer, vec2f(min.x, min.y), vec2f(max.x, min.y), 0);
 //vec2f_line(buffer, vec2f(max.x, min.y), vec2f(max.x, max.y), 0);
 //vec2f_line(buffer, vec2f(max.x, max.y), vec2f(min.x, max.y), 0);
 //vec2f_line(buffer, vec2f(min.x, max.y), vec2f(min.x, min.y), 0);

 for(k = 0; k < vertex_count; k++)
  vec2f_point(buffer, map_vertex[k], 2.0, 0);
 for(k = 0; k < segment_count; k++)
  vec2f_line(buffer, map_vertex[map_wall_segment[k].a], map_vertex[map_wall_segment[k].b], 0);
 for(k = 0; k < vertex_count; k++)
  map_vertex[k] = VEC2F_DIFF(map_vertex[k], origin);
 vec2f_point(buffer, origin, 5.0, 0);

 save_map("map.txt", map_vertex, map_wall_segment, vertex_count, segment_count);

 blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 readkey();

 free(map_vertex);
 free(map_wall_segment);

 for(i = 0; i < sn; i++)
  free(loop[i].point);
 destroy_bitmap(cursor_bmp);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
