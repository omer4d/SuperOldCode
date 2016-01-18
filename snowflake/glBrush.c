#include <malloc.h>
#include <alleggl.h>
#include "vector.h"
#include <math.h>

#define ARM_NUMBER 6

void init()
{
 allegro_init();
 install_allegro_gl();

 allegro_gl_set(AGL_Z_DEPTH, 16);
 allegro_gl_set(AGL_COLOR_DEPTH, 32);
 allegro_gl_set(AGL_SUGGEST, AGL_Z_DEPTH | AGL_COLOR_DEPTH);

 set_gfx_mode(GFX_OPENGL_WINDOWED, 1024, 1024, 0, 0);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(0, SCREEN_W, SCREEN_H, 0, -10.0, 10.0);
 glEnable(GL_TEXTURE_2D);

 install_keyboard();
 install_mouse();
}

GLuint empty_texture(int w, int h)
{
 GLuint id;
 unsigned int *data = (unsigned int *)malloc(w * h * 4 * sizeof(unsigned int));

 glGenTextures(1, &id);
 glBindTexture(GL_TEXTURE_2D, id);
 glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 free(data);
 return id;
}

GLuint load_texture(char *path, int flags, int type)
{
 BITMAP *texture = load_bitmap(path, NULL);

 if(!texture)
  {
   allegro_message("Failed loading texture '%s'.", path);
   exit(1);
  }

 int id = allegro_gl_make_texture_ex(flags, texture, type);
 destroy_bitmap(texture);
 return id;
}

void my_rect(VECTOR c, float r)
{
 glTexCoord2f(0.0, 1.0); glVertex2f(c.x - r, c.y - r);
 glTexCoord2f(1.0, 1.0); glVertex2f(c.x + r, c.y - r);
 glTexCoord2f(1.0, 0.0); glVertex2f(c.x + r, c.y + r);
 glTexCoord2f(0.0, 0.0); glVertex2f(c.x - r, c.y + r);
}

int main()
{
 int exit = 0, i;
 GLuint fbo, canvas, brush;

 init();

 VECTOR arm[ARM_NUMBER], arm_normal[ARM_NUMBER], center = vector(SCREEN_W / 2, SCREEN_H / 2), p, p1, p2;

 for(i = 0; i < ARM_NUMBER; i++)
  {
   arm[i].x = cos(i * 2.0 * M_PI / ARM_NUMBER);
   arm[i].y = sin(i * 2.0 * M_PI / ARM_NUMBER);
   arm_normal[i] = VECTOR_NORMAL(arm[i]);
  }

 glGenFramebuffersEXT(1, &fbo);
 glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
 canvas = empty_texture(SCREEN_W, SCREEN_H);
 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, canvas, 0);

 brush = load_texture("brush.bmp", AGL_TEXTURE_MIPMAP, GL_RGB8);
 glBindTexture(GL_TEXTURE_2D, brush);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

 glEnable(GL_BLEND);
 glBlendFunc(GL_ONE, GL_ONE);
 //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 while(!exit)
  {
   if(key[KEY_ESC]) { exit = 1; } 

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   if(mouse_b == 1)
    {         
     glBindTexture(GL_TEXTURE_2D, brush);
     glColor3ub(rand() % 128, rand() % 128, rand() % 128);
     glBegin(GL_QUADS);
     p = vector(center.x - mouse_x, center.y - mouse_y);

     for(i = 0; i < ARM_NUMBER; i++)
      {
       p1 = VECTOR_SUM(USCALE_VECTOR(arm[i], p.x), USCALE_VECTOR(arm_normal[i], -p.y));
       p2 = VECTOR_SUM(USCALE_VECTOR(arm[i], p.x), USCALE_VECTOR(arm_normal[i], p.y));
       my_rect(vector(center.x + p1.x, center.y + p1.y), 5.0);
       my_rect(vector(center.x + p2.x, center.y + p2.y), 5.0);
      }

     glEnd();
    }

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glBindTexture(GL_TEXTURE_2D, canvas);
   glColor3f(1.0, 1.0, 1.0);
   glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex2f(0.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(SCREEN_W, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(SCREEN_W, SCREEN_H);
    glTexCoord2f(0.0, 0.0); glVertex2f(0.0, SCREEN_H);
   glEnd();
   glBindTexture(GL_TEXTURE_2D, brush);
   glColor3ub(255, 0, 0);
   glBegin(GL_QUADS);
    my_rect(vector(mouse_x, mouse_y), 10.0);
   glEnd();

   allegro_gl_flip();
  }

 return 0;
}
END_OF_MAIN()
