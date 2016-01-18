#include <stdio.h>
#include <allegro.h>
#include "vector3f.h"
#include "color.h"

class cRay {
 public:
  cVector3f orig, dest;
  cColor col;

  cVector3f at(float t);
};

class cObject {
 public:
  static int number;
  float r, g, b, diff, spec, refl, refr;

  void color(float, float, float);
  void material(float, float, float, float);
  virtual int intersectRay(cRay, float *) { return 0; }
  virtual cVector3f normal(cVector3f) { return vector3f(0.0f, 0.0f, 0.0f); }
  virtual float shade(cVector3f, cVector3f, cVector3f, float, float) { return 0.0f; }
};

int cObject::number = 0;

void cObject::color(float r1, float g1, float b1)
{
 r = r1;
 g = g1;
 b = b1;
}

void cObject::material(float a, float b, float c, float d)
{
 diff = a;
 spec = b;
 refl = c;
 refr = d;
}

class cSphere: public cObject {
 public:
  cVector3f center;
  float radius;

  cSphere() { number++; }
  ~cSphere() { number--; }
  int intersectRay(cRay, float *);
  cVector3f normal(cVector3f);
  float shade(cVector3f, cVector3f, cVector3f, float, float);
};

class cPlane: public cObject {
 public :
  cVector3f n;
  float d;

  cPlane() { number++; }
  ~cPlane() { number--; }
  int intersectRay(cRay, float *);
  cVector3f normal(cVector3f);
  float shade(cVector3f, cVector3f, cVector3f, float, float);
};

BITMAP *buffer;

int cPlane::intersectRay(cRay ray, float *t)
{
 float cosAlpha, deltaD;

 cosAlpha = DOT(ray.dest, n);
 if(cosAlpha == 0.0f) return 0;
 deltaD = d - DOT(ray.orig, n);
 *t = deltaD / cosAlpha;
 if(*t < 0.0) return 0;

 return 1;
}

cVector3f cPlane::normal(cVector3f p)
{
 n.normalize();
 return n;
}

cVector3f cRay::at(float t)
{
 return orig + dest * t;
}

int cSphere::intersectRay(cRay ray, float *t)
{
 cVector3f delta = ray.orig - center;

 float B = 2.0f * DOT(delta, ray.dest);
 float C = DOT(delta, delta) - SQUARED(radius);
 float D = SQUARED(B) - 4.0f * C;

 *t = -1.0;

 if(D < 0.0f)
  return 0;

 *t = (-B - sqrt(D)) * 0.5f;

 if(*t < 0.0f)
  return 0;

 return 1;
}

cVector3f cSphere::normal(cVector3f p)
{
 cVector3f n = center - p;
 n.normalize();
 return n;
}

cVector3f light = vector3f(200.0, -500.0, -300.0);

float cSphere::shade(cVector3f p, cVector3f n, cVector3f v, float diff, float spec)
{
 cVector3f l = p - light;
 l.normalize();
 cVector3f r = l - n * 2.0 * DOT(l, n);
 r.normalize();
 float k = DOT(l, n) * diff + powf(DOT(v, r), 20.0) * spec;

 if(k < 0.0) k = 0.0;
 return k;
}

float cPlane::shade(cVector3f p, cVector3f n, cVector3f v, float diff, float spec)
{
 cVector3f l = p - light;
 l.normalize();
 cVector3f r = l - n * 2.0 * DOT(l, n);
 r.normalize();
 float k = DOT(l, n) * diff + powf(DOT(v, r), 10.0) * spec;

 if(k < 0.0) k = 0.0;
 return 1.0 - k;
}

cObject *object[5];

int traceDepth = 0;

cColor raytrace(cRay ray)
{
 int i, j;
 float tMin = 1000000.0f, t, t2, t3, k;
 cVector3f ip, n;
 cColor rayColor = color(-1.0, -1.0, -1.0), reflRayColor = color(0.0, 0.0, 0.0);

 for(i = 0; i < cObject::number; i++)
  if(object[i]->intersectRay(ray, &t))
   if(t < tMin)
    {
     tMin = t;
     ip = ray.at(t);
     n = object[i]->normal(ip);
     k = object[i]->shade(ip, n, ray.dest, object[i]->diff, object[i]->spec);

     for(j = 0; j < cObject::number; j++)
      if(j != i)
       {
        cRay toLight;
        toLight.orig = light;
        toLight.dest = ip - toLight.orig;
        toLight.dest.normalize();
        t2 = (light - ip).length();

        if(object[j]->intersectRay(toLight, &t3) && t3 < t2)
         k = 0.0f;
       }

     if(traceDepth < 5)
      {
       traceDepth++;
       cVector3f r = ray.dest - n * 2.0f * DOT(ray.dest, n);
       cRay reflected;
       reflected.orig = ip + r;
       reflected.dest = r;
       reflRayColor = raytrace(reflected);

       if(reflRayColor.r < 0.0f)
        reflRayColor = color(0.0f, 0.0f, 0.0f);
      }

     rayColor = (color(object[i]->r, object[i]->g, object[i]->b) + reflRayColor * object[i]->refl) * k;
    }

 return rayColor;
}

