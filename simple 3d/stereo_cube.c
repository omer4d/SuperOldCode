#include <stdio.h>
#include <math.h>
#include <allegro.h>

#define RED makecol(255, 0, 0)
#define BLUE makecol(0, 0, 255)
#define WHITE makecol(255, 255, 255)

typedef struct {
 float x, y;
}_2D;

typedef struct {
 float x, y, z;
}_3D;

typedef struct {
 _3D local;
 _2D screen[2];
}VERTEX;

float sin_table[512], cos_table[512];

BITMAP *buffer;

#define SIN(x) sin_table[abs((int)x&511)]
#define COS(x) cos_table[abs((int)x&511)]

#define DEG_TO_PI M_PI / 256.0

#define FOCAL_DISTANCE 1024.0

void init_trig_tables();
void init();
void rotate_vertex(VERTEX *v, int xang, int yang, int zang);
void project_vertex(VERTEX *v, _3D world_pos);
void _2d_line(_2D a, _2D b, int color);

int main()
{
 int exit_flag = 0, i;

 VERTEX cube[8];
 cube[0].local = (_3D){-10.0, -10.0, -10.0};
 cube[1].local = (_3D){10.0, -10.0, -10.0};
 cube[2].local = (_3D){10.0, 10.0, -10.0};
 cube[3].local = (_3D){-10.0, 10.0, -10.0};
 cube[4].local = (_3D){-10.0, -10.0, 10.0};
 cube[5].local = (_3D){10.0, -10.0, 10.0};
 cube[6].local = (_3D){10.0, 10.0, 10.0};
 cube[7].local = (_3D){-10.0, 10.0, 10.0};

 _3D world_pos = {0.0, 0.0, 100.0};

 init();

 while(!exit_flag)
  {
   rest(10);

   if(keypressed())
    {
     if(key[KEY_ESC]) { exit_flag = 1; }
     if(key[KEY_LEFT]) { world_pos.x -= 1.0; }
     if(key[KEY_RIGHT]) { world_pos.x += 1.0; }
     if(key[KEY_UP]) { world_pos.z += 3.0; }
     if(key[KEY_DOWN]) { world_pos.z -= 3.0; }
    }

   clear_to_color(buffer, 0);

   for(i = 0; i < 8; i++)
    {
     rotate_vertex(&cube[i], 1, 2, 3);
     project_vertex(&cube[i], world_pos);
    }

   _2d_line(cube[0].screen[0], cube[1].screen[0], RED);
   _2d_line(cube[1].screen[0], cube[2].screen[0], RED);
   _2d_line(cube[2].screen[0], cube[3].screen[0], RED);
   _2d_line(cube[3].screen[0], cube[0].screen[0], RED);

   _2d_line(cube[4].screen[0], cube[5].screen[0], RED);
   _2d_line(cube[5].screen[0], cube[6].screen[0], RED);
   _2d_line(cube[6].screen[0], cube[7].screen[0], RED);
   _2d_line(cube[7].screen[0], cube[4].screen[0], RED);

   _2d_line(cube[0].screen[0], cube[4].screen[0], RED);
   _2d_line(cube[1].screen[0], cube[5].screen[0], RED);
   _2d_line(cube[2].screen[0], cube[6].screen[0], RED);
   _2d_line(cube[3].screen[0], cube[7].screen[0], RED);

   _2d_line(cube[0].screen[1], cube[1].screen[1], BLUE);
   _2d_line(cube[1].screen[1], cube[2].screen[1], BLUE);
   _2d_line(cube[2].screen[1], cube[3].screen[1], BLUE);
   _2d_line(cube[3].screen[1], cube[0].screen[1], BLUE);

   _2d_line(cube[4].screen[1], cube[5].screen[1], BLUE);
   _2d_line(cube[5].screen[1], cube[6].screen[1], BLUE);
   _2d_line(cube[6].screen[1], cube[7].screen[1], BLUE);
   _2d_line(cube[7].screen[1], cube[4].screen[1], BLUE);

   _2d_line(cube[0].screen[1], cube[4].screen[1], BLUE);
   _2d_line(cube[1].screen[1], cube[5].screen[1], BLUE);
   _2d_line(cube[2].screen[1], cube[6].screen[1], BLUE);
   _2d_line(cube[3].screen[1], cube[7].screen[1], BLUE);

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()

void init_trig_tables()
{
 int d;

 for(d = 0; d < 512; d++)
  {
   sin_table[d] = sin(d * DEG_TO_PI);
   cos_table[d] = cos(d * DEG_TO_PI);
  }
}

void init()
{
 allegro_init();

 set_color_depth(16);
 if(set_gfx_mode(GFX_AUTODETECT, 1024, 768, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);

 init_trig_tables();
}

void rotate_vertex(VERTEX *v, int xang, int yang, int zang)
{
 _3D temp = v->local, local = v->local;

 temp.y = local.y * COS(xang) - local.z * SIN(xang);
 temp.z = local.y * SIN(xang) + local.z * COS(xang);
 v->local = temp;

 temp = v->local, local = v->local;
 temp.x = local.x * COS(yang) - local.z * SIN(yang);
 temp.z = local.x * SIN(yang) + local.z * COS(yang);
 v->local = temp;

 temp = v->local, local = v->local;
 temp.x = local.x * COS(zang) - local.y * SIN(zang);
 temp.y = local.x * SIN(zang) + local.y * COS(zang);
 v->local = temp;
}

void project_vertex(VERTEX *v, _3D world_pos)
{
 float z = v->local.z + world_pos.z;
 if(!z) { z = 1.0; }

 v->screen[0].x = (v->local.x + world_pos.x) / z * FOCAL_DISTANCE;
 v->screen[0].y = (v->local.y + world_pos.y) / z * FOCAL_DISTANCE;

 v->screen[1].x = (v->local.x + world_pos.x - 10.0) / z * FOCAL_DISTANCE;
 v->screen[1].y = (v->local.y + world_pos.y) / z * FOCAL_DISTANCE;
}

void _2d_line(_2D a, _2D b, int color)
{
 line(buffer, (int)a.x + (SCREEN_W >> 1), (int)a.y + (SCREEN_H >> 1),
              (int)b.x + (SCREEN_W >> 1), (int)b.y + (SCREEN_H >> 1), color);
}
