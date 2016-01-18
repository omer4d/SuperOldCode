#include <allegro.h>
#include <math.h>
#include "vec3f.h"

BITMAP *buffer;

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
}

#define NORMAP_W (2048)
#define NORMAP_H (NORMAP_W / 2)

int vec3f_to_int(VEC3F v)
{
 return makecol32((v.x + 1.0) * 127.5, (v.y + 1.0) * 127.5, (v.z + 1.0) * 127.5);
}

VEC3F texcoord_to_vec3f(int i, int j)
{
 float alpha = (float)i / (float)(NORMAP_W - 1) * 2.0 * M_PI;
 float beta = (float)j / (float)(NORMAP_H - 1) * M_PI + 0.5 * M_PI;
 VEC3F v;

 v.x = cos(beta) * cos(alpha);
 v.y = sin(beta);
 v.z = cos(beta) * sin(alpha);

 return v;
}

int main()
{
 int exit = 0;
 int i, j;

 init();

 BITMAP *height = load_bitmap("data/height.bmp", NULL);
 BITMAP *normap = create_bitmap(NORMAP_W, NORMAP_H);
 float height_scale = 0.00005;

 for(j = 0; j < NORMAP_H; j++)
  {
   printf("%f\n", (float)j / (float)NORMAP_H * 100.0);

  for(i = 0; i < NORMAP_W; i++)
   {
    float h = getr(getpixel(height, i, j)) * height_scale;
    VEC3F a = USCALE_VEC3F(texcoord_to_vec3f(i, j), 1.0 + h);

    h = getr(getpixel(height, (i + 1) % (normap->w - 1), j)) * height_scale;
    VEC3F b = USCALE_VEC3F(texcoord_to_vec3f((i + 1) % (normap->w - 1), j), 1.0 + h);

    h = getr(getpixel(height, (i + 1) % (normap->w - 1), (j + 1) % (normap->h - 1))) * height_scale;
    VEC3F c = USCALE_VEC3F(texcoord_to_vec3f((i + 1) % (normap->w - 1), (j + 1) % (normap->h - 1)), 1.0 + h);

    VEC3F n = NORMALIZED_VEC3F(vec3f_normal(a, b, c));

    putpixel(normap, i, j, vec3f_to_int(n));
   }
  }
   
 save_bitmap("normal.bmp", normap, NULL);

 destroy_bitmap(height);
 destroy_bitmap(normap);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
