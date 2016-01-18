#include <allegro.h>
#include <math.h>

#define RAND_SCALE (1.0 / (float)RAND_MAX)
float CLOUD_SMOOTHNESS = (0.96);
#define CLOUD_BLEND_SPEED (0.1)
#define CLOUD_DISPLACE_SPEED (0.5)

typedef float NOISE[32][32];

volatile float timer = 0.0;
long lummap_palette[256];

void tick()
{
 timer += 1.0;
}
END_OF_FUNCTION(tick)

void init()
{
 int i;

 allegro_init();
 set_color_depth(32);

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();

 LOCK_VARIABLE(timer);
 LOCK_FUNCTION(tick);

 for(i = 0; i < 256; i++)
  lummap_palette[i] = makecol32(0, 0, i);
}

inline int wrap(int x, int max);
inline int wrap32(int x);
inline void bilerped_quad(BITMAP *dest, int minx, int miny, int maxx, int maxy, int h1, int h2, int h3, int h4);
inline void scale_lummap(BITMAP *input, BITMAP *output, int mag);
inline void make_noise(NOISE a);
inline void smooth_noise(NOISE a);
inline void noise_to_lummap(BITMAP *input, NOISE a);
inline void noise_blend(NOISE a, NOISE b, NOISE c, float k);
inline void copy_noise(NOISE a, NOISE b);
inline void silly_bitmap(BITMAP *dest, BITMAP *src, int tx, int ty);
inline void evenly_sliced_palette(int color_num, int rgb[], long palette[256]);
inline void blend_palettes(long a[256], long b[256], long c[256], float k);
inline void noise_to_clouds(BITMAP *clouds, float sky, BITMAP *n32, BITMAP *n64, BITMAP *n128, BITMAP *n256);
inline void emboss(BITMAP *dest, BITMAP *src, int ex, int ey, float hightlight);
inline void apply_transformation(BITMAP *dest, BITMAP *src, BITMAP *transform);
inline void compose_final(BITMAP *dest, BITMAP *back, BITMAP *fore, float brightness, long palette[256]);
inline void blend_bitmaps(BITMAP *bmp1, BITMAP *bmp2, BITMAP *res, float k);

