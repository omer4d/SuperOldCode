#ifndef _FLOATMAP_H_
#define _FLOATMAP_H_

typedef struct {
 int w, h;
 float **line;
}FLOATMAP;

void clear_floatmap(FLOATMAP *map)
{
 int i, j;

 for(j = 0; j < map->h; j++)
  for(i = 0; i < map->w; i++)
   map->line[j][i] = 0.0f;
}

FLOATMAP *create_floatmap(int w, int h)
{
 int j;
 FLOATMAP *map;

 map = (FLOATMAP *)malloc(sizeof(FLOATMAP));
 map->w = w;
 map->h = h;
 map->line = (float **)malloc(h * sizeof(float *));
 for(j = 0; j < h; j++)
  map->line[j] = (float *)malloc(w * sizeof(float));
 clear_floatmap(map);

 return map;
}

void destroy_floatmap(FLOATMAP *map)
{
 int j;

 for(j = 0; j < map->h; j++)
  free(map->line[j]);
 free(map->line);
 free(map);
}

void f_circlefill(FLOATMAP *map, int cx, int cy, int r, float c)
{
 int x, y, dx, dy;

 for(y = cy - r; y < cy + r + 1; y++)
  for(x = cx - r; x < cx + r + 1; x++)
   {
    dx = cx - x;
    dy = cy - y;
    if((dx * dx + dy * dy) <= (r * r))
     map->line[y][x] = c;
   }
}

void f_tobitmap(BITMAP *buffer, FLOATMAP *map)
{
 int x, y, c;

 for(y = 0; y < map->h; y++)
  for(x = 0; x < map->w; x++)
   {
    c = (int)map->line[y][x];
    if(c < 0) c = 0;
    if(c > 255) c = 255;
    if(c > 0.0000001)
    _putpixel32(buffer, x, y, makecol32(c, 255, 0));
    //else
     //_putpixel32(buffer, x, y, 0);
   }
}

void f_putpixel(FLOATMAP *map, int x, int y, float c)
{
 map->line[y][x] = c;
}

float f_getpixel(FLOATMAP *map, int x, int y)
{
 return map->line[y][x];
}

void cpy_floatmap(FLOATMAP *map1, FLOATMAP *map2)
{
 int x, y;

 for(y = 0; y < map1->h; y++)
  for(x = 0; x < map1->w; x++)
   map1->line[y][x] = map2->line[y][x];
}

#endif
