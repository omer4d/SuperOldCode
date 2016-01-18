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
#include "bsp.h"

#define GFX_W (320)
#define GFX_H (240)

BITMAP *buffer, *zbuffer, *texture, *texture2, *skin;

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
 skin = load_bitmap("data/babe_skin.bmp", NULL);
 texture2 = load_bitmap("data/color.bmp", NULL);

 setup_projection(90.0, -1.333, 1.333, -1.0, 1.0, -1.0, 200.0);
 init_renderer(GFX_H);
 bind_color_buffer(buffer);
 bind_zbuffer(zbuffer);
 bind_texture(texture);
}


int val = 0;

void draw_wall(VERTEX map_vertex[], int a, int b)
{
 TRI temp[2];
 float l = VEC3F_LENGTH(VEC3F_DIFF(map_vertex[a * 2].local,
                                   map_vertex[b * 2].local)) / 20.0;

 temp[0].a = a * 2 + 1;
 temp[0].b = b * 2 + 1;
 temp[0].c = b * 2;
 temp[0].tex[0] = vec2f(0.0, 0.0);
 temp[0].tex[1] = vec2f(1.0 * l, 0.0);
 temp[0].tex[2] = vec2f(1.0 * l, 1.0);

 temp[1].a = b * 2;
 temp[1].b = a * 2;
 temp[1].c = a * 2 + 1;
 temp[1].tex[0] = vec2f(1.0 * l, 1.0);
 temp[1].tex[1] = vec2f(0.0, 1.0);
 temp[1].tex[2] = vec2f(0.0, 0.0);

 render_tri(&temp[0], map_vertex);
 render_tri(&temp[1], map_vertex);
}

void traverse_tree(NODE *node, VEC2F p, VEC2F v, VEC2F vertex[], VERTEX map_vertex[])
{
 float k;
 int j = 1;
 if(key[KEY_SPACE])
  j = -1;

 if(node != NULL)
  {
   VEC2F vert = vec2f(map_vertex[node->a * 2].local.x, map_vertex[node->a * 2].local.z);
   k = VEC2F_DOT_PRODUCT(node->n, VEC2F_DIFF(vert, p));

   if(k * j < 0.0)
    {
     if(node->right != NULL)
      traverse_tree(node->right, p, v, vertex, map_vertex);
     //if(in_view(p, v, vertex[node->a], vertex[node->b]))
      draw_wall(map_vertex, node->a, node->b);
     if(node->left != NULL)
      traverse_tree(node->left, p, v, vertex, map_vertex);
    }

   else
    {
     if(node->left != NULL)
      traverse_tree(node->left, p, v, vertex, map_vertex);
     //if(in_view(p, v, vertex[node->a], vertex[node->b]))
      draw_wall(map_vertex, node->a, node->b);
     if(node->right != NULL)
      traverse_tree(node->right, p, v, vertex, map_vertex);
    }
  }
}


#define WALL_HEIGHT 150.0
#define SPEED (0.3)

