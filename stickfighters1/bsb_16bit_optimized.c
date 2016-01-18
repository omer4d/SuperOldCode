#include <allegro.h>
#include "vector.h"
#include "particle.h"
#include "stickman.h"
#include "graphics.h"

float game_speed = 1.0;
int sm_flag;
long sm_time = 0;

#define RELAXATION_LOOP_NUM 10
#define CONTROL_FORCE 10000.0

BITMAP *buffer;
SAMPLE *blow_sound;
int ma_table[256][256];

void init();

void respond(PARTICLE *p1, PARTICLE *p2, VECTOR v1, VECTOR v2)
{
 float k = /*VECTOR_LENGTH(VECTOR_DIFF(v1, v2))*/10.0;

 VECTOR d = normalized_vector(VECTOR_DIFF(p1->pos, p2->pos));
 set_particle_velocity(p1, USCALE_VECTOR(d, k));
 set_particle_velocity(p2, USCALE_VECTOR(d, -k));
 
 if(sm_time < 1)
 play_sample(blow_sound, 50, 0, 700 + rand() % 800, 0);
 else
 play_sample(blow_sound, 50, 0, (700 + rand() % 800) / 2, 0);
 
 sm_time = 40;
}

void handle_collision(PARTICLE *p1, PARTICLE *p2, float r1, float r2)
{
 VECTOR d = VECTOR_DIFF(p1->pos, p2->pos), v1, v2;

 float ds = approx_vector_length(d), p, k = 1.0 - (p1->inverse_mass / (p1->inverse_mass + p2->inverse_mass));

 if(ds < r1 + r2)
  {
   p = r1 + r2 - ds;
   d = USCALE_VECTOR(d, 1.0 / ds);
   d = USCALE_VECTOR(d, p);

   v1 = particle_velocity(*p1);
   v2 = particle_velocity(*p2);

   position_particle(p1, VECTOR_SUM(p1->pos, USCALE_VECTOR(d, 1.0 - k)));
   position_particle(p2, VECTOR_DIFF(p2->pos, USCALE_VECTOR(d, k)));

   respond(p1, p2, v1, v2);
  }
}

void apply_convex_lens(BITMAP *source, BITMAP *lens, BITMAP *dest, int px, int py)
{
 int ix, iy, sx, sy, dx, dy; 
 int hw, hh;
 int c;

// float m;
 int m;
 
 hw = lens->w >> 1;
 hh = lens->h >> 1;
 
 for(iy = 0; iy < lens->h; iy++)
  for(ix = 0; ix < lens->w; ix++)
   {
    c = ((short*)(lens->line[iy]))[ix];

    if(c != 0)
     m = (255 - (getr16(c)+ getg16(c))) >> 3;
    else
     m = 1;

    dx = ix + px - hw;
    dy = iy + py - hh;
 
    sx = (int)(dx + m);
    sy = (int)(dy + m);

    ((short*)(dest->line[dy]))[dx] = ((short*)(source->line[sy]))[sx];
   }
}

void process_bitmap(BITMAP *last, BITMAP *current)
{
 int x, y, a;
 float damp = 0.98;

 for(y = 1; y < last->h - 1; y++)
  for(x = 1; x < last->w - 1; x++)
   {
    if(getr16(((short*)(last->line[y]))[x]) > 0)
     {
    a = ((getr16(((short*)(last->line[y]))[x - 1]) +
          getr16(((short*)(last->line[y]))[x + 1]) +
          getr16(((short*)(last->line[y + 1]))[x]) +
          getr16(((short*)(last->line[y - 1]))[x])) >> 2);

   ((short*)(current->line[y]))[x] = makecol16(a * damp, 0 , 0);
}

    else if(getg16(((short*)(last->line[y]))[x]) > 0)
     {
    a = ((getg16(((short*)(last->line[y]))[x - 1]) +
          getg16(((short*)(last->line[y]))[x + 1]) +
          getg16(((short*)(last->line[y + 1]))[x]) +
          getg16(((short*)(last->line[y - 1]))[x])) >> 2);

   ((short*)(current->line[y]))[x] = makecol16(0, a * damp , 0);
}

  }
}

void fire(BITMAP *target, BITMAP *bmp)
{
 int x, y, a, p2;
 int c, d;
 int r, g, b, p;

 for(y = 1; y < bmp->h - 1; y++)
  for(x = 1; x < bmp->w - 1; x++)
   {
    p = ((short*)(bmp->line[y]))[x];

    if((a = getr16(p)) > 0)
    {
     d = ((short*)(target->line[y]))[x];
     r = getr16(d);
     g = getg16(d);
     b = getb16(d);

    if(a < 128)
     c = makecol16(r, g, a + ma_table[b][255 - a]);
    else
     c = makecol16(ma_table[a][204] + ma_table[r][51], ma_table[a][204] + ma_table[g][51], 204 + ma_table[b][51]);

   ((short*)(target->line[y]))[x] = c; 
  }

 else if((a = getg16(p)) > 0)
  {
   d = ((short*)(target->line[y]))[x];
   r = getr16(d);
   g = getg16(d);
   b = getb16(d);

   if(a < 128)
    c = makecol16(a + ma_table[r][255 - a], g, b);
   else
    c = makecol16(204 + ma_table[b][51], ma_table[a][204] + ma_table[g][51], ma_table[a][204] + ma_table[r][51]);

   ((short*)(target->line[y]))[x] = c; 
  }

  }
  
}

