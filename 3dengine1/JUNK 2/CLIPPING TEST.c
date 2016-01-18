#include <allegro.h>
#include "vec2f.h"
#include "triangle.h"
#include "clipping.h"

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
 bind_color_buffer(buffer);
 texture = load_bitmap("data/texture.bmp", NULL);
}

int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

int render_edge(int curr, REND_INT_POINT *v0, REND_INT_POINT *v1)
{
 line(buffer, (*v0).affine_interp[0], (*v0).y, (*v1).affine_interp[0], (*v1).y, makecol(255, 0, 0));
}

int rt_count;
REND_INT_POINT fr0;

int render_tris(int curr, REND_INT_POINT *v0, REND_INT_POINT *v1)
{
 REND_INT_POINT tri[3];
 if(rt_count == 0)
   fr0 = *v0;
 else
  {
   if(fabs(v1->affine_interp[0]- fr0.affine_interp[0]) + fabs(v1->y - fr0.y) < FLOAT_EPSILON)
   return;

   tri[0] = fr0;
   tri[1] = *v0;
   tri[2] = *v1;
/*
       triangle(buffer, tri[0].affine_interp[0], tri[0].y,
                    tri[1].affine_interp[0], tri[1].y,
                   tri[2].affine_interp[0], tri[2].y, makecol(255, 255, 255));
*/

   rasterize_triangle(tri);
   
  }
  rt_count++;
}

#define VNUM 7
#define RADIUS 100.0
#define SPEED 5

void rotate_vec2f(VEC2F *output, VEC2F input, float ang)
{
 VEC2F temp;

 temp.x = input.x * cos(ang) - input.y * sin(ang);
 temp.y = input.x * sin(ang) + input.y * cos(ang);
 *output = temp;
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
 int exit = 0, i;
 REND_INT_POINT poly_local[VNUM], poly_world[VNUM];
 VEC2F pos = vec2f(320.0, 240.0);

 init();

 for(i = 0; i < VNUM; i++)
  {
   poly_local[i].affine_interp[0] = RADIUS * cos(i * 2.0 * M_PI / VNUM);
   poly_local[i].y = RADIUS * sin(i * 2.0 * M_PI / VNUM);

   poly_local[i].affine_interp[1] = 0.5 + 1.0 * cos(i * 2.0 * M_PI / VNUM);
   poly_local[i].affine_interp[2] = 0.5 + 1.0 * sin(i * 2.0 * M_PI / VNUM);
  }

 set_scr_clip_func(0, scr_clip, 0, 450.0, -1, 1);
 set_scr_clip_func(1, scr_clip, 0, 300.0, 1, 2);
 set_scr_clip_func(2, scr_clip, 1, 450.0, -1, 3);
 set_scr_clip_func(3, scr_clip, 1, 300.0, 1, 4);
 set_scr_clip_func(4, render_tris, 0.0, 0, 0, 0);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; }
   if(key[KEY_LEFT]) { pos.x -= SPEED; }
   if(key[KEY_RIGHT]) { pos.x += SPEED; }
   if(key[KEY_UP]) { pos.y -= SPEED; }
   if(key[KEY_DOWN]) { pos.y += SPEED; }

   if(key[KEY_SPACE])
    {
     VEC2F in, out;

     for(i = 0; i < VNUM; i++)
      {
       in = vec2f(poly_local[i].affine_interp[0], poly_local[i].y);
       rotate_vec2f(&out, in, 0.05);
       poly_local[i].affine_interp[0] = out.x;
       poly_local[i].y = out.y;
      }
    }

   for(i = 0; i < VNUM; i++)
    {    
     poly_world[i] = poly_local[i];
     poly_world[i].affine_interp[0] = poly_local[i].affine_interp[0] + pos.x;
     poly_world[i].y = poly_local[i].y + pos.y;
    }

   clear_to_color(buffer, 0);

/*
   for(i = 0; i < VNUM - 1; i++)
    line(buffer, poly_world[i].affine_interp[0], poly_world[i].y, poly_world[i+1].affine_interp[0], poly_world[i+1].y,
         makecol(255, 255, 255));
   line(buffer, poly_world[i].affine_interp[0], poly_world[i].y, poly_world[0].affine_interp[0], poly_world[0].y,
        makecol(255, 255, 255));
*/

  set_flat_color(makecol(50, 50, 50));
   rasterize_sides =flat_sides;
  
  for(i = 0; i < VNUM - 1; i++)
   {
    REND_INT_POINT tri[3];
    tri[0] = poly_world[0];
    tri[1] = poly_world[i];
    tri[2] = poly_world[i + 1];

   rasterize_triangle(tri);
   }

  set_flat_color(makecol(50,0, 0));
  rasterize_sides = my_sides;
   int tj = 0;

  for(tj = 0; tj < 1; tj++)
   {
    for(i = 0; i < MAX_SCR_CLIP_PLANES; i++)
    scr_clip_param[i].count = 0;

    rt_count = 0;
   for(i = 0; i < VNUM - 1; i++)
    scr_clip_func[0](0, &poly_world[i], &poly_world[i + 1]);
   scr_clip_func[0](0, &poly_world[i], &poly_world[0]);
}

      textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);

  blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
         frame_count++;
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