int main()
{
 int exit_flag = 0, i;
 MAT16F tmat, modelmat;
 VEC3F pos = vec3f(0.0, 0.0, 0.5), ang = vec3f(0.0, 0.0, 0.0);

 int vnum, snum;
 VEC2F *map_vec2f;
 WALL_SEGMENT *map_segment;
 VERTEX *map_vertex;

 MD2_MODEL mdl;
 VERTEX *model_verts;
 TRI *model_tris;

 VERTEX floor[4];
 floor[0].local = vec3f(-200.0, 0.0, 200.0);
 floor[1].local = vec3f(200.0, 0.0, 200.0);
 floor[2].local = vec3f(200.0, 0.0, -200.0);
 floor[3].local = vec3f(-200.0, 0.0, -200.0);
 
  NODE *head = NULL;

 init();
 
 
  if(load_md2("data/babe.md2", &mdl))
  {
   allegro_message("Error: I need the MD2 model, stupid!");
   exit(1);
  }

 convert_md2_to_base(&model_verts, &model_tris, &mdl, 0.2);

 load_map("map.txt", &map_vec2f, &map_segment, &vnum, &snum, 1.0);

 head = (NODE *)malloc(sizeof(NODE));
 head->val = val;
 head->a = map_segment[0].a;
 head->b = map_segment[0].b;
 head->n = NORMALIZED_NORMAL_VEC2F(map_vec2f[map_segment[0].a], map_vec2f[map_segment[0].b]);
 head->parent = NULL;
 head->left = NULL;
 head->right = NULL;
 val++;
 
 for(i = 1; i < snum; i++)
  {
   add_node(head, &val, map_segment[i].a, map_segment[i].b, &map_vec2f, &vnum);
   val++;
  }
  
  map_vertex = (VERTEX *)malloc(vnum * 2 * sizeof(VERTEX));

 for(i = 0; i < vnum; i++)
  {
   map_vertex[i * 2].local = USCALE_VEC3F(vec3f(map_vec2f[i].x, 0.0, map_vec2f[i].y), 0.2);
   map_vertex[i * 2 + 1].local = USCALE_VEC3F(vec3f(map_vec2f[i].x, WALL_HEIGHT, map_vec2f[i].y), 0.2);
   map_vec2f[i] = USCALE_VEC2F(map_vec2f[i], 0.2);
  }

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
   cam_dir.y = 0.0;
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

   convert_md2_frame_to_base(model_verts, &mdl, frame, 0.2);
   frame += 0.1;
   reset_mat16f(modelmat);
   rotate_mat16f(modelmat, M_PI * 1.5, 0.0, 0.0);
   translate_mat16f(modelmat, 0.0, 5.0, 0.0);
   translate_mat16f(modelmat, -cam_pos.x, -cam_pos.y, -cam_pos.z);
   rotate_mat16f(modelmat, -cam_ang.x, -cam_ang.y, 0.0);

   for(i = 0; i < vnum * 2; i++)
    transform_vec3f(&map_vertex[i].world, map_vertex[i].local, tmat);
   for(i = 0; i < 4; i++)
    transform_vec3f(&floor[i].world, floor[i].local, tmat);
   for(i = 0; i < mdl.header.num_vertices; i++)
    transform_vec3f(&model_verts[i].world, model_verts[i].local, modelmat);

   clear_bitmap(buffer);
   clear_to_color(zbuffer, ZBUFFER_PRECISION);

   bind_texture(texture);

   VEC2F p = vec2f(cam_pos.x, cam_pos.z);
   VEC2F v = vec2f(cam_dir.x, cam_dir.z);
   traverse_tree(head, p, v, map_vec2f, map_vertex);

   bind_texture(skin);
   for(i = 0; i < mdl.header.num_tris; i++)
    {
     update_tri_normal(&model_tris[i], model_verts);
     model_tris[i].n = NORMALIZED_VEC3F(model_tris[i].n);
     if(cull_backface(&model_tris[i], model_verts))
      render_tri(&model_tris[i], model_verts);
    }
    
   bind_texture(texture2);
   TRI temp;
   temp.a = 0; temp.b = 1; temp.c = 2;
   temp.tex[0] = vec2f(0.0, 0.0); temp.tex[1] = vec2f(20.0, 0.0); temp.tex[2] = vec2f(20.0, 20.0);
   render_tri(&temp, floor);
   temp.a = 2; temp.b = 3; temp.c = 0;
   temp.tex[0] = vec2f(20.0, 20.0); temp.tex[1] = vec2f(20.0, 0.0); temp.tex[2] = vec2f(0.0, 0.0);
   render_tri(&temp, floor);

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), 0, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 destroy_bitmap(texture2);
 destroy_tree(head);
 free_md2(&mdl);
 free(model_tris);
 free(model_verts);
 free(map_vertex);
 free(map_vec2f);
 free(map_segment);
 destroy_bitmap(skin);
 destroy_bitmap(texture);
 destroy_bitmap(zbuffer);
 destroy_bitmap(buffer);
 deinit_renderer();

 return 0;
}
END_OF_MAIN()
