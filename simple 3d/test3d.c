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

float dist_3d(_3D a, _3D b)
{
 _3D d = {a.x - b.x, a.y - b.y, a.z - b.z};
 return d.x * d.x + d.y * d.y + d.z * d.z;
}

int main()
{
 int exit_flag = 0, i;

 VERTEX cube[8];
 cube[0].local = (_3D){-10.0, -10.0, 0.0};
 cube[1].local = (_3D){10.0, -10.0, 0.0};
 cube[2].local = (_3D){10.0, 10.0, 0.0};
 cube[3].local = (_3D){-10.0, 10.0, -10.0};

 cube[4].local = (_3D){-10.0, -10.0, 10.0};
 cube[5].local = (_3D){10.0, -10.0, 10.0};
 cube[6].local = (_3D){10.0, 10.0, 10.0};
 cube[7].local = (_3D){-10.0, 10.0, 10.0};

 _3D world_pos = {0.0, 0.0, 100.0};

 init();
 
 
    printf("D1:=%f;\nD2:=%f;\nD3:=%f;\nD4:=%f;\n", dist_3d(cube[0].local, cube[1].local),
                               dist_3d(cube[1].local, cube[2].local),
                               dist_3d(cube[2].local, cube[3].local),
                               dist_3d(cube[3].local, cube[0].local));

 readkey();

 int xang, yang, zang;

 while(!exit_flag)
  {
   rest(10);

   xang = 0;
   yang = 0;
   zang = 0;

   if(keypressed())
    {
     if(key[KEY_ESC]) { exit_flag = 1; }
     if(key[KEY_A]) { world_pos.x -= 1.0; }
     if(key[KEY_D]) { world_pos.x += 1.0; }
     if(key[KEY_W]) { world_pos.z += 3.0; }
     if(key[KEY_S]) { world_pos.z -= 3.0; }
     if(key[KEY_UP]) { xang = 3; }
     if(key[KEY_DOWN]) { xang = -3; }
     if(key[KEY_RIGHT]) { yang = 3; }
     if(key[KEY_LEFT]) { yang = -3; }
    }

   clear_to_color(buffer, 0);

   for(i = 0; i < 8; i++)
    {
     rotate_vertex(&cube[i], xang, yang, zang);
     project_vertex(&cube[i], world_pos);
    }
/*
    printf("%f, %f, %f\n", dist_3d(cube[0].local, cube[1].local),
                         dist_3d(cube[1].local, cube[2].local),
                         dist_3d(cube[2].local, cube[0].local));
*/

    printf("Sx1:=%f;\nSy1:=%f;\nSx2:=%f;\nSy2:=%f;\nSx3:=%f;\nSy3:=%f;\nSx4:=%f;\nSy4:=%f;\n", cube[0].screen[0].x, cube[0].screen[0].y,
                                        cube[1].screen[0].x, cube[1].screen[0].y,
                                        cube[2].screen[0].x, cube[2].screen[0].y, cube[3].screen[0].x, cube[3].screen[0].y);

    printf("t = %f\n\n\n\n\n\n", (cube[0].local.x + world_pos.x) / cube[0].screen[0].x);

   /*printf("{%f %f %f}\n", cube[0].local.x + world_pos.x,
    cube[0].local.y + world_pos.y, cube[0].local.z + world_pos.z);
   */    

   _2d_line(cube[0].screen[0], cube[1].screen[0], RED);
   _2d_line(cube[1].screen[0], cube[2].screen[0], RED);
   _2d_line(cube[2].screen[0], cube[3].screen[0], RED);
   _2d_line(cube[3].screen[0], cube[0].screen[0], RED);

/*
   _2d_line(cube[4].screen[0], cube[5].screen[0], RED);
   _2d_line(cube[5].screen[0], cube[6].screen[0], RED);
   _2d_line(cube[6].screen[0], cube[7].screen[0], RED);
   _2d_line(cube[7].screen[0], cube[4].screen[0], RED);

   _2d_line(cube[0].screen[0], cube[4].screen[0], RED);
   _2d_line(cube[1].screen[0], cube[5].screen[0], RED);
   _2d_line(cube[2].screen[0], cube[6].screen[0], RED);
   _2d_line(cube[3].screen[0], cube[7].screen[0], RED);
*/

/*
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
*/

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
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0))
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
