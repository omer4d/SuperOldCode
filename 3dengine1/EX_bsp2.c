#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "vec2f.h"
#include "vec3f.h"
#include "mat16f.h"
#include "base.h"
#include "rendering.h"
#include "static_bsp_3d.h"

#define CAM_SPEED 0.8

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

 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0))
  {
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 init_engine(SCREEN_W, SCREEN_H);
 point_at_color_buffer(&buffer);
 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, 0.0, 200.0);
 set_znear(1.0);
 set_clipping_rect(10, 10, buffer->w - 11, buffer->h - 11);
 init_clipper();
 set_rasterizer_function(flat_zbuff_sides);
 set_world_clip_func(2, wclip_to_rend_int, 0, 0, 0, 0);
 set_scr_clip_func(4, rend_int_to_tri, 0, 0, 0, 0);

 install_keyboard();
 install_mouse();
 srand(time(NULL));
}

void traverse_tree(STATIC_BSP_NODE *tree, int node, VEC3F p, VERTEX map_vertex[], int order)
{
 int i;
 float k;

 if(tree != NULL)
  {
   VEC3F vert = ZERO_VEC3F;
   for(i = 0; i < tree[node].poly.vnum; i++)
    vert = VEC3F_SUM(vert, map_vertex[tree[node].poly.vind[i]].object);
   vert = USCALE_VEC3F(vert, 1.0 / tree[node].poly.vnum);
   k = VEC3F_DOT_PRODUCT(tree[node].poly.normal, VEC3F_DIFF(vert, p));

   if(k * order < 0.0)
    {
     if(tree[node].right != -1)
      traverse_tree(tree, tree[node].right, p, map_vertex, order);
     if(tree[node].flag == 1)
      render_poly3d(&tree[node].poly, map_vertex);
     if(tree[node].left != -1)
      traverse_tree(tree, tree[node].left, p, map_vertex, order);
    }

   else
    {
     if(tree[node].left != -1)
      traverse_tree(tree, tree[node].left, p, map_vertex, order);
     if(tree[node].flag == 1)
      render_poly3d(&tree[node].poly, map_vertex);
     if(tree[node].right != -1)
      traverse_tree(tree, tree[node].right, p, map_vertex, order);
    }
  }
}

void mark_nodes_inside_volume(STATIC_BSP_NODE tree[], int node, VERTEX map_vertex[], VEC3F volume[], int vnum, int *calls)
{
 //(*calls)++;
 int which = classify_volume(map_vertex[tree[node].poly.vind[0]].object,
                             tree[node].poly.normal, volume, vnum);

 if(tree[node].left == -1 && tree[node].right == -1)
  (*calls)++;

 if(which == BSP_SPANNED)
   tree[node].flag = 1;
 if((which == BSP_INFRONT || which == BSP_SPANNED) && tree[node].right != -1)
  mark_nodes_inside_volume(tree, tree[node].right, map_vertex, volume, vnum, calls);
 if((which == BSP_BEHIND || which == BSP_SPANNED) && tree[node].left != -1)
  mark_nodes_inside_volume(tree, tree[node].left, map_vertex, volume, vnum, calls);
}

int main()
{
 int exit_flag = 0, i, j;
 MAT16F tmat;

 int vnum, node_num;
 VERTEX *map_vertex;
 STATIC_BSP_NODE *tree;

 init();
 
 load_static_bsp("map.bsp", &map_vertex, &vnum, &tree, &node_num);
 printf("Vertex Number: %d.\nNode Number: %d.\n", vnum, node_num);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
 install_int(update_fps, 1000);

 float frame = 0.0;

 VEC3F view_volume[5];
 VEC3F cam_pos = vec3f(0.0, 3.0, 0.0), cam_dir, cam_dir_normal, cam_ang = vec3f(0.0, 0.0, 0.0);

 while(!exit_flag)
  {              
   int mx, my;
   get_mouse_mickeys(&mx, &my);
   position_mouse(SCREEN_W / 2, SCREEN_H / 2);

   cam_ang.x += my * 0.001;
   cam_ang.y -= mx * 0.001;
   cam_dir.x = CAM_SPEED * cos(0.5 * M_PI + cam_ang.y);
   cam_dir.y = CAM_SPEED * cos(0.5 * M_PI + cam_ang.x);
   cam_dir.z = CAM_SPEED * sin(0.5 * M_PI + cam_ang.y);
   cam_dir_normal = vec3f(-cam_dir.z, 0.0, cam_dir.x);

   if(key[KEY_ESC]) { exit_flag = 1; }
   if(key[KEY_W]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir); }
   if(key[KEY_S]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir); }
   if(key[KEY_A]) { cam_pos = VEC3F_SUM(cam_pos, cam_dir_normal); }
   if(key[KEY_D]) { cam_pos = VEC3F_DIFF(cam_pos, cam_dir_normal); }

   set_fov(90.0);
   if(mouse_b > 1)
    set_fov(30.0);

   build_view_volume(view_volume);

   reset_mat16f(tmat);
   rotate_x_mat16f(tmat, cam_ang.x);
   rotate_y_mat16f(tmat, cam_ang.y);
   rotate_z_mat16f(tmat, 0.0);
   translate_mat16f(tmat, cam_pos.x, cam_pos.y, cam_pos.z);

   for(i = 0; i < 5; i++)
    transform_vec3f(&view_volume[i], view_volume[i], tmat);

   reset_mat16f(tmat);
   translate_mat16f(tmat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_z_mat16f(tmat, 0.0);
   rotate_y_mat16f(tmat, -cam_ang.y);
   rotate_x_mat16f(tmat, -cam_ang.x);

   for(i = 0; i < vnum; i++)
    {
     transform_vec3f(&map_vertex[i].trans, map_vertex[i].object, tmat);
     project_vertex(&map_vertex[i]);
    }

   for(i = 0; i < node_num; i++)
    tree[i].flag = 0;
   int checks = 0;
   mark_nodes_inside_volume(tree, 0, map_vertex, view_volume, 5, &checks);
   int count = 0;
   for(i = 0; i < node_num; i++)
    if(tree[i].flag == 1)
     count++;

   clear_to_color(buffer, 0);
   clear_to_color(BASE_INT_z_buffer, BASE_INT_z_buffer_precision);
   traverse_tree(tree, 0, cam_pos, map_vertex, -1);

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
      textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   textprintf_ex(buffer, font, 10, 20, makecol(255, 255, 255), 0, "Rendered: %d of nodes, %d of faces.",
   (int)((float)checks / (float)node_num * 100.0), (int)((float)count / (float)node_num * 100.0));
   //textprintf_ex(buffer, font, 10, 20, makecol(255, 255, 255), 0, "%d", checks);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 deinit_engine();

 for(i = 0; i < node_num; i++)
  free(tree[i].poly.vind);
 free(tree);
 free(map_vertex);

 return 0;
}
END_OF_MAIN()
