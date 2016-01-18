#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "map.h"
#include "bsp_3d.h"

BITMAP *buffer;
int fps, frame_count = 0;

void update_fps()
{
 fps = frame_count;
 frame_count = 0;
}
END_OF_FUNCTION(update_fps)

void init()
{
 allegro_init();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

int main()
{
 int i, exit_flag = 0;

 int vnum, pnum;
 VEC3F *map_vertex;
 POLY3D *map_poly;
 BSP_NODE *root, *node;

 convert_map_to_3d("map.txt", &map_vertex, &map_poly, &vnum, &pnum);

 add_bsp_root(&root, &map_poly[0]);
 for(i = 1; i < pnum; i++)
  add_bsp_node(root, &map_poly[i], &map_vertex, &vnum);

 init();

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 while(!exit_flag)
  {              
   if(key[KEY_ESC]) { exit_flag = 1; }

   clear_bitmap(buffer);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 free(map_vertex);
 for(i = 0; i < pnum; i++)
  free(map_poly[i].vind);
 free(map_poly);

 destroy_bsp_tree(root);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
