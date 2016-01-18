#include <allegro.h>
#include <math.h>

volatile float timer = 0.0;

void tick()
{
 timer += 1.0;
}
END_OF_FUNCTION(tick)

void init()
{
 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT, 512, 512, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();

 LOCK_VARIABLE(timer);
 LOCK_FUNCTION(tick);
}

inline int wrap(int x, int max)
{
 return (abs((int)x&(max - 1)));
}

inline int wrap32(int x)
{
 return (abs((int)x&31));
}

inline void bilerped_quad(BITMAP *dest, int minx, int miny, int maxx, int maxy, int h1, int h2, int h3, int h4)
{
 int dx = maxx - minx, dy = maxy - miny;
 float x, y, sx = 1.0 / (float)(dy), sy = 1.0 / (float)(dx), omx, omy;
 int h;

 for(y = 0.0; y < 1.0; y += sy)
  for(x = 0.0; x < 1.0; x += sx)
   {
    omx = 1.0 - x;
    omy = 1.0 - y;
    h = (int)(h1 * omx * omy + h2 * x * omy + h3 * y * omx + h4 * x * y);
    dest->line[(int)(miny + y * dy)][(int)(minx + x * dx)] = h;
   }
}

inline void scale_heightmap(BITMAP *input, BITMAP *output, int mag)
{
 int x, y, h1, h2, h3, h4;

 for(y = 0; y < input->h; y++)
  for(x = 0; x < input->w; x++)
   {
    h1 = input->line[y][x], h2 = input->line[y][wrap(x + 1, input->w)],
    h3 = input->line[wrap(y + 1, input->h)][x], h4 = input->line[wrap(y + 1, input->h)][wrap(x + 1, input->w)];
    bilerped_quad(output, x * mag, y * mag, (x + 1) * mag, (y + 1) * mag, h1, h2, h3,  h4);
   }
}

#define RAND_SCALE (1.0 / (float)RAND_MAX)

inline void make_noise(float a[32][32])
{
 int x, y;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   a[x][y] = (float)rand() * RAND_SCALE;
}

inline void smooth_noise(float a[32][32])
{
 float corners, sides, center;
 int x, y;
 float b[32][32];

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   {
    corners = (a[wrap32(x - 1)][wrap32(y - 1)] + a[wrap32(x + 1)][wrap32(y - 1)] + a[wrap32(x - 1)][wrap32(y + 1)] + a[wrap32(x + 1)][wrap32(y + 1)]) * 0.0625;
    sides = (a[wrap32(x - 1)][y] + a[wrap32(x + 1)][y] + a[x][wrap32(y - 1)] + a[x][wrap32(y + 1)]) * 0.125;
    center = a[x][y] * 0.25;
    b[x][y] = corners + sides + center;
   }

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   a[x][y] = b[x][y];
}

inline void noise_to_heightmap(BITMAP *input, float a[32][32])
{
 int x, y, n;

 for(y = 0; y < input->h; y++)
  for(x = 0; x < input->w; x++)
   {
    n = (int)(a[x][y] * 255.0);
    input->line[y][x] = makecol32(n, n, n);
   }
}

void noise_blend(float a[32][32], float b[32][32], float c[32][32], float k)
{
 int x, y;
 float l = 1.0 - k;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   c[x][y] = a[x][y] * l + b[x][y] * k;
}

typedef float NOISE[32][32];

void copy_noise(NOISE a, NOISE b)
{
 int x, y;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   a[x][y] = b[x][y];
}

void silly_bitmap(BITMAP *dest, BITMAP *src, int tx, int ty)
{
 int x, y;
 int dx, dy;
 
 for(y = 0; y < src->h; y++)
  for(x = 0; x < src->w; x++)
   {
    dx = wrap(x + tx, dest->w);
    dy = wrap(y + ty, dest->h);
    ((long *)dest->line[y])[x] = ((long *)src->line[dy])[dx];
   }
}

