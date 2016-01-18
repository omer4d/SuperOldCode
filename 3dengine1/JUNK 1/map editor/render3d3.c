#include <malloc.h>
#include <alleggl.h>
#include "vector.h"
#include "map.h"
#include "floatmap.h"

#define ASPECT_RATIO (float)SCREEN_W / (float)SCREEN_H
#define HEIGHT 5.0

void transform(float tx, float ty, float tz, float ax, float ay, float az)
{
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glRotatef(ax, 1, 0, 0);
 glRotatef(ay, 0, 1, 0);
 glRotatef(az, 0, 0, 1);
 glTranslatef(tx, ty, tz);
}

void init();

void vector_wall(VECTOR a, VECTOR b)
{
 VECTOR n = NORMALIZED_NORMAL(a, b);

 glNormal3f(n.x, 0.0, n.y);
 glColor3ub(255, 0, 0); glVertex3f(a.x, HEIGHT, a.y);
 glColor3ub(0, 255, 0); glVertex3f(b.x, HEIGHT, b.y);
 glColor3ub(0, 0, 255); glVertex3f(b.x, 0.0, b.y);
 glColor3ub(255, 255, 0); glVertex3f(a.x, 0.0, a.y);
}

void vector_wall_loop(VECTOR p[], int n, int flag)
{
 int i;

   for(i = 0; i < n - 1; i++)
    vector_wall(p[i], p[i + 1]);
   if(flag)
    vector_wall(p[i], p[0]);
}

void draw_map_3d(LINE_LOOP *loop, int t)
{
 int i, j;
     
  for(j = 0; j < t; j++)
   {
    if(loop[j].n > 4 && !loop[j].closed)
     {
      int pn = loop[j].n / 2;
      vector_wall_loop(loop[j].point, pn, loop[j].closed);
 
      for(i = 0; i < pn - 1; i++)
        vector_wall(loop[j].point[i + pn], loop[j].point[i + pn + 1]);

      vector_wall(loop[j].point[0], loop[j].point[pn]);
      vector_wall(loop[j].point[pn - 1], loop[j].point[pn + pn - 1]);
     }

    if(loop[j].n > 2 && loop[j].closed)
     {
      int pn = loop[j].n;
      vector_wall_loop(loop[j].point, loop[j].n, loop[j].closed);
     }

    if(loop[j].n == 4 && !loop[j].closed)
     {
      vector_wall(loop[j].point[0], loop[j].point[1]);
      vector_wall(loop[j].point[2], loop[j].point[3]);
      vector_wall(loop[j].point[0], loop[j].point[2]);
      vector_wall(loop[j].point[1], loop[j].point[3]);
     }
   }
}

#define SPEED (0.015)

GLuint make_texture(char *path, int flags)
{
 BITMAP *texture = load_bitmap(path, NULL);

 if(!texture)
  {
   allegro_message("Failed loading texture '%s'.", path);
   exit(1);
  }

 int id = allegro_gl_make_texture_ex(flags, texture, GL_RGBA8);
 destroy_bitmap(texture);
 return id;
}

int main()
{
 int exit = 0;
 int snum;
 LINE_LOOP *map = load_map("map.txt", &snum, 0.2);

 init();
 
 float cam_x = 0.0, cam_y = 2.0, cam_z = 30.0, cam_ax = 0.0, cam_ay = 0.0;

 printf("%d %d", sizeof(float), sizeof(float *));

 while(!exit)
  {
   VECTOR cam_dir = vector(SPEED * cos(M_PI * 0.5 - cam_ay * M_PI / 180.0), SPEED * sin(cam_ay * M_PI / 180.0 - M_PI * 0.5));

   int mx, my;
   get_mouse_mickeys(&mx, &my);
   //position_mouse(240, 320);
   
   cam_ay += mx * 0.03;
   cam_ax -= my * 0.03;
   if(cam_ax > 60) cam_ax = 60;
   if(cam_ax < -40) cam_ax = -40;
 
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
     if(key[KEY_W]) { cam_x += cam_dir.x*10.0; cam_z += cam_dir.y*10.0; }
     if(key[KEY_S]) { cam_x -= cam_dir.x*10.0; cam_z -= cam_dir.y*10.0; }
     if(key[KEY_A]) { cam_x += cam_dir.y*10.0; cam_z += -cam_dir.x*10.0; }
     if(key[KEY_D]) { cam_x -= cam_dir.y*10.0; cam_z -= -cam_dir.x*10.0; }
     
     if(key[KEY_LEFT]) { cam_ay -= 0.09; }
     if(key[KEY_RIGHT]) { cam_ay += 0.09; }
     if(key[KEY_PGDN]) { cam_ax -= 0.09; }
     if(key[KEY_PGUP]) { cam_ax += 0.09; }
    }

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   transform(-cam_x, -cam_y, -cam_z, -cam_ax, cam_ay, 0.0);
   glBegin(GL_QUADS);

    glColor3ub(128, 128, 128); glVertex3f(-300.0, 0.0, 300.0);
     glVertex3f(300.0, 0.0, 300.0);
     glVertex3f(300.0, 0.0, -300.0);
     glVertex3f(-300.0, 0.0, -300.0);
   
   draw_map_3d(map, snum);
   glEnd();
   allegro_gl_flip();
  }

 free_map(map, snum);
 readkey();
 return 0;
}
END_OF_MAIN()

void init()
{
 allegro_init();
 install_allegro_gl();

 allegro_gl_set(AGL_Z_DEPTH, 16);
 allegro_gl_set(AGL_COLOR_DEPTH, 32);
 allegro_gl_set(AGL_SUGGEST, AGL_Z_DEPTH | AGL_COLOR_DEPTH);

 set_gfx_mode(GFX_OPENGL, 640, 480, 0, 0);

 install_keyboard();
 install_mouse();

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glFrustum(-1.0 * ASPECT_RATIO, 1.0 * ASPECT_RATIO, -1.0, 1.0, 2.0, 1000.0);
 glEnable(GL_DEPTH_TEST);
 disable_hardware_cursor();
 glDisable(GL_MULTISAMPLE_ARB);
}