int main()
{
 int exit_flag = 0, i, j;
 PARTICLE p[26], p2[26];
 
   BITMAP *background, *result;
 BITMAP *last, *current;
 FONT *game_font;
 
 VECTOR delta1;
 
 BITMAP *temp;
 

 init();
 GLOBAL_TIMESCALE = 0.01 * 0.8;
 GLOBAL_DAMPING = 0.999;
 create_default_pose();
   blow_sound = load_sample("blow.wav");

 game_font = load_bitmap_font("font.bmp", NULL, NULL);
 

 for(i = 0; i < 26; i++)
  {
   p[i] = create_particle(1.0, vector(100.0 + default_pose[i].x, 10.0 + default_pose[i].y));
   p2[i] = create_particle(1.0, vector(220.0 + default_pose[i].x, 10.0 + default_pose[i].y));
  }

 p[25].inverse_mass = 1.0 / 10.0;
 p[0].inverse_mass = 1.0 / 2.0;
 p[12].inverse_mass = 1.0 / 2.0;
 p[23].inverse_mass = 1.0 / 2.0;
 p[19].inverse_mass = 1.0 / 2.0;

 p2[25].inverse_mass = 1.0 / 10.0;
 p2[0].inverse_mass = 1.0 / 2.0;
 p2[12].inverse_mass = 1.0 / 2.0;
 p2[23].inverse_mass = 1.0 / 2.0;
 p2[19].inverse_mass = 1.0 / 2.0;
 
   background = load_bitmap("background.bmp", NULL);
 result = create_bitmap(background->w, background->h);
 last = create_bitmap(screen->w, screen->h);
 current = create_bitmap(screen->w, screen->h);

 clear_to_color(last, 0);
 clear_to_color(current, 0);

 while(!exit_flag)
  {               
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit_flag = 1; }
     if(key[KEY_RIGHT]) { p[25].force.x += CONTROL_FORCE; }
     if(key[KEY_LEFT]) { p[25].force.x -= CONTROL_FORCE; }
     if(key[KEY_UP]) { p[25].force.y -= CONTROL_FORCE; }
     if(key[KEY_DOWN]) { p[25].force.y += CONTROL_FORCE; }
     //if(key[KEY_SPACE]) { rest(30); }

     if(key[KEY_PGUP]) { gfx_scale += 0.1; key[KEY_PGUP] = 0; }
     if(key[KEY_PGDN]) { gfx_scale -= 0.1; key[KEY_PGDN] = 0; }
    }

    if(sm_time > 0)
     {
      rest(30);
      sm_time--;
      sm_flag = 1;
     }

    else
     sm_flag = 0;

    sm_flag = key[KEY_SPACE];

    VECTOR spring = USCALE_VECTOR(VECTOR_DIFF(p[25].pos, p2[25].pos), 50.0);
    p2[25].force = VECTOR_SUM(p2[25].force, spring);

   for(i = 0; i < 26; i++)
    {
     p[i].force.y += 100.0;
     p2[i].force.y += 100.0;
     simulate_particle(&p[i]);
     simulate_particle(&p2[i]);
    }

   for(i = 0; i < RELAXATION_LOOP_NUM; i++)
    {
     form_stickman(p);
     form_stickman(p2);
    }

   for(j = 0; j < 25; j++)
    {
     border_collision(&p[j], PARTICLE_RADIUS, 1.0);
     border_collision(&p2[j], PARTICLE_RADIUS, 1.0);
    }

   border_collision(&p[25], HEAD_RADIUS, 1.0);
   border_collision(&p2[25], HEAD_RADIUS, 1.0);

   int a, b;
   float r1, r2;

   for(a = 0; a < 26; a++)
    for(b = 0; b < 26; b++)
     {
      if(a == 25) { r1 = HEAD_RADIUS; }
      else { r1 = PARTICLE_RADIUS; }
      if(b == 25) { r2 = HEAD_RADIUS; }
      else { r2 = PARTICLE_RADIUS; }

      handle_collision(&p[a], &p2[b], r1, r2);
     }
     
        //clear_bitmap(buffer);
   
   process_bitmap(last, current);

   blit(current, buffer, 0, 0, 0, 0, screen->w, screen->h);
   blit(current, last, 0, 0, 0, 0, screen->w, screen->h);

   //circlefill(last, mouse_x, mouse_y, 5, makecol(255, 0, 0));
   draw_stickman(last, p, makecol16(255, 0, 0), makecol16(255, 0, 0));
   draw_stickman(last, p2, makecol16(0, 255, 0), makecol16(0, 255, 0));

   //clear_to_color(background, makecol16(255, 255, 255));

   blit(background, result, 0, 0, 0, 0, result->w, result->h);
   apply_convex_lens(background, current, result, screen->w >> 1, screen->h >> 1);
   //draw_stickman2(result, p, makecol16(200, 200, 200), 0);
   //draw_stickman2(result, p2, makecol16(200, 200, 200), 0);
   //blit(current, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   fire(result, current);
   
   //draw_stickman(result, p, makecol16(200, 200, 255), 0);
   //draw_stickman(result, p2, makecol16(255, 200, 200), 0);
   
   draw_stickman(result, p, makecol16(200, 200, 255), 0);
   draw_stickman(result, p2, makecol16(255, 200, 200), 0);
   
   //textprintf_ex(result, game_font, 0, 0, makecol16(0, 0, 128), 0, "THIS IS SOME TEXT!");
   
   
   blit(result, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

/*
   clear_bitmap(buffer);
   draw_stickman(buffer, p, makecol16(64, 64, 64), makecol16(64, 64, 64));
   draw_stickman(buffer, p2, makecol16(200, 200, 200), makecol16(200, 200, 200));
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
*/
  }

 destroy_font(game_font);
 destroy_sample(blow_sound);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()

void init()
{
 allegro_init();

 set_color_depth(16);
 if(set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

 install_keyboard();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 srand(time(NULL));
 
 int m, a;

 for(m = 0; m < 256; m++)
  for(a = 0; a < 256; a++)
   ma_table[m][a] = (int)(m * (a / 255.0));
}
