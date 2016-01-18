#include <allegro.h>
#include "particle.h"
#include "body.h"

#define MOVE_ZIG (30000.0)

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
 srand(time(NULL));
}

int main()
{
 int exit = 0;

 init();

 BITMAP *canvas = create_bitmap(SCREEN_W, SCREEN_H);
 BITMAP *cursor = load_bitmap("cursor.bmp", NULL);

 GLOBAL_TIMESCALE = 0.01;

 int i;

 VECTOR mvt, shape_vertex[200];
 int vnum = 0, timeout = -1;

 BODY body[100];
 int bnum = 0;

 while(!exit)
  {
   mvt = ZERO_VECTOR;

   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
     if(key[KEY_LEFT]) { mvt.x -= MOVE_ZIG; }
     if(key[KEY_RIGHT]) { mvt.x += MOVE_ZIG; }
     if(key[KEY_UP]) { mvt.y -= MOVE_ZIG; }
     if(key[KEY_DOWN]) { mvt.y += MOVE_ZIG; }
    }
    
   int cursor_x = (mouse_x / 5) * 5, cursor_y = (mouse_y / 5) * 5;

   if(mouse_b == 1 && timeout < 0)
    {
     shape_vertex[vnum] = vector(cursor_x, cursor_y);
     vnum++;
     timeout = 20;
    }
   timeout--;

   if(key[KEY_ENTER] && vnum > 0)
    {
     clear_to_color(canvas, EMPTY_COL);
     vector_poly(canvas, shape_vertex, vnum, SHAPE_COL);
     for(i = 0; i < vnum; i++)
      putpixel(canvas, shape_vertex[i].x, shape_vertex[i].y, VERTEX_COL);
     vnum = 0;
     int col = makecol(20 + rand() % 230,
                       20 + rand() % 230,
                       20 + rand() % 230);
     body[bnum] = make_body(canvas, 20, col, vector(rand() % SCREEN_W, rand() % SCREEN_H));
     bnum++;
    }

   if(bnum > 0)
   for(i = 0; i < body[0].pnum; i++)
    body[0].par[i].force = VECTOR_SUM(body[0].par[i].force, mvt);
 
  for(i = 0; i < bnum; i++)
  {
   simulate_body(&body[i]);
   body_border_collision(&body[i], 1.0);
  }


 int j;

   for(i = 0; i < 10; i++)
    for(j = 0; j < bnum; j++)
     constrain_body(&body[j]);

  if(bnum > 1)
  {
   for(i = 0; i < bnum; i++)
    for(j = i + 1; j < bnum; j++)
    handle_body_collision(&body[i], &body[j]);
  }

   clear_to_color(buffer, makecol(64, 64, 64));
   
   for(i = 0; i < bnum; i++)
   draw_body(buffer, &body[i]);

   //circlefill(buffer, mouse_x, mouse_y, 2, makecol(0, 255, 0));
   draw_sprite(buffer, cursor, cursor_x - 4, cursor_y - 5);

   if(vnum > 0)
   {
   for(i = 0; i < vnum - 1; i++)
    line(buffer, shape_vertex[i].x, shape_vertex[i].y, shape_vertex[i + 1].x, shape_vertex[i + 1].y,
         makecol(0, 255, 255));
    line(buffer, shape_vertex[i].x, shape_vertex[i].y, shape_vertex[0].x, shape_vertex[0].y,
         makecol(50, 0, 0));
    line(buffer, shape_vertex[i].x, shape_vertex[i].y, cursor_x, cursor_y,
         makecol(0, 50, 50));
    line(buffer, cursor_x, cursor_y, shape_vertex[0].x, shape_vertex[0].y,
         makecol(0, 50, 50));

   for(i = 0; i < vnum; i++)
    circlefill(buffer, shape_vertex[i].x, shape_vertex[i].y, 2, makecol(0, 255, 255));
   }

   if(key[KEY_SPACE])
    draw_sprite(buffer, canvas, 0, 0);

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 for(i = 0; i < bnum; i++)
 destroy_body(&body[i]);

 destroy_bitmap(cursor);
 destroy_bitmap(canvas);
 destroy_bitmap(buffer);

 return 0;
}
END_OF_MAIN()