int main()
{
 init();

 NOISE noise[8], final[4];

 BITMAP *temp = create_bitmap_ex(8, 32, 32), *n32 = create_bitmap_ex(8, 32, 32),
        *n64 = create_bitmap_ex(8, 64, 64), *n128 = create_bitmap_ex(8, 128, 128), *n256 = create_bitmap_ex(8, 256, 256);

 BITMAP *clouds = create_bitmap(256, 256);
 BITMAP *temp_clouds = create_bitmap(256, 256);
 BITMAP *transform = create_bitmap(256, 256);
 BITMAP *back = load_bitmap("sky.bmp", NULL), *fore = load_bitmap("trees.bmp", NULL);
 BITMAP *calm = load_bitmap("calm.bmp", NULL), *storm = load_bitmap("storm.bmp", NULL);

 float displace_clouds = 0.0, brightness = 2.0, sky_amount = 100.0, sky_amount_change = 0.1, storm_size = 0.0;
 long frame_num = 0;
 int i, storm_size_changed = 1;

 long old_palette[256], output_palette[256];
 long palette1[256], palette2[256];
 int nuclear[] = {0, 0, 0, 0, 128, 128, 255, 255, 0, 255, 255, 255};
 int rainy[] = {0, 0, 0, 64, 0, 32, 0, 0, 64, 255, 255, 255};
 int fire[] = {0, 0, 0, 255, 0, 0, 255, 255, 0, 255, 255, 255};
 int grayscale[] = {0, 0, 0, 255, 255, 255};

 float cloud_blend_speed[4] = {CLOUD_BLEND_SPEED, CLOUD_BLEND_SPEED / 2.0, CLOUD_BLEND_SPEED / 4.0, CLOUD_BLEND_SPEED / 8.0};
 float cloud_blend[4] = {0.0, 0.0, 0.0, 0.0};
 float speed_scale = 0.0;

 float countdown = 200;

 srand(100);

 for(i = 0; i < 8; i++)
  make_noise(noise[i]);

 evenly_sliced_palette(4, rainy, palette1);
 evenly_sliced_palette(2, grayscale, palette2);
 blend_palettes(palette1, palette2, output_palette, 0.4);

 install_int(tick, 1);

 while(!key[KEY_ESC])
  {         
   if(keypressed())
    {
     if(key[KEY_UP]) { sky_amount += 1.0 * speed_scale; }
     if(key[KEY_DOWN]) { sky_amount -= 1.0 * speed_scale; }
     if(key[KEY_SPACE]) { brightness = 2.0; }
     if(key[KEY_LEFT]) { storm_size += 0.025 * speed_scale; storm_size_changed = 1; }
     if(key[KEY_RIGHT]) { storm_size -= 0.025 * speed_scale; storm_size_changed = 1; }
     if(key[KEY_1]) evenly_sliced_palette(4, rainy, palette1);
     if(key[KEY_2]) evenly_sliced_palette(4, nuclear, palette1);
     if(key[KEY_3]) evenly_sliced_palette(4, fire, palette1);
     if(key[KEY_4]) evenly_sliced_palette(2, grayscale, palette1);
     if(key[KEY_ENTER]) CLOUD_SMOOTHNESS = 0.995;
     else CLOUD_SMOOTHNESS = 0.96;

     blend_palettes(palette1, palette2, output_palette, 0.4);
    }

   countdown -= speed_scale;
   sky_amount += sky_amount_change * speed_scale;
   displace_clouds += CLOUD_DISPLACE_SPEED * speed_scale;

   if(brightness > 0.6) brightness -= 0.05 * speed_scale;
   if(storm_size < 0.0) storm_size = 0.0;
   if(storm_size > 1.0) storm_size = 1.0;
   if(sky_amount < 30.0) sky_amount = 30.0;
   if(sky_amount > 120.0) sky_amount = 120.0;

   for(i = 0; i < 4; i++)
    if(cloud_blend[i] > 1.0)
     {
      copy_noise(noise[i], noise[i + 4]);
      make_noise(noise[i + 4]);
      cloud_blend[i] = 0.0;
     }

   for(i = 0; i < 4; i++)
    {
     cloud_blend[i] += cloud_blend_speed[i] * speed_scale;
     noise_blend(noise[i], noise[i + 4], final[i], cloud_blend[i]);
     smooth_noise(final[i]);
    }

   noise_to_lummap(n32, final[0]);
   noise_to_lummap(temp, final[1]);
   scale_lummap(temp, n64, 2);
   noise_to_lummap(temp, final[2]);
   scale_lummap(temp, n128, 4);
   noise_to_lummap(temp, final[3]);
   scale_lummap(temp, n256, 8);

   if(rand() % ((int)(200 / speed_scale)) == 0 && speed_scale > 0.01)
    {
     brightness = 2.0;

     if(countdown < 0)
      {
       sky_amount_change *= -1.0;
       countdown = 100;
      }
    }

   noise_to_clouds(clouds, sky_amount, n32, n64, n128, n256);
   silly_bitmap(temp_clouds, clouds, displace_clouds * 2.0, displace_clouds);
   emboss(clouds, temp_clouds, 5, 10, brightness);

   if(storm_size_changed)
    {
     blend_bitmaps(storm, calm, transform, storm_size);
     storm_size_changed = 0;
    }

   apply_transformation(temp_clouds, clouds, transform);
   compose_final(temp_clouds, back, fore, brightness + 0.4, output_palette);
   stretch_blit(temp_clouds, screen,  0, 0, clouds->w, clouds->h, 0, 0, SCREEN_W, SCREEN_H);
   frame_num++;

   if(frame_num > 10)
    speed_scale = 70.0 / (frame_num / (timer / 1000.0));
  }

 printf("Average FPS: %f", frame_num / (timer / 1000.0));
 clear_keybuf();
 readkey();
 save_bitmap("output.bmp", temp_clouds, NULL);

 destroy_bitmap(storm);
 destroy_bitmap(calm);
 destroy_bitmap(fore);
 destroy_bitmap(back);
 destroy_bitmap(transform);
 destroy_bitmap(temp_clouds);
 destroy_bitmap(clouds);

 destroy_bitmap(n256);
 destroy_bitmap(n128);
 destroy_bitmap(n64);
 destroy_bitmap(n32);
 destroy_bitmap(temp);

 return 0;
}
END_OF_MAIN()

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

inline void scale_lummap(BITMAP *input, BITMAP *output, int mag)
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

inline void make_noise(NOISE a)
{
 int x, y;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   a[x][y] = (float)rand() * RAND_SCALE;
}

inline void smooth_noise(NOISE a)
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

inline void noise_to_lummap(BITMAP *input, NOISE a)
{
 int x, y, n;

 for(y = 0; y < input->h; y++)
  for(x = 0; x < input->w; x++)
   {
    n = (int)(a[x][y] * 255.0);
    input->line[y][x] = makecol32(n, n, n);
   }
}

inline void noise_blend(NOISE a, NOISE b, NOISE c, float k)
{
 int x, y;
 float l = 1.0 - k;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   c[x][y] = a[x][y] * l + b[x][y] * k;
}

inline void copy_noise(NOISE a, NOISE b)
{
 int x, y;

 for(y = 0; y < 32; y++)
  for(x = 0; x < 32; x++)
   a[x][y] = b[x][y];
}

inline void silly_bitmap(BITMAP *dest, BITMAP *src, int tx, int ty)
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

