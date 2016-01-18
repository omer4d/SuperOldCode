#include <allegro.h>
#include "sgl.h"
#include "3ds.h"

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
   allegro_message("%s.", allegro_error);
   exit(1);
  }

 install_keyboard();
 //install_mouse();
 buffer = create_bitmap(SCREEN_W, SCREEN_H);

 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_FUNCTION(update_fps);
}

void point_cube()
{
 sglBegin(SGL_POINTS);
  sglVertex3f(-100.0, -100.0, -100.0);
  sglVertex3f(100.0, -100.0, -100.0);
  sglVertex3f(100.0, 100.0, -100.0);
  sglVertex3f(-100.0, 100.0, -100.0);
  sglVertex3f(-100.0, -100.0, 100.0);
  sglVertex3f(100.0, -100.0, 100.0);
  sglVertex3f(100.0, 100.0, 100.0);
  sglVertex3f(-100.0, 100.0, 100.0);
 sglEnd();
}

void wire_cube()
{
 sglBegin(SGL_LINES);
  sglVertex3f(-100.0, -100.0, -100.0); sglVertex3f(100.0, -100.0, -100.0);
  sglVertex3f(100.0, -100.0, -100.0); sglVertex3f(100.0, 100.0, -100.0);
  sglVertex3f(100.0, 100.0, -100.0); sglVertex3f(-100.0, 100.0, -100.0);
  sglVertex3f(-100.0, 100.0, -100.0); sglVertex3f(-100.0, -100.0, -100.0);

  sglVertex3f(-100.0, -100.0, 100.0); sglVertex3f(100.0, -100.0, 100.0);
  sglVertex3f(100.0, -100.0, 100.0); sglVertex3f(100.0, 100.0, 100.0);
  sglVertex3f(100.0, 100.0, 100.0); sglVertex3f(-100.0, 100.0, 100.0);
  sglVertex3f(-100.0, 100.0, 100.0); sglVertex3f(-100.0, -100.0, 100.0);

  sglVertex3f(-100.0, -100.0, -100.0); sglVertex3f(-100.0, -100.0, 100.0);
  sglVertex3f(100.0, -100.0, -100.0); sglVertex3f(100.0, -100.0, 100.0);
  sglVertex3f(100.0, 100.0, -100.0); sglVertex3f(100.0, 100.0, 100.0);
  sglVertex3f(-100.0, 100.0, -100.0); sglVertex3f(-100.0, 100.0, 100.0);
 sglEnd();
}

int main()
{
 int exit = 0;
 float a = 0.0;

 MODEL_3DS model_3ds;
 load_3ds(&model_3ds, "spaceship.3ds", 400.0);

 init();

 int i;
 int model = sglAllocModel(model_3ds.vnum, model_3ds.tnum);
 sglBindModel(model);

 for(i = 0; i < model_3ds.vnum; i++)
  sglUploadModelVertex3f(i, model_3ds.x[i], model_3ds.y[i], model_3ds.z[i]);
 for(i = 0; i < model_3ds.tnum; i++)
  sglUploadModelTri3i(i, model_3ds.a[i], model_3ds.b[i], model_3ds.c[i]);

 destroy_3ds(&model_3ds);

 install_int(update_fps, 1000);

 while(!exit)
  {
   if(keypressed())
    {
     if(key[KEY_ESC]) { exit = 1; }
    }

   a += 0.05;

   sglMatrixMode(SGL_MODELVIEW);

   sglLoadIdentity();
   sglRotatef(0.0, 0.0, a);
   sglTranslatef(300.0, 100.0, 400.0);
   point_cube();

   sglLoadIdentity();
   sglRotatef(a, 0.0, 0.0);
   sglTranslatef(-300.0, 100.0, 400.0);
   wire_cube();

   sglLoadIdentity();
   sglRotatef(0.0, a, 0.0);
   sglTranslatef(0.0, -200.0, 400.0);

   clear_to_color(buffer, 0);
   sglRenderPoints();
   sglRenderLines();
   sglRenderModel(SGL_POINTS);

   textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "FPS: %d", fps);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   frame_count++;
  }

 model = sglFreeModel(model);
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()
