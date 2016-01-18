#include <allegro.h>
#include "vector.h"
#include "bsp.h"
#include "map.h"

#define SCALE 5.0

BITMAP *buffer;

int inverse_color(int c)
{
 return makecol(255 - getr(c), 255 - getg(c), 255 - getb(c));
}

int val = 0;

void add_wall(NODE *node, VECTOR a, VECTOR b)
{
 add_node(node, &val, USCALE_VECTOR(a,SCALE), USCALE_VECTOR(b,SCALE));
 val++;
}

void add_wall_loop(NODE *node, VECTOR p[], int n, int flag)
{
 int i;

   for(i = 0; i < n - 1; i++)
    add_wall(node, p[i], p[i + 1]);
   if(flag)
    add_wall(node, p[i], p[0]);
}

void compile_map(NODE *node, LINE_LOOP *loop, int t)
{
 int i, j;
     
  for(j = 0; j < t; j++)
   {
    if(loop[j].n > 4 && !loop[j].closed)
     {
      int pn = loop[j].n / 2;
      add_wall_loop(node, loop[j].point, pn, loop[j].closed);
 
      for(i = 0; i < pn - 1; i++)
        add_wall(node, loop[j].point[i + pn], loop[j].point[i + pn + 1]);

      add_wall(node, loop[j].point[0], loop[j].point[pn]);
      add_wall(node, loop[j].point[pn - 1], loop[j].point[pn + pn - 1]);
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n;
      add_wall_loop(node, loop[j].point, loop[j].n, loop[j].closed);
     }

    if(loop[j].n == 4 && !loop[j].closed)
     {
      add_wall(node, loop[j].point[0], loop[j].point[1]);
      add_wall(node, loop[j].point[2], loop[j].point[3]);
      add_wall(node, loop[j].point[0], loop[j].point[2]);
      add_wall(node, loop[j].point[1], loop[j].point[3]);
     }
   }
}

void draw_tree(BITMAP *bitmap, VECTOR p, VECTOR v, NODE *node, int x, int y, int r, int c)
{
 int inv_c = inverse_color(c);

 if(node != NULL)
  {
   if(node->right != NULL)
    {
     //line(bitmap, x, y, x + 2 * r, y + 2 * r, inv_c);
     draw_tree(bitmap, p, v, node->right, x + 2 * r, y + 2 * r, r, c);
    }

   if(node->left != NULL)
    {
     //line(bitmap, x, y, x - 2 * r, y + 2 * r, inv_c);
     draw_tree(bitmap, p, v, node->left, x - 2 * r, y + 2 * r, r, c);
    }

  if(in_view(p, v, node->a, node->b))
   vector_line(bitmap, node->a, node->b, makecol(255, 255, 255));
  else
   vector_line(bitmap, node->a, node->b, 0);

   textprintf_centre_ex(bitmap, font, node->a.x - 10, node->a.y, makecol(0, 0, 255), -1, "%d", node->val);

   //circlefill(bitmap, x, y, r, c);
   //circle(bitmap, x, y, r, inv_c);
   //textprintf_centre_ex(bitmap, font, x, y - 4, inv_c, -1, "%d", node->val);
  }
}



int first = 1;

void traverse_tree(BITMAP *bitmap, NODE *node, VECTOR p, VECTOR v)
{
 float k;

 if(node != NULL)
  {
   k = VECTOR_DOT_PRODUCT(node->n, VECTOR_DIFF(node->a, p));

   if(k > 0.0)
    {
     if(node->right != NULL)
      traverse_tree(bitmap, node->right, p, v);
     if(in_view(p, v, node->a, node->b))
      {
        vector_line(bitmap, node->a, node->b, 0);
        textprintf_centre_ex(bitmap, font, (node->a.x + node->b.x) * 0.5, (node->a.y+node->b.y)*0.5, makecol(0, 0, 255), -1, "%d", first);
        first++;
      }
     if(node->left != NULL)
      traverse_tree(bitmap, node->left, p, v);
    }

   else
    {
     if(node->left != NULL)
      traverse_tree(bitmap, node->left, p, v);
     if(in_view(p, v, node->a, node->b))
      {
        vector_line(bitmap, node->a, node->b, 0);
       textprintf_centre_ex(bitmap, font, (node->a.x + node->b.x) * 0.5, (node->a.y+node->b.y)*0.5, makecol(0, 0, 255), -1, "%d", first);
       first++;
      }
     if(node->right != NULL)
      traverse_tree(bitmap, node->right, p, v);
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
}

#define SPEED 1.0

int main()
{
 int exit = 0, a_flag = -1;//, val = 0;
 
 
 NODE *head = NULL;
 VECTOR a, b;
 VECTOR p = vector(320.0, 240.0), v;
 float alpha = 0.0;
 int snum;
 LINE_LOOP *map = load_map("map.txt", &snum, 0.2);

 init();
 
       a = vector(0.0, 0.0);
       b = vector(1.0, 0.0);
       head = (NODE *)malloc(sizeof(NODE));
       head->val = val;
       head->a = a;
       head->b = b;
       head->n = NORMALIZED_NORMAL(a, b);
       head->parent = NULL;
       head->left = NULL;
       head->right = NULL;
       val++;
       
 compile_map(head, map, snum);

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

   v = vector(20.0 * cos(alpha), 20.0 * sin(alpha));

   if(mouse_b == 1)
    {
     a.x = mouse_x;
     a.y = mouse_y;
     a_flag = 1;
    }

   if(mouse_b == 2 && a_flag > 0)
    {
     b.x = mouse_x;
     b.y = mouse_y;
     a_flag = 0;

      add_node(head, &val, a, b);
     val++;
    }

   clear_to_color(buffer, makecol(128, 128, 128));
   //draw_tree(buffer, p, v, head, 320, 240, 10, makecol(128, 0, 0));
   first = 1;
        traverse_tree(buffer, head, p, v);
        printf("\n\n");
   
   if(a_flag == 0)
    vector_line(buffer, a, b, makecol(0, 0, 128));
   if(a_flag == 1)
    vector_line(buffer, a, vector(mouse_x, mouse_y), makecol(0, 0, 128));
   circlefill(buffer, mouse_x, mouse_y, 2, 0);
   vector_point(buffer, p, 10, makecol(0, 128, 0));

   vector_line(buffer, p, VECTOR_SUM(p, v), makecol(0, 0, 0));
   vector_line(buffer, p, VECTOR_SUM(p, USCALE_VECTOR(VECTOR_NORMAL(v), 200.0)), makecol(0, 0, 0));
   vector_line(buffer, p, VECTOR_SUM(p, USCALE_VECTOR(VECTOR_NORMAL(v), -200.0)), makecol(0, 0, 0));

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }
 
 free_map(map, snum);
 destroy_tree(head);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
