#include <math.h>
#include <allegro.h>

typedef struct {
 float x, y, z;
}_3D;

typedef struct {
 _3D local, screen;
}VERTEX;

BITMAP *buffer;

float sin_table[512], cos_table[512];
int char_br[94];

void char_br_init();


void ascii_art(BITMAP *input, BITMAP *output)
{
 int x, y, z, c, r, g, b;
 float k;
     
 for(y = 0; y < input->h; y++)
  for(x = 0; x < output->w; x++)
   {
    z = ((x + 1) << 3) - 1;
    r = 0, g = 0, b = 0;

    c = _getpixel16(input, z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, --z, y); r += getr16(c); g += getg16(c); b += getb16(c);
    r = r >> 3; g = g >> 3; b = b >> 3;

    _putpixel16(input, x, y, makecol16(r, g, b));
   }

 for(y = 0; y < output->h; y++)
  for(x = 0; x < output->w; x++)
   {
    z = ((y + 1) << 3) - 1;
    r = 0, g = 0, b = 0;

    c = _getpixel16(input, x, z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    c = _getpixel16(input, x, --z); r += getr16(c); g += getg16(c); b += getb16(c);
    r = r >> 3; g = g >> 3; b = b >> 3;

    _putpixel16(output, x, y, makecol16(r, g, b));
   }

 for(y = 0; y < output->h; y++)
  for(x = 0; x < output->w; x++)
   {
    z = rand() % 94;
    c = _getpixel16(output, x, y);
    r = getr16(c); g = getg16(c); b = getb16(c);
    k = (100 - char_br[z]) * 0.01;
    r = (int)(r * k); g = (int)(g * k); b = (int)(b * k);
    textprintf_ex(input, font, x << 3, y << 3, c, makecol16(r, g, b), "%c", z + 33);
   }
}

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

void normalize(_3D *v)
{
 float len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

 if(len == 0.0)
  len = 1.0;

 v->x /= len;
 v->y /= len;
 v->z /= len;
}

_3D normal(_3D v1, _3D v2, _3D v3)
{
 _3D a, b, n;

 a.x = v1.x - v2.x;
 a.y = v1.y - v2.y;
 a.z = v1.z - v2.z;

 b.x = v2.x - v3.x;
 b.y = v2.y - v3.y;
 b.z = v2.z - v3.z;

 n.x = (a.y * b.z) - (a.z * b.y);
 n.y = (a.z * b.x) - (a.x * b.z);
 n.z = (a.x * b.y) - (a.y * b.x);
 normalize(&n);

 return n;
}

float vector_dot_product(_3D a, _3D b)
{
 return a.x * b.x + a.y * b.y + a.z * b.z;
}

_3D light = {0.0, 0.0, -10.0};
_3D light2 = {0.0, 50.0, 0.0};

void draw_quad(BITMAP *buffer, VERTEX a, VERTEX b, VERTEX c, VERTEX d, int col)
{
 _3D n =  normal(c.local, b.local, a.local);
 normalize(&light);

 float li = vector_dot_product(n, light);
 if(li > 1.0)
  li = 1.0;
 if(li < 0.0)
  li = 0.0;
  
 normalize(&light2);

 float li2 = 0.7 * vector_dot_product(n, light2);
 if(li2 > 1.0)
  li2 = 1.0;
 if(li2 < 0.0)
  li2 = 0.0;

 int tc;

 if(col != makecol(64, 0, 0))
 {
  tc = makecol(255, 255 - 255 * li2, 255 - 255 * li2);//col;
 col = makecol16(li * getr(tc), li * getg(tc), li * getb(tc));
}

/*
 if(col != makecol(64, 0, 0))
 {
  tc = makecol(255, 0, 0);//col;
 col = makecol16(li2 * getr(tc), li2 * getg(tc), li2 * getb(tc));
}
 */   
 V3D_f vtx1 = mine_to_yours(a, col), vtx2 = mine_to_yours(b, col), vtx3 = mine_to_yours(c, col), vtx4 = mine_to_yours(d, col);

 //if(polygon_z_normal_f(&vtx1, &vtx1, &vtx2) > 0.0)
  quad3d_f(buffer, POLYTYPE_FLAT | POLYTYPE_ZBUF, NULL, &vtx1, &vtx2, &vtx3, &vtx4);
  //quad3d_f(buffer, POLYTYPE_GCOL | POLYTYPE_ZBUF, NULL, &vtx1, &vtx2, &vtx3, &vtx4);
}

int main()
{
 int exit_flag = 0, i;
 ZBUFFER *z_buffer;
 BITMAP *final;

 VERTEX cube[8], shadow[8];
 BITMAP *tx;

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
   char_br_init();
 srand(time(NULL));
 tx = load_bitmap("3dcube.bmp", NULL);

 z_buffer = create_zbuffer(buffer);
 set_zbuffer(z_buffer);
 final = create_bitmap(SCREEN_W >> 3, SCREEN_H >> 3);

 while(!exit_flag)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit_flag = 1; }
     if(key[KEY_LEFT]) { world_pos.x -= 1.0; }
     if(key[KEY_RIGHT]) { world_pos.x += 1.0; }
     if(key[KEY_UP]) { world_pos.z += 3.0; }
     if(key[KEY_DOWN]) { world_pos.z -= 3.0; }
    }

   clear_to_color(buffer, makecol(128, 128, 128));
   
   rectfill(buffer, 0, SCREEN_H / 2, SCREEN_W, SCREEN_H, makecol(128, 0, 0));
   draw_sprite(buffer, tx, 0, 0);
   
   clear_zbuffer(z_buffer, 0.0);

  int x = 0, y = 0, z = 0;

  //if(key[KEY_SPACE]) y+=3;
  
   for(i = 0; i < 8; i++)
    {
     rotate_vertex(&cube[i], 2, 2, 2);
     project_vertex(&cube[i], world_pos);
    }
    
   for(i = 0; i < 8; i++)
    {
     shadow[i].local.x = cube[i].local.x;
     shadow[i].local.y = 20.0;
     shadow[i].local.z = cube[i].local.z;
     project_vertex(&shadow[i], world_pos);
   }

   draw_quad(buffer, cube[0], cube[1], cube[2], cube[3], makecol(128, 0, 0));
   draw_quad(buffer, cube[7], cube[6], cube[5], cube[4], makecol(0, 0, 128));
   draw_quad(buffer, cube[4], cube[5], cube[1], cube[0], makecol(0, 128, 0));
   draw_quad(buffer, cube[6], cube[7], cube[3], cube[2], makecol(128, 128, 0));
   draw_quad(buffer, cube[0], cube[3], cube[7], cube[4], makecol(128, 0, 128));
   draw_quad(buffer, cube[5], cube[6], cube[2], cube[1], makecol(0, 128, 128));


   draw_quad(buffer, shadow[0], shadow[1], shadow[2], shadow[3], makecol(64, 0, 0));
   draw_quad(buffer, shadow[7], shadow[6], shadow[5], shadow[4], makecol(64, 0, 0));
   draw_quad(buffer, shadow[4], shadow[5], shadow[1], shadow[0], makecol(64, 0, 0));
   draw_quad(buffer, shadow[6], shadow[7], shadow[3], shadow[2], makecol(64, 0, 0));
   draw_quad(buffer, shadow[0], shadow[3], shadow[7], shadow[4], makecol(64, 0, 0));
   draw_quad(buffer, shadow[5], shadow[6], shadow[2], shadow[1], makecol(64, 0, 0));

   ascii_art(buffer, final);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }

destroy_bitmap(tx);
destroy_bitmap(final);
 destroy_bitmap(buffer);
 destroy_zbuffer(z_buffer);
 return 0;
}
END_OF_MAIN()

void char_br_init()
{
 BITMAP *character = create_bitmap(8, 8);
 int x, y, i;

 for(i = 0; i < 94; i++)
  {
   textprintf_ex(character, font, 0, 0, makecol16(255, 255, 255), 0, "%c", i + 33);

   for(y = 8; y--;)
    for(x = 8; x--;)
     if(_getpixel16(character, x, y))
      char_br[i]++;

   char_br[i] = (int)(char_br[i] * 1.5625);
  }

 destroy_bitmap(character);
}

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
 if(set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0))
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

 v->screen.x = SCREEN_W * 0.5 + (v->local.x + world_pos.x) / z * FOCAL_DISTANCE;
 v->screen.y = SCREEN_H * 0.5 + (v->local.y + world_pos.y) / z * FOCAL_DISTANCE;
 v->screen.z = z;
}