int main()
{
 init();

 //float noise1[32][32], noise2[32][32], noise3[32][32], noise4[32][32];
 NOISE noise[8], final[4];

 BITMAP *temp = create_bitmap_ex(8, 32, 32), *n32 = create_bitmap_ex(8, 32, 32),
        *n64 = create_bitmap_ex(8, 64, 64), *n128 = create_bitmap_ex(8, 128, 128), *n256 = create_bitmap_ex(8, 256, 256);
 BITMAP *clouds = create_bitmap(256, 256);
 BITMAP *t2 = create_bitmap(256, 256);

 srand(100);

 install_int(tick, 1);

 long frm = 0;
 int x, y, h1, h2, h3, h4, h;
 float cld[256][256];
 int max = -1, min = 5000;
 float k, sky = 80.0, cloud_smoothness = 0.97, c;
 int i;

 float table[500];

 for(i = 0; i < 500; i++)
  table[i] = 256.0 - pow(cloud_smoothness, (int)i) * 256.0;

 for(i = 0; i < 8; i++)
  make_noise(noise[i]);

 float blend_speed[4] = {0.03, 0.03 / 2.0, 0.03 / 4.0, 0.03 / 8.0};
 float blend[4] = {0.0, 0.0, 0.0, 0.0};

 int j;
 
 double tfm = 0.0;

 while(!key[KEY_ESC])
 {
  if(key[KEY_UP]) { sky += 1.0; }
  if(key[KEY_DOWN]) { sky -= 1.0; }
  
  if(sky < 30.0) sky = 30.0;
  if(sky > 120.0) sky = 120.0;
                     
  for(i = 0; i < 4; i++)
   if(blend[i] > 1.0)
    {
     copy_noise(noise[i], noise[i + 4]);
     make_noise(noise[i + 4]);
     blend[i] = 0.0;
    }

  for(i = 0; i < 4; i++)
   {
     blend[i] += blend_speed[i];
    noise_blend(noise[i], noise[i + 4], final[i], blend[i]);
    smooth_noise(final[i]);
   }

  noise_to_heightmap(n32, final[0]);
  noise_to_heightmap(temp, final[1]);
  scale_heightmap(temp, n64, 2);
  noise_to_heightmap(temp, final[2]);
  scale_heightmap(temp, n128, 4);
  noise_to_heightmap(temp, final[3]);
  scale_heightmap(temp, n256, 8);

 max = -1;
 min = 5000;

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    h1 = n32->line[wrap32(y)][wrap32(x)];
    h2 = n64->line[wrap(y, 64)][wrap(x, 64)];
    h3 = n128->line[wrap(y, 128)][wrap(x, 128)];
    h4 = n256->line[y][x];
    h = h4 + h3 * 0.5 + h2 * 0.25 + h1 * 0.125;
    cld[x][y] = h;

    if(h > max) max = h;
    if(h < min) min = h;
   }

  for(y = 0; y < 256; y++)
   for(x = 0; x < 256; x++)
    {
     k = (cld[x][y] - min) / (float)(max - min) * 255.0;
     c = k - sky;
     if(c < 0.0) c = 0.0;
     k = table[(int)c];
     ((long *)clouds->line[y])[x] = makecol32(k, k, k);
    }

 tfm += 1.0;
 silly_bitmap(t2, clouds, (int)tfm, (int)tfm);
 //blit(t2, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 
 stretch_blit(t2, screen, 0, 0, t2->w, t2->h,
                   0, 0, SCREEN_W, SCREEN_H);
 
 frm++;
}

 printf("%f", frm / (timer / 1000.0));
 clear_keybuf();
 readkey();
 save_bitmap("output.bmp", clouds, NULL);

 destroy_bitmap(clouds);
 destroy_bitmap(n256);
 destroy_bitmap(n128);
 destroy_bitmap(n64);
 destroy_bitmap(n32);
 destroy_bitmap(temp);
 destroy_bitmap(t2);

 return 0;
}
END_OF_MAIN()
