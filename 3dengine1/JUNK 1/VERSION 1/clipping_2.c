#include <allegro.h>
#include "vec2f.h"
#include "vec2f_graph.h"

#define BSP_EPSILON 0.0001
#define BSP_BEHIND 0
#define BSP_INFRONT 1
#define BSP_SPANNED 2

float which_side(VEC2F a, VEC2F n, VEC2F p)
{
 return VEC2F_DOT_PRODUCT(VEC2F_DIFF(a, p), n);
}

int classify_polygon(VEC2F a, VEC2F normal, VEC2F poly[], int pnum,
                     VEC2F out_poly0[], int *out_pnum0, VEC2F out_poly1[], int *out_pnum1)
{
 int i, behind = 0, infront = 0, flag, count0 = 0, count1 = 0;
 float *d = (float *)malloc(pnum * sizeof(float)), k;

 for(i = 0; i < pnum; i++)
  {
   d[i] = which_side(a, normal, poly[i]);
   if(d[i] < -BSP_EPSILON) { behind++; }
   if(d[i] > BSP_EPSILON) { infront++; }
  }

 if(behind && !infront)
  return BSP_BEHIND;
 if(infront && ! behind)
  return BSP_INFRONT;
 if(!behind && !infront)
  return BSP_INFRONT;

 inline void clip_count(int v0, int v1)
  {
   flag = 0;
   if(d[v0] > BSP_EPSILON) { flag++; (*out_pnum0)++; }
   else                    { (*out_pnum1)++; }
   if(d[v1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     (*out_pnum0)++;
     (*out_pnum1)++;
    }
  }

 inline void clip_seg(int v0, int v1)
  {
   flag = 0;
   if(d[v0] > BSP_EPSILON) { out_poly0[count0] = poly[v0]; flag++; count0++; }
   else { out_poly1[count1] = poly[v0]; count1++; }
   if(d[v1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     k = fabs(d[v0]) / (fabs(d[v0]) + fabs(d[v1]));
     out_poly0[count0] = VEC2F_SUM(poly[v0], USCALE_VEC2F(VEC2F_DIFF(poly[v1], poly[v0]), k));
     out_poly1[count1] = out_poly0[count0];
     count0++;
     count1++;
    }
  }

 *out_pnum0 = 0;
 *out_pnum1 = 0;

 for(i = 0; i < pnum - 1; i++)
  clip_count(i, i + 1);
 clip_count(pnum - 1, 0);

 for(i = 0; i < pnum - 1; i++)
  clip_seg(i, i + 1);
 clip_seg(pnum - 1, 0);

 free(d);
 return BSP_SPANNED;
}

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

#define VNUM 10
#define SPEED 5

int main()
{
 int exit = 0, i;

 init();

 VEC2F a = vec2f(100.0, 100.0), b = vec2f(500.0, 400.0), n = NORMALIZED_NORMAL_VEC2F(a, b);
 VEC2F poly_local[VNUM], poly_world[VNUM], poly_pos = vec2f(320.0, 240.0);
 
 for(i = 0; i < VNUM; i++)
  {
   poly_local[i].x = 50.0 * cos(i * 2.0 * M_PI / VNUM);
   poly_local[i].y = 50.0 * sin(i * 2.0 * M_PI / VNUM);
  }

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_LEFT]) { poly_pos.x -= SPEED; }
   if(key[KEY_RIGHT]) { poly_pos.x += SPEED; }
   if(key[KEY_UP]) { poly_pos.y -= SPEED; }
   if(key[KEY_DOWN]) { poly_pos.y += SPEED; }

   for(i = 0; i < VNUM; i++)
    poly_world[i] = VEC2F_SUM(poly_local[i], poly_pos);

   clear_to_color(buffer, 0);

   VEC2F clipped1[100], clipped2[100];
   int cv1, cv2;

   int flag = classify_polygon(a, n, poly_world, VNUM, clipped1, &cv1, clipped2, &cv2);
   int col;
   if(flag == BSP_BEHIND)
    col = makecol(0, 255, 0);
   if(flag == BSP_INFRONT)
    col = makecol(0, 0, 255);
   if(flag == BSP_SPANNED)
    col = makecol(255, 0, 0);
   
   vec2f_line(buffer, a, b, makecol(255, 255, 255));

   if(flag == BSP_SPANNED)
    {
     if(key[KEY_SPACE])
      vec2f_loop(buffer, clipped1, cv1, 1, makecol(255, 0, 255));
     else
      vec2f_loop(buffer, clipped2, cv2, 1, makecol(255, 0, 255));
    }

   else
    vec2f_loop(buffer, poly_world, VNUM, 1, col);
   
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
