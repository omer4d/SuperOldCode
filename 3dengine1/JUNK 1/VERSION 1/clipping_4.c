#include <allegro.h>
#include "vec2f.h"
#include "vec2f_graph.h"

#define BSP_EPSILON 0.0001
#define BSP_BEHIND 0
#define BSP_INFRONT 1
#define BSP_SPANNED 2

typedef struct {
 int vnum, *vind;
}POLY2D;

float which_side(VEC2F a, VEC2F n, VEC2F p)
{
 return VEC2F_DOT_PRODUCT(VEC2F_DIFF(a, p), n);
}

int classify_polygon(VEC2F a, VEC2F normal, VEC2F **vertex, int *vnum, POLY2D *in, POLY2D *out0, POLY2D *out1)
{
 int i, behind = 0, infront = 0, flag;
 int count0 = 0, count1 = 0, new_vert_num = 0, new_vert_index = 0;
 float *d = (float *)malloc(in->vnum * sizeof(float)), k;

 for(i = 0; i < in->vnum; i++)
  {
   d[i] = which_side(a, normal, (*vertex)[in->vind[i]]);
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
   if(d[v0] > BSP_EPSILON) { flag++; out0->vnum++; }
   else                    { out1->vnum++; }
   if(d[v1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     out0->vnum++;
     out1->vnum++;
     new_vert_num++;
    }
  }

 inline void clip_seg(int v0, int v1)
  {
   flag = 0;
   if(d[v0] > BSP_EPSILON) { out0->vind[count0] = v0; flag++; count0++; }
   else { out1->vind[count1] = v0; count1++; }
   if(d[v1] > BSP_EPSILON) { flag++; }

   if(flag == 1)
    {
     k = fabs(d[v0]) / (fabs(d[v0]) + fabs(d[v1]));
     (*vertex)[(*vnum) + new_vert_index] = VEC2F_SUM((*vertex)[in->vind[v0]], USCALE_VEC2F(VEC2F_DIFF((*vertex)[in->vind[v1]], (*vertex)[in->vind[v0]]), k));
     out0->vind[count0] = (*vnum) + new_vert_index;
     out1->vind[count1] = (*vnum) + new_vert_index;
     count0++;
     count1++;
     new_vert_index++;
    }
  }

 out0->vnum = 0;
 out1->vnum = 0;

 for(i = 0; i < in->vnum - 1; i++)
  clip_count(i, i + 1);
 clip_count(in->vnum - 1, 0);

 out0->vind = (int *)malloc(out0->vnum * sizeof(int));
 out1->vind = (int *)malloc(out1->vnum * sizeof(int));
 *vertex = (VEC2F *)realloc((*vertex), sizeof(VEC2F) * ((*vnum) + new_vert_num));

 for(i = 0; i < in->vnum - 1; i++)
  clip_seg(i, i + 1);
 clip_seg(in->vnum - 1, 0);

 (*vnum) += new_vert_num; 
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

#define VNUM 5
#define SPEED 5

int main()
{
 int exit = 0, i;

 init();

 VEC2F *vertex;
 POLY2D *poly;

 vertex = (VEC2F *)malloc(VNUM * sizeof(VEC2F));
 poly = (POLY2D *)malloc(1 * sizeof(POLY2D));

 poly[0].vind = (int *)malloc(VNUM * sizeof(int));
 poly[0].vnum = VNUM;

  for(i = 0; i < VNUM; i++)
  {
   vertex[i].x = 320.0 + 50.0 * cos(i * 2.0 * M_PI / VNUM);
   vertex[i].y = 240.0 + 50.0 * sin(i * 2.0 * M_PI / VNUM);
   poly[0].vind[i] = i;
  }

 VEC2F a = vec2f(100.0, 100.0), b = vec2f(500.0, 300.0), n = NORMALIZED_NORMAL_VEC2F(a, b);
 
 POLY2D out0, out1;
 int vnum = VNUM;
 int flag = classify_polygon(a, n, &vertex, &vnum, &poly[0], &out0, &out1);
 printf("%d", vnum);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   //b = vec2f(mouse_x, mouse_y);
   //n = NORMALIZED_NORMAL_VEC2F(a, b);

   clear_to_color(buffer, 0);
   vec2f_line(buffer, a, b, makecol(255, 255, 255));

 if(flag == BSP_SPANNED)
 {
   if(key[KEY_SPACE])
   {
    for(i = 0; i < out0.vnum - 1; i++)
     vec2f_line(buffer, vertex[out0.vind[i]], vertex[out0.vind[i + 1]], makecol(255, 0, 0));
    vec2f_line(buffer, vertex[out0.vind[i]], vertex[out0.vind[0]], makecol(255, 0, 0));
   }

   else
    {
    for(i = 0; i < out1.vnum - 1; i++)
     vec2f_line(buffer, vertex[out1.vind[i]], vertex[out1.vind[i + 1]], makecol(255, 0, 0));
    vec2f_line(buffer, vertex[out1.vind[i]], vertex[out1.vind[0]], makecol(255, 0, 0));
   }
}

 else
  {
           for(i = 0; i < poly[0].vnum - 1; i++)
    vec2f_line(buffer, vertex[poly[0].vind[i]], vertex[poly[0].vind[i + 1]], makecol(255, 255, 255));
   vec2f_line(buffer, vertex[poly[0].vind[i]], vertex[poly[0].vind[0]], makecol(255, 255, 255));
  }

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 free(poly[0].vind);
 free(poly);
 free(vertex);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
