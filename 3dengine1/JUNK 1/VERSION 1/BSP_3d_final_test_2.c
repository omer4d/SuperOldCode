#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "md2.h"
#include "rendering.h"
#include "map.h"
#include "bsp_3d.h"

#define GFX_W (320)
#define GFX_H (240)

BITMAP *buffer, *zbuffer, *texture;

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
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, GFX_W, GFX_H, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 install_mouse();

 buffer = create_bitmap(SCREEN_W, SCREEN_H);
 zbuffer = create_bitmap_ex(32, SCREEN_W, SCREEN_H);
 texture = load_bitmap("data/wall.bmp", NULL);

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, -1.0, 200.0);
 init_renderer(GFX_H);
 bind_color_buffer(buffer);
 bind_zbuffer(zbuffer);
 bind_texture(texture);
}

#define WALL_HEIGHT 150.0
#define SPEED (0.3)

void omg_poly(POLY3D poly, VERTEX map_vertex[])
{
 int j;
 TRI tri;

 for(j = 1; j < poly.vnum - 1; j++)
  {
   tri.a = poly.vind[0];
   tri.b = poly.vind[j];
   tri.c = poly.vind[j + 1];
   tri.tex[0] = vec2f(0.0, 0.0);
   tri.tex[1] = vec2f(1.0, 0.0);
   tri.tex[2] = vec2f(1.0, 1.0);
   render_tri(&tri, map_vertex);
  }
}

void traverse_tree(BSP_NODE *node, VEC3F p, VEC3F v, VERTEX map_vertex[])
{
 float k;
 int j = 1;
 if(key[KEY_SPACE])
  j = -1;

 if(node != NULL)
  {
   VEC3F vert = map_vertex[node->poly.vind[0]].local;
   k = VEC3F_DOT_PRODUCT(node->poly.normal, VEC3F_DIFF(vert, p));

   if(k * j < 0.0)
    {
     if(node->right != NULL)
      traverse_tree(node->right, p, v, map_vertex);
      omg_poly(node->poly, map_vertex);
     //if(in_view(p, v, vertex[node->a], vertex[node->b]))
      //draw_wall(map_vertex, node->a, node->b);
     if(node->left != NULL)
      traverse_tree(node->left, p, v, map_vertex);
    }

   else
    {
     if(node->left != NULL)
      traverse_tree(node->left, p, v, map_vertex);
     //if(in_view(p, v, vertex[node->a], vertex[node->b]))
      //draw_wall(map_vertex, node->a, node->b);
      omg_poly(node->poly, map_vertex);
     if(node->right != NULL)
      traverse_tree(node->right, p, v, map_vertex);
    }
  }
}

int main()
{
 int exit_flag = 0, i, j;
 MAT16F tmat;
 VEC3F pos = vec3f(0.0, 0.0, 0.5), ang = vec3f(0.0, 0.0, 0.0);

 int vnum, pnum;
 VEC3F *map_vec3f;
 POLY3D *map_poly;
 VERTEX *map_vertex;

 init();

 convert_map_to_3d("map.txt", &map_vec3f, &map_poly, &vnum, &pnum);

 BSP_NODE *root, *node;
 add_bsp_root(&root, &map_poly[0]);
 for(i = 1; i < pnum; i++)
  add_bsp_node(root, &map_poly[i], &map_vec3f, &vnum);

 map_vertex = (VERTEX *)malloc(vnum * sizeof(VERTEX));

 printf("%d", vnum);

 for(i = 0; i < vnum; i++)
  map_vertex[i].local = map_vec3f[i];

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 float frame = 0.0;

 VEC3F cam_pos = vec3f(0.0, 3.0, 0.0), cam_dir, cam_dir_normal, cam_ang = vec3f(0.0, 0.0, 0.0);

 while(!exit_flag)
  {              
   set_texture_mapping_mode(0);

   int mx, my;
   get_mouse_mickeys(&mx, &my);
   position_mouse(SCREEN_W / 2, SCREEN_H / 2);

   cam_ang.x += my * 0.001;
   cam_ang.y -= mx * 0.001;
   cam_dir.x = SPEED * cos(0.5 * M_PI + cam_ang.y);
   cam_dir.y = 0.0;//SPEED * cos(0.5 * M_PI + cam_ang.x);
   cam_dir.z = SPEED * sin(0.5 * M_PI + cam_ang.y);
   cam_dir_normal = vec3f(-cam_dir.z, 0.0, cam_dir.x);

   if(key[KEY_ESC]) { exit_flag = 1; }
   if(key[KEY_W]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir); }
   if(key[KEY_S]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir); }
   if(key[KEY_A]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir_normal); }
   if(key[KEY_D]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir_normal); }
   //if(key[KEY_SPACE]) { set_texture_mapping_mode(1); }

   reset_mat16f(tmat);
   translate_mat16f(tmat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_mat16f(tmat, -cam_ang.x, -cam_ang.y, 0.0);

   for(i = 0; i < vnum; i++)
    transform_vec3f(&map_vertex[i].world, map_vertex[i].local, tmat);

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   //for(i = 0; i < pnum; i++)
    //omg_poly(map_poly[i], map_vertex);


   traverse_tree(root, cam_pos, cam_dir, map_vertex);

   /*
   for(i = 0; i < vnum; i++)
    {
     project_vertex(&map_vertex[i]);
     putpixel(buffer,map_vertex[i].sx, map_vertex[i].sy, makecol(255, 255, 255));
    }
    */

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 free(map_vec3f);
 for(i = 0; i < pnum; i++)
  free(map_poly[i].vind);
 free(map_poly);
 destroy_bsp_tree(root);

 free(map_vertex);
 destroy_bitmap(texture);
 destroy_bitmap(zbuffer);
 destroy_bitmap(buffer);
 deinit_renderer();

 return 0;
}
END_OF_MAIN()
