#include <stdio.h>
#include <math.h>
#include <allegro.h>

#define WHITE makecol32(255, 255, 255)

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

#define FOCAL_DISTANCE 256.0

void init_trig_tables();
void init();
void rotate_vertex(VERTEX *v, int xang, int yang, int zang);
void project_vertex(VERTEX *v, _3D world_pos);
void _2d_line(_2D a, _2D b, int color);

void _2d_point(_2D p, int color)
{
 putpixel(buffer, (int)p.x + (SCREEN_W >> 1), (int)p.y + (SCREEN_H >> 1), WHITE);
}

void _2d_quad(_2D a, _2D b, _2D c, _2D d, int color)
{
 //putpixel(buffer, (int)p.x + (SCREEN_W >> 1), (int)p.y + (SCREEN_H >> 1), WHITE);
 _2d_line(a, b, color);
 _2d_line(b, c, color);
 _2d_line(c, d, color);
 _2d_line(d, a, color);
}

int main()
{
 int exit_flag = 0, i, j, h;
 BITMAP *heightmap;

 VERTEX terrain[80][80];

 _3D world_pos = {0.0, 50.0, 100.0};

 init();

 heightmap = load_bitmap("heightmap.bmp", NULL);

 for(j = 0; j < 80; j++)
  for(i = 0; i < 80; i++)
   {
    h = getr(getpixel(heightmap, i * 10, j * 10));

    terrain[i][j].local.x = (float)(i - 40);
    terrain[i][j].local.y = (float)(j - 40);
    terrain[i][j].local.z = (float)(h * 0.1);
   }
   
   for(j = 0; j < 80; j++)
    for(i = 0; i < 80; i++)
     {
      rotate_vertex(&terrain[i][j], 0, 128, 128);
      project_vertex(&terrain[i][j], world_pos);
      //_2d_point(terrain[i][j].screen, WHITE);
     }

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

   for(j = 0; j < 80; j++)
    for(i = 0; i < 80; i++)
     {
      rotate_vertex(&terrain[i][j], 0, 5, 0);
      project_vertex(&terrain[i][j], world_pos);
      //_2d_point(terrain[i][j].screen, WHITE);
     }
     
   for(j = 0; j < 79; j++)
    for(i = 0; i < 79; i++)
     {
            _2d_quad(terrain[i][j].screen[0], terrain[i + 1][j].screen[0],
               terrain[i + 1][j + 1].screen[0], terrain[i][j + 1].screen[0], makecol(255, 0, 0));
               
            _2d_quad(terrain[i][j].screen[1], terrain[i + 1][j].screen[1],
               terrain[i + 1][j + 1].screen[1], terrain[i][j + 1].screen[1], makecol(0, 0, 255));
     }
     
     
     
   

   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

 destroy_bitmap(heightmap);
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
 if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
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

 v->screen[1].x = (v->local.x + world_pos.x + 4.0) / z * FOCAL_DISTANCE;
 v->screen[1].y = (v->local.y + world_pos.y) / z * FOCAL_DISTANCE;
}

void do_shit(BITMAP *bitmap, int x, int y, int b)
{
 int a = getpixel(bitmap, x, y);
 putpixel(bitmap, x, y, makecol(getr(a) + getr(b), getg(a) + getg(b), getb(a) + getb(b)));
}


void _2d_line(_2D a, _2D b, int color)
{
 do_line(buffer, (int)a.x + (SCREEN_W >> 1), (int)a.y + (SCREEN_H >> 1),
              (int)b.x + (SCREEN_W >> 1), (int)b.y + (SCREEN_H >> 1), color, do_shit);
}

/*
void _2d_line(_2D a, _2D b, int color)
{
 line(buffer, (int)a.x + (SCREEN_W >> 1), (int)a.y + (SCREEN_H >> 1),
              (int)b.x + (SCREEN_W >> 1), (int)b.y + (SCREEN_H >> 1), color);
}
*/
