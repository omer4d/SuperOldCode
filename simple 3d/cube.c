#include <stdio.h>
#include <math.h>
#include <allegro.h>

typedef struct {
 float x, y, z;
}_3D;

typedef struct {
 _3D local, screen;
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
void _2d_line(_3D a, _3D b, int color);

V3D_f mine_to_yours(VERTEX mine, int c)
{
 V3D_f yours;

 yours.x = mine.screen.x;
 yours.y = mine.screen.y;
 yours.z = mine.screen.z;
 yours.c = c;

 return yours;
}

void draw_quad(BITMAP *buffer, VERTEX a, VERTEX b, VERTEX c, VERTEX d, int col)
{
 V3D_f vtx1 = mine_to_yours(a, col), vtx2 = mine_to_yours(b, col), vtx3 = mine_to_yours(c, col), vtx4 = mine_to_yours(d, col);

 //if(polygon_z_normal_f(&vtx1, &vtx1, &vtx2) > 0.0)
  quad3d_f(buffer, POLYTYPE_FLAT | POLYTYPE_ZBUF, NULL, &vtx1, &vtx2, &vtx3, &vtx4);
  //quad3d_f(buffer, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, &vtx1, &vtx2, &vtx3, &vtx4);
}

int main()
{
 int exit_flag = 0, i;
 ZBUFFER *z_buffer;

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
 
 z_buffer = create_zbuffer(buffer);
 set_zbuffer(z_buffer);
 int xang, yang, zang;

 while(!exit_flag)
  {
   xang = 0;
   yang = 0;
   zang = 0;

   rest(10);

   if(keypressed())
    {
     if(key[KEY_ESC]) { exit_flag = 1; }
     if(key[KEY_LEFT]) { world_pos.x -= 1.0; }
     if(key[KEY_RIGHT]) { world_pos.x += 1.0; }
     if(key[KEY_UP]) { world_pos.z += 3.0; }
     if(key[KEY_DOWN]) { world_pos.z -= 3.0; }
     if(key[KEY_PGUP]) { xang = 1; }
     if(key[KEY_PGDN]) { xang = -1; }
     if(key[KEY_HOME]) { yang = 1; }
     if(key[KEY_END]) { yang = -1; }
    }

   clear_to_color(buffer, 0);
   clear_zbuffer(z_buffer, 0.0);

   for(i = 0; i < 8; i++)
    {
     rotate_vertex(&cube[i], xang, yang, zang);
     project_vertex(&cube[i], world_pos);
    }

   draw_quad(buffer, cube[0], cube[1], cube[2], cube[3], makecol(128, 0, 0));
   draw_quad(buffer, cube[4], cube[5], cube[6], cube[7], makecol(0, 0, 128));
   draw_quad(buffer, cube[0], cube[1], cube[5], cube[4], makecol(0, 128, 0));
   draw_quad(buffer, cube[2], cube[3], cube[7], cube[6], makecol(128, 128, 0));
   draw_quad(buffer, cube[0], cube[3], cube[7], cube[4], makecol(128, 0, 128));
   draw_quad(buffer, cube[1], cube[2], cube[6], cube[5], makecol(0, 128, 128));

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(buffer);
 destroy_zbuffer(z_buffer);
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
 if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);

 init_trig_tables();
}

void rotate_vertex(_3D *output, _3D input, int xang, int yang, int zang)
{
 _3D temp = input, local = input;

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

 v->screen.x = SCREEN_W * 0.5 + (v->local.x + world_pos.x) / z * FOCAL_DISTANCE;
 v->screen.y = SCREEN_H * 0.5 + (v->local.y + world_pos.y) / z * FOCAL_DISTANCE;
 v->screen.z = z;
}
