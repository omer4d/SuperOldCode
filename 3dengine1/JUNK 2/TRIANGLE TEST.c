#include <allegro.h>
#include "vec2f.h"
#include "triangle.h"

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

BITMAP *buffer, *texture;

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
 texture = load_bitmap("data/texture.bmp", NULL);
 BASE_INT_color_buffer = buffer;
}

void my_hline(BITMAP *buffer, int y, float x2, float interp1[], float didx[])
{    
 int x, minx, maxx, i;
 float curr[2];

 minx = (int)ceil(interp1[0]);
 maxx = (int)ceil(x2) - 1;

 if(minx > maxx)
  return;

 curr[0] = interp1[1] + (minx - interp1[0]) * didx[0];
 curr[1] = interp1[2] + (minx - interp1[0]) * didx[1];

 for(x = minx; x < maxx + 1; x++)
  {
   int iu, iv, col;

   FAST_ROUND(curr[0] * (texture->w - 1), iu);
   FAST_ROUND(curr[1] * (texture->h - 1), iv);

   col = ((long *)texture->line[BIT_WRAP(iu, texture->h - 1)])[BIT_WRAP(iv, texture->w - 1)];
   ((long *)buffer->line[y])[x] = col;

   curr[0] += didx[0];
   curr[1] += didx[1];
  }
}

void my_sides(int miny, int maxy, float didy1[], float accum1[], float dxdy2, float *accum2, float didx[])
{
 int minx, maxx, y, i;

 for(y = miny; y < maxy + 1; y++)
  {
   my_hline(buffer, y, *accum2, accum1, didx);

   accum1[0] += didy1[0];
   accum1[1] += didy1[1];
   accum1[2] += didy1[2];
   (*accum2) += dxdy2;
  }
}

int main()
{
 int exit = 0;

 init();

 /*
 
A: 420.000000 375.000000
B: 300.000000 300.000000
C: 300.000000 450.000000
*/


 VEC2F a = vec2f(420.0, 375.0), b = vec2f(300.0, 300.0), c = vec2f(200.0, 450.0);
 REND_INT_POINT tri[3];

 tri[0].y = a.y;
 tri[1].y = b.y;
 tri[2].y = c.y;

 tri[0].affine_interp[0] = a.x;
 tri[0].affine_interp[1] = 0.0;
 tri[0].affine_interp[2] = 0.0;
 
 tri[1].affine_interp[0] = b.x;
 tri[1].affine_interp[1] = 0.0;
 tri[1].affine_interp[2] = 10.0;

 tri[2].affine_interp[0] = c.x;
 tri[2].affine_interp[1] = 10.0;
 tri[2].affine_interp[2] = 10.0;

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);
 rasterize_sides = my_sides;
 set_flat_color(makecol(200, 0, 0));

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   
   if(key[KEY_1]) { a.x = mouse_x; a.y = mouse_y; }
   if(key[KEY_2]) { b.x = mouse_x; b.y = mouse_y; }
   if(key[KEY_3]) { c.x = mouse_x; c.y = mouse_y; }

    tri[0].y = a.y;
 tri[1].y = b.y;
 tri[2].y = c.y;

 tri[0].affine_interp[0] = a.x;
 tri[0].affine_interp[1] = 0.0;
 tri[0].affine_interp[2] = 0.0;
 
 tri[1].affine_interp[0] = b.x;
 tri[1].affine_interp[1] = 0.0;
 tri[1].affine_interp[2] = 2.0;

 tri[2].affine_interp[0] = c.x;
 tri[2].affine_interp[1] = 2.0;
 tri[2].affine_interp[2] = 2.0;

   clear_to_color(buffer, 0);

  int i;
/*
  for(i = 0; i < 100; i++)
   {
    a.x = 5 + rand() % (SCREEN_W - 15);
    b.x = 5 + rand() % (SCREEN_W - 15);
    c.x = 5 + rand() % (SCREEN_W - 15);

    a.y = 5 + rand() % (SCREEN_H - 15);
    b.y = 5 + rand() % (SCREEN_H - 15);
    c.y = 5 + rand() % (SCREEN_H - 15);

     tri[0].y = a.y;
 tri[1].y = b.y;
 tri[2].y = c.y;


 tri[0].affine_interp[0] = a.x;
 tri[0].affine_interp[1] = 0.0;
 tri[0].affine_interp[2] = 0.0;
 
 tri[1].affine_interp[0] = b.x;
 tri[1].affine_interp[1] = 0.0;
 tri[1].affine_interp[2] = 1.0;

 tri[2].affine_interp[0] = c.x;
 tri[2].affine_interp[1] = 1.0;
 tri[2].affine_interp[2] = 1.0;
 
     triangle(buffer, tri[0].affine_interp[0], tri[0].y, 
                      tri[1].affine_interp[0], tri[1].y,
                       tri[1].affine_interp[0], tri[2].y, makecol(255, 255, 255));

   
    rasterize_triangle(tri);
   }
 */
  
   //circlefill(buffer, tri[0].affine_interp[0], tri[0].y, 2, makecol(255,0 ,0));
   //circlefill(buffer, tri[1].affine_interp[0], tri[1].y, 2, makecol(255,0 ,0));
   //circlefill(buffer, tri[2].affine_interp[0], tri[2].y, 2, makecol(255,0 ,0));

   rasterize_triangle(tri);

   //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

/*
   for(i = 0; i < 25; i++)
    triangle(buffer, tri[0].affine_interp[0], tri[0].y, 
                      tri[1].affine_interp[0], tri[1].y,
                       tri[1].affine_interp[0], tri[2].y, makecol(255, 255, 255));
*/
   //printf("%d\n", counter);

   circlefill(buffer, mouse_x, mouse_y, 2, makecol(0, 255, 0));
   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps * 100);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
      frame_count++;
  }

 destroy_bitmap(texture);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