void init();

int main()
{
 int x, y, tx, ty;
 cVector3f eye = vector3f(0.0f, 0.0f, -1000.0f), projectionPlane;
 cRay ray;
 cColor pixelColor;
 cSphere sphere[4];
 cPlane plane;

 sphere[0].center = vector3f(-10.0f, -150.0f, 1300.0f);
 sphere[0].radius = 50.0f;
 sphere[0].color(0.0f, 255.0f, 0.0f);
 sphere[0].material(0.25f, 0.5f, 2.0f, 0.0f);

 sphere[1].center = vector3f(0.0f, 0.0f, 1300.0f);
 sphere[1].radius = 100.0f;
 sphere[1].color(255.0f, 0.0f, 0.0f);
 sphere[1].material(0.25f, 0.5f, 2.0f, 0.0f);

 sphere[2].center = vector3f(100.0f, -150.0f, 1300.0f);
 sphere[2].radius = 50.0f;
 sphere[2].color(0.0f, 0.0f, 255.0f);
 sphere[2].material(0.25f, 0.5f, 2.0f, 0.0f);

 sphere[3].center = vector3f(100.0, -10.0, 1100.0);
 sphere[3].radius = 50.0;
 sphere[3].color(255.0f, 255.0f, 0.0f);
 sphere[3].material(0.25f, 0.5f, 2.0f, 0.0f);

 plane.n = vector3f(1.0f, -1.0f, 0.0f);
 plane.d = -100.0f;
 plane.color(64.0f, 64.0f, 64.0f);
 plane.material(0.0f, 0.5f, 0.5f, 0.0f);

 object[0] = &sphere[0];
 object[1] = &sphere[1];
 object[2] = &sphere[2];
 object[3] = &sphere[3];
 object[4] = &plane;

 init();

 clear_to_color(buffer, makecol(32, 32, 32));

 for(y = 0; y < buffer->h; y++)
  for(x = 0; x < buffer->w; x++)
   {
    /*
    pixelColor = color(0.0, 0.0, 0.0);

    for(ty = 0; ty < 4; ty++)
     for(tx = 0; tx < 4; tx++)
      {
       ray.orig = eye;
       projectionPlane = vector3f((float)(x - (buffer->w >> 1)) + (float)(tx * 0.25),
                                  (float)(y - (buffer->h >> 1)) + (float)(ty * 0.25), 0.0);
       ray.dest = projectionPlane - ray.orig;
       ray.dest.normalize();
       traceDepth = 0;
       cColor temp = raytrace(ray);

       if(temp.r >= 0.0f) pixelColor = pixelColor + temp;
       else pixelColor = pixelColor + color(32.0f, 32.0f, 32.0f);           
      }

    pixelColor = pixelColor * 0.0625;
    */

    ray.orig = eye;
    projectionPlane = vector3f((float)(x - (buffer->w >> 1)), (float)(y - (buffer->h >> 1)), 0.0);
    ray.dest = projectionPlane - ray.orig;
    ray.dest.normalize();
    traceDepth = 0;
    pixelColor = raytrace(ray);
    if(pixelColor.r >= 0.0f) putpixel(buffer, x, y, pixelColor.c());
   }

 stretch_blit(buffer, screen, 0, 0, buffer->w, buffer->h, 0, 0, SCREEN_W, SCREEN_H);

 readkey();
 destroy_bitmap(buffer);
 return 0;
}
END_OF_MAIN()

void init()
{
 allegro_init();
 install_keyboard();

 set_color_depth(32);
 if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0))
  {
   allegro_message("Error: %s.", allegro_error);
   exit(1);
  }

 buffer = create_bitmap(320, 240);
}