inline void evenly_sliced_palette(int color_num, int rgb[], long palette[256])
{
 int i, j, slice = 256 / (color_num - 1);
 float a;

 for(j = 0; j < color_num - 1; j++)
  for(i = slice * j; i < slice * (j + 1); i++)
   {
    a = (float)(i - slice * j) / slice;
    palette[i] = makecol32((1.0 - a) * rgb[j * 3] + a * rgb[(j + 1) * 3],
                           (1.0 - a) * rgb[j * 3 + 1] + a * rgb[(j + 1) * 3 + 1],
                           (1.0 - a) * rgb[j * 3 + 2] + a * rgb[(j + 1) * 3 + 2]);
   }
}

inline void blend_palettes(long a[256], long b[256], long c[256], float k)
{
 int i;

 for(i = 0; i < 256; i++)
  {
   c[i] = makecol32(k * getr32(a[i]) + (1.0 - k) * getr32(b[i]),
                    k * getg32(a[i]) + (1.0 - k) * getg32(b[i]),
                    k * getb32(a[i]) + (1.0 - k) * getb32(b[i]));
  }
}

inline void noise_to_clouds(BITMAP *clouds, float sky, BITMAP *n32, BITMAP *n64, BITMAP *n128, BITMAP *n256)
{
 int x, y, h, h1, h2, h3, h4;
 float noise_sum[256][256], table[500], min = 5000, max = -1, c, k;

 for(x = 0; x < 500; x++)
  table[x] = 256.0 - pow(CLOUD_SMOOTHNESS, x) * 256.0;

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    h1 = n32->line[wrap32(y)][wrap32(x)];
    h2 = n64->line[wrap(y, 64)][wrap(x, 64)];
    h3 = n128->line[wrap(y, 128)][wrap(x, 128)];
    h4 = n256->line[y][x];
    h = h4 + h3 * 0.5 + h2 * 0.25 + h1 * 0.125;
    noise_sum[x][y] = h;

    if(h > max) max = h;
    if(h < min) min = h;
   }

 float s = 256.0 / (float)(max - min);

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    k = (noise_sum[x][y] - min) * s;
    c = k - sky;
    if(c < 0.0) c = 0.0;
    k = table[(int)c];
    ((long *)clouds->line[y])[x] = lummap_palette[(int)k];
   }
}

inline void emboss(BITMAP *dest, BITMAP *src, int ex, int ey, float hightlight)
{
 int x, y, l1, l2, l3;

 for(y = ey; y < 256; y++)
  for(x = ex; x < 256; x++)
   {
    l1 = ((long *)src->line[y])[x];
    l2 = ((long *)src->line[y - ey])[x - ex];
    l3 = 0.4 * l1 + (l1 * (255 - l2) * 0.01) * hightlight;
    if(l3 > 255) l3 = 255;
    ((long *)dest->line[y])[x] = lummap_palette[l3];
   }
}

inline void apply_transformation(BITMAP *dest, BITMAP *src, BITMAP *transform)
{
 int x, y, d, dx, dy, dz, l;

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    d = ((long *)transform->line[y])[x];
    dx = getr32(d);
    dy = getg32(d);
    dz = getb32(d);

    if(dz) l = 0;
    else l = ((long *)src->line[dy])[dx];
    ((long *)dest->line[y])[x] = l;
   }
}

inline void compose_final(BITMAP *dest, BITMAP *back, BITMAP *fore, float brightness, long palette[256])
{
 int x, y, back_l, final_l;
 float cloud_k;

 for(y = 0; y < 256; y++)
  for(x = 0; x < 256; x++)
   {
    back_l = ((long *)back->line[y])[x];
    cloud_k = ((long *)dest->line[y])[x] * 0.00390625;
    final_l = back_l * (1.0 - cloud_k) * brightness + cloud_k * 255.0;
    if(final_l > 255) final_l = 255;
    final_l *= (((long *)fore->line[y])[x] * 0.00390625);
    if(final_l > 255) final_l = 255;
    if(final_l < 0) final_l = 0;
    ((long *)dest->line[y])[x] = palette[final_l];
   }
}

inline void blend_bitmaps(BITMAP *bmp1, BITMAP *bmp2, BITMAP *res, float k)
{
 int x, y, c1, c2, r, g, b;

 for(y = 0; y < bmp1->h; y++)
  for(x = 0; x < bmp1->w; x++)
   {
    c1 = ((long *)bmp1->line[y])[x];
    c2 = ((long *)bmp2->line[y])[x];
    r = getr32(c1) * k + getr32(c2) * (1.0 - k);
    g = getg32(c1) * k + getg32(c2) * (1.0 - k);
    b = getb32(c1) * k + getb32(c2) * (1.0 - k);
    ((long *)res->line[y])[x] = makecol32(r, g, b);
   }
}
