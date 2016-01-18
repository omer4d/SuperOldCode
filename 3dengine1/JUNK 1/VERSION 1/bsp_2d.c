#include <allegro.h>
#include "vec2f.h"
#include "bsp.h"
#include "map.h"

#define SCALE 5.0

BITMAP *buffer;
/*
int inverse_color(int c)
{
 return makecol(255 - getr(c), 255 - getg(c), 255 - getb(c));
}
*/
int val = 0;

void traverse_tree(BITMAP *bitmap, NODE *node, VEC2F p, VEC2F v, VEC2F vertex[])
{
 float k;

 if(node != NULL)
  {
   k = VEC2F_DOT_PRODUCT(node->n, VEC2F_DIFF(vertex[node->a], p));

   if(k < 0.0)
    {
     if(node->right != NULL)
      traverse_tree(bitmap, node->right, p, v, vertex);
     if(in_view(p, v, vertex[node->a], vertex[node->b]))
      {
       vec2f_line(bitmap, vertex[node->a], vertex[node->b], 0); printf("%d ", node->val);
       textprintf_centre_ex(bitmap, font, (vertex[node->a].x + vertex[node->b].x) * 0.5 + SCREEN_W * 0.5,
                                          (vertex[node->a].y + vertex[node->b].y) * 0.5 + SCREEN_H * 0.5,
                                           makecol(0, 0, 255), -1, "%d", node->val);
      }
     if(node->left != NULL)
      traverse_tree(bitmap, node->left, p, v, vertex);
    }

   else
    {
     if(node->left != NULL)
      traverse_tree(bitmap, node->left, p, v, vertex);
     if(in_view(p, v, vertex[node->a], vertex[node->b]))
      {
       vec2f_line(bitmap, vertex[node->a], vertex[node->b], 0); printf("%d ", node->val);
       textprintf_centre_ex(bitmap, font, (vertex[node->a].x + vertex[node->b].x) * 0.5 + SCREEN_W * 0.5,
                                          (vertex[node->a].y + vertex[node->b].y) * 0.5 + SCREEN_H * 0.5,
                                           makecol(0, 0, 255), -1, "%d", node->val);
      }
     if(node->right != NULL)
      traverse_tree(bitmap, node->right, p, v, vertex);
    }
  }
}

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
 VEC2F_GRAPH_tx = SCREEN_W * 0.5;
 VEC2F_GRAPH_ty = SCREEN_H * 0.5;
}

#define SPEED 1.0

void realloc_bullshit(VEC2F **bullshit, int n)
{
 *bullshit = (VEC2F*)realloc(*bullshit, n * sizeof(VEC2F));
 (*bullshit)[n - 1] = vec2f(20.0, 20.0);
}

int main()
{
 int exit = 0, a_flag = -1;//, val = 0;
 
 int vnum, snum, i;
 VEC2F *map_vertex;
 WALL_SEGMENT *map_segment;

 NODE *head = NULL;
 VEC2F p = vec2f(0.0, 0.0), v;
 float alpha = 0.0;

 load_map("map.txt", &map_vertex, &map_segment, &vnum, &snum, 1.0);
 
 //realloc_bullshit(&map_vertex, 20);
  //map_vertex[16] = vec2f(20.0, 20.0);

 init();

 head = (NODE *)malloc(sizeof(NODE));
 head->val = val;
 head->a = map_segment[0].a;
 head->b = map_segment[0].b;
 head->n = NORMALIZED_NORMAL_VEC2F(map_vertex[map_segment[0].a], map_vertex[map_segment[0].b]);
 head->parent = NULL;
 head->left = NULL;
 head->right = NULL;
 val++;
 
 for(i = 1; i < snum; i++)
  {
   add_node(head, &val, map_segment[i].a, map_segment[i].b, &map_vertex, &vnum);
   val++;
  }
 
 //compile_map(head, map, snum);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_SPACE])
    {
     printf("\n");
     key[KEY_SPACE] = 0;
    }
   if(key[KEY_LEFT]) { alpha -= 0.03; }
   if(key[KEY_RIGHT]) { alpha += 0.03; }
   if(key[KEY_W]) { p.y -= SPEED; }
   if(key[KEY_S]) { p.y += SPEED; }
   if(key[KEY_A]) { p.x -= SPEED; }
   if(key[KEY_D]) { p.x += SPEED; }

   v = vec2f(20.0 * cos(alpha), 20.0 * sin(alpha));

   clear_to_color(buffer, makecol(128, 128, 128));
   //draw_tree(buffer, p, v, head, 320, 240, 10, makecol(128, 0, 0));
        traverse_tree(buffer, head, p, v, map_vertex);
        printf("\n\n");

/*
   if(a_flag == 0)
    vec2f_line(buffer, a, b, makecol(0, 0, 128));
   if(a_flag == 1)
    ve2f_line(buffer, a, vector(mouse_x, mouse_y), makecol(0, 0, 128));
   circlefill(buffer, mouse_x, mouse_y, 2, 0);
   vec2f_point(buffer, p, 10, makecol(0, 128, 0));
*/

   vec2f_line(buffer, p, VEC2F_SUM(p, v), makecol(0, 0, 0));
   vec2f_line(buffer, p, VEC2F_SUM(p, USCALE_VEC2F(VEC2F_NORMAL(v), 200.0)), makecol(0, 0, 0));
   vec2f_line(buffer, p, VEC2F_SUM(p, USCALE_VEC2F(VEC2F_NORMAL(v), -200.0)), makecol(0, 0, 0));

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }
 
 free(map_vertex);
 free(map_segment);
 destroy_tree(head);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
