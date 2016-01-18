#include <allegro.h>
#include "vector.h"
#include "graphics.h"
#include "stickman.h"

#define CONTROL_FORCE 94.0
#define P1_COLOR makecol16(200, 20, 20)
#define P2_COLOR makecol16(100, 10, 10)
#define BLOW_POWER 5.0
BITMAP *buffer;

int sm_time = 0;

void init();

void respond_circle_collision(PARTICLE *p1, PARTICLE *p2, VECTOR v1, VECTOR v2)
{
 float k = BLOW_POWER;
 VECTOR d = normalized_vector(VECTOR_DIFF(p1->pos, p2->pos));

 p1->vel = USCALE_VECTOR(d, k);
 p2->vel = USCALE_VECTOR(d, -k);
}

void handle_circle_collision(PARTICLE *p1, PARTICLE *p2, float r1, float r2)
{
 VECTOR d = VECTOR_DIFF(p1->pos, p2->pos), v1, v2;
 float ds = approx_vector_length(d), p,
       k = 1.0 - (p1->inverse_mass / (p1->inverse_mass + p2->inverse_mass));

 if(ds < r1 + r2)
  {
   p = r1 + r2 - ds;
   d = USCALE_VECTOR(d, 1.0 / ds);
   d = USCALE_VECTOR(d, p);
   p1->pos = VECTOR_SUM(p1->pos, USCALE_VECTOR(d, 1.0 - k));
   p2->pos = VECTOR_DIFF(p2->pos, USCALE_VECTOR(d, k));
   respond_circle_collision(p1, p2, p1->vel, p2->vel);
   if(sm_time < 1)
    sm_time = 200;
  }
}

void handle_stickman_collision(PARTICLE p1[26], PARTICLE p2[26])
{
 int a, b;
 float r1, r2;

 for(a = 0; a < 26; a++)
  for(b = 0; b < 26; b++)
   {
    if(a == 25) { r1 = HEAD_RADIUS; }
    else { r1 = PARTICLE_RADIUS; }
    if(b == 25) { r2 = HEAD_RADIUS; }
    else { r2 = PARTICLE_RADIUS; }
    handle_circle_collision(&p1[a], &p2[b], r1, r2);
   }
}

void focus_on_stickmen(PARTICLE p1[26], PARTICLE p2[26])
{
 GFX_TX = -(p1[25].pos.x + p2[25].pos.x) * 0.5;
 GFX_TY = -(p1[25].pos.y + p2[25].pos.y) * 0.5;
 GFX_TZ = 80.0 + VECTOR_LENGTH(VECTOR_DIFF(p1[25].pos, p2[25].pos)) * 130.0;
}

float h = 0.0;

void draw_grid()
{
 float x, y, t = GFX_TZ, z;
 int r, g, b, c;
 hsv_to_rgb((int)h, 1.0, 1.0, &r, &g, &b);

 h += 0.5 * TIMESCALE;
 if(h > 359.0) h = 0.0;

 float fr = r / 5.0, fg = g / 5.0, fb = 5.0;
 float r2, g2, b2;
 r2 = 0, g2 = 0, b2 = 0;

 for(z = 0; z < 5; z++)
  {
   //fr *= 1.3;
   //fg *= 1.3;
   //fb *= 1.3;
   r2 += fr; g2 += fg; b2 += fb;
   
   c = makecol16(r2, g2, b2);
   GFX_TZ = t + 20.0 * (1.0 - z);
   for(x = BORDER_MINX - 1.0; x < BORDER_MAXX + 1.0; x += 0.25)
    vector_line(buffer, vector(x, BORDER_MINY), vector(x, BORDER_MAXY), c);
   for(y = BORDER_MINY - 1.0; y < BORDER_MAXY + 1.0; y += 0.25)
    vector_line(buffer, vector(BORDER_MINX, y), vector(BORDER_MAXX, y), c);
 }

 GFX_TZ = t;
 vector_fillrect(buffer, vector(BORDER_MINX - 1.0, BORDER_MAXY),
 vector(BORDER_MAXX + 1.0, BORDER_MAXY + 1.0)
 ,makecol16(100, 100, 100));
 
  vector_fillrect(buffer, vector(BORDER_MINX - 1.0, BORDER_MINY),
 vector(BORDER_MAXX + 1.0, BORDER_MINY - 1.0)
 ,makecol16(100, 100, 100));
 
   vector_fillrect(buffer, vector(BORDER_MINX, BORDER_MINY - 1.0),
 vector(BORDER_MINX - 1.0, BORDER_MAXY + 1.0)
 ,makecol16(100, 100, 100));
 
   vector_fillrect(buffer, vector(BORDER_MAXX, BORDER_MINY - 1.0),
 vector(BORDER_MAXX + 1.0, BORDER_MAXY + 1.0)
 ,makecol16(100, 100, 100));
}

void sprite(BITMAP *spr, VECTOR a, VECTOR b, float at)
{
 VECTOR delta, p;
 float al;
 delta = VECTOR_DIFF(a, b);
 delta.x *= at;
 delta.y *= at;
 p.x = a.x - delta.x;
 p.y = a.y - delta.y;
 p = transform(p);
 al = atan2(a.y - b.y, a.x - b.x) / (M_PI / 128.0) - 64.0;
 pivot_scaled_sprite_v_flip(buffer, spr, (int)p.x, (int)p.y, 18, 0, ftofix(al), 
 ftofix(256.0 * 0.6 / GFX_TZ));
}

int main()
{
 int exit = 0, i, j;
 PARTICLE p1[26], p2[26];
 BITMAP *head, *head2;

 init();
 head = load_bitmap("santa_fury.bmp", NULL);
 head2 = load_bitmap("santa_head.bmp", NULL);

 create_stickman(p1, vector(-0.5, 0.0));
 create_stickman(p2, vector(0.5, 0.0));

 while(!exit)
  {
   TIMESCALE = 1.0;

   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
     control_particle(&p1[25], CONTROL_FORCE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
     control_particle(&p2[25], CONTROL_FORCE, KEY_W, KEY_S, KEY_A, KEY_D);
     if(key[KEY_SPACE]) { TIMESCALE = 0.1; }
    }
    
       if(sm_time)
     {
      if(sm_time < 180)
       TIMESCALE = 0.05;
      
      sm_time--;
     }
    

   VECTOR spring = USCALE_VECTOR(VECTOR_DIFF(p1[25].pos, p2[25].pos), 30.0);
   p2[25].force = VECTOR_SUM(p2[25].force, spring);
   handle_stickman(p1);
   handle_stickman(p2);
   handle_stickman_collision(p1, p2);

   clear_to_color(buffer, 0);

   focus_on_stickmen(p1, p2);
   draw_grid();
   draw_stickman(buffer, p1, P1_COLOR);
   draw_stickman(buffer, p2, P2_COLOR);
   sprite(head, p1[25].pos, p1[24].pos, 3.0);
   
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }
  

 destroy_bitmap(head2);
 destroy_bitmap(head);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()

void init()
{
 allegro_init();
 set_color_depth(16);

 if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 //install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 create_default_pose();
}
