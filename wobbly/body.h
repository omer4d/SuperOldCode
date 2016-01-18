#ifndef _BODY_H_
#define _BODY_H_

#include <allegro.h>
#include "particle.h"
#include "graphics.h"

#define EMPTY_COL (makecol(0, 255, 0))
#define SHAPE_COL (makecol(255, 0, 0))
#define SKIP_COL (makecol(128, 0, 0))
#define VERTEX_COL (makecol(0, 0, 255))

typedef struct {
 int c0, c1, type;
 float len, k;
}CONSTRAINT;

typedef struct {
 int pnum, cnum, col;
 PARTICLE *par;
 CONSTRAINT *con;
}BODY;

void handle_collision(PARTICLE *p1, PARTICLE *p2)
{
 VECTOR d = VECTOR_DIFF(p1->pos, p2->pos), v1, v2;
 float ds = approx_vector_length(d), p, k;

 if(ds < p1->radius + p2->radius)
  {
   k = 1.0 - (p1->inverse_mass / (p1->inverse_mass + p2->inverse_mass));
   p = p1->radius + p2->radius - ds;
   d = USCALE_VECTOR(d, 1.0 / ds);
   d = USCALE_VECTOR(d, p);
   p1->pos = VECTOR_SUM(p1->pos,  USCALE_VECTOR(d, 1.0 - k));
   p2->pos = VECTOR_DIFF(p2->pos, USCALE_VECTOR(d, k));
  }
}

float max_radius(BITMAP *bmp, int x0, int y0)
{
 int i = 1, c, f1 = -1, f2 = -1;

 while(f1 < 0)
  {
   c = getpixel(bmp, x0 + i, y0);
   if(c == EMPTY_COL) { f1 = i; break; }
   c = getpixel(bmp, x0 - i, y0);
   if(c == EMPTY_COL) { f1 = i; break; }
   c = getpixel(bmp, x0, y0 + i);
   if(c == EMPTY_COL) { f1 = i; break; }
   c = getpixel(bmp, x0, y0 - i);
   if(c == EMPTY_COL) { f1 = i; break; }
   i++;
  }

 i = 1;

 while(f2 < 0)
  {
   c = getpixel(bmp, x0 + i, y0 + i);
   if(c == EMPTY_COL) { f2 = sqrt(i * i * 2); break; }
   c = getpixel(bmp, x0 - i, y0 + i);
   if(c == EMPTY_COL) { f2 = sqrt(i * i * 2); break; }
   c = getpixel(bmp, x0 + i, y0 - i);
   if(c == EMPTY_COL) { f2 = sqrt(i * i * 2); break; }
   c = getpixel(bmp, x0 - i, y0 - i);
   if(c == EMPTY_COL) { f2 = sqrt(i * i * 2); break; }
   i++;
  }

 if(f1 < f2) return f1;
 return f2;
}

BODY make_body(BITMAP *bmp, int lod, int color, VECTOR pos)
{
 int j, i;
 int x, y, r;
 int maxx, maxy, maxr = -1;
 int cnum = 0, vnum = 0;
 int vx[100], vy[100], vi[100], nb[100];
 int col;

 if(lod > 100) lod = 100;

 BODY body;
 body.par = (PARTICLE *)malloc(lod * sizeof(PARTICLE));
 body.con = (CONSTRAINT *)malloc(SQUARED(lod) * sizeof(CONSTRAINT));
 body.pnum = lod;
 body.col = color;

 for(i = 0; i < SQUARED(lod); i++)
  {
   body.con[i].type = -1;
   body.con[i].k = 0.1;
  }

 for(j = 0; j < lod; j++)
  {
   maxr = -1;
   for(y = 0; y < bmp->h; y++)
    for(x = 0; x < bmp->w; x++)
     {
      col = getpixel(bmp, x, y);
      if(col != EMPTY_COL && col != SKIP_COL)
       {
        if(col == VERTEX_COL && j == 0)
         {
          vx[vnum] = x;
          vy[vnum] = y;
          vnum++;
         }

        r = max_radius(bmp, x, y);
        if(r > maxr)
         {
          maxr = r;
          maxx = x;
          maxy = y;
         }
       }
     }

   body.par[j] = create_particle(10.0, maxr, vector(maxx, maxy));
   circlefill(bmp, maxx, maxy, maxr, SKIP_COL);
  }

 for(i = 0; i < lod; i++)
  nb[i] = 0;

 for(i = 0; i < lod; i++)
  for(j = i + 1; j < lod; j++)
   {
    VECTOR d = VECTOR_DIFF(body.par[i].pos, body.par[j].pos);
    int rs = body.par[i].radius + body.par[j].radius;

    if((d.x * d.x + d.y * d.y) < rs * rs)
     {
      nb[i]++;
      nb[j]++;
      body.con[cnum].c0 = i;
      body.con[cnum].c1 = j;
      body.con[cnum].type = 1;
      body.con[cnum].len = VECTOR_LENGTH(d);
      cnum++;
     }
   }

 for(j = 0; j < vnum; j++)
  {
   float min_d = 100000000.0;
   for(i = 0; i < lod; i++)
    {
     int dx = body.par[i].pos.x - vx[j];
     int dy = body.par[i].pos.y - vy[j];

     if(dx * dx + dy * dy < min_d)
      {
       min_d = dx * dx + dy * dy;
       vi[j] = i;
      }
    }
  }

 for(i = 0; i < vnum; i++)
  for(j = i + 1; j < vnum; j++)
   {
    VECTOR d = VECTOR_DIFF(body.par[vi[i]].pos, body.par[vi[j]].pos);
    body.con[cnum].c0 = vi[i];
    body.con[cnum].c1 = vi[j];
    body.con[cnum].type = 2;
    body.con[cnum].len = VECTOR_LENGTH(d);
    nb[vi[i]]++;
    nb[vi[j]]++;
    cnum++;
   }

 x = 100000;
 y = 100000;

 for(i = 0; i < lod; i++)
  {
   if(body.par[i].pos.x < x) x = body.par[i].pos.x;
   if(body.par[i].pos.y < y) y = body.par[i].pos.y;
  }

 for(i = 0; i < lod; i++)
  position_particle(&body.par[i], vector(body.par[i].pos.x + pos.x - x,
                                         body.par[i].pos.y + pos.y - y));

 body.con = (CONSTRAINT *)realloc(body.con, (cnum + 1) * sizeof(CONSTRAINT));
 body.cnum = cnum;
 return body;
}

void simulate_body(BODY *body)
{
 int i;

 for(i = 0; i < body->pnum; i++)
  simulate_particle(&body->par[i]);
}

void body_border_collision(BODY *body, float e)
{
 int i;

 for(i = 0; i < body->pnum; i++)
  border_collision(&body->par[i], e);
}

void constrain_body(BODY *body)
{
 int i;

 for(i = 0; i < body->cnum; i++)
  connect_equal_mass_particles(&body->par[body->con[i].c0], &body->par[body->con[i].c1],
                               body->con[i].len, body->con[i].k);
}

void handle_body_collision(BODY *a, BODY *b)
{
 int i, j;

 for(i = 0; i < a->pnum; i++)
  for(j = 0; j < b->pnum; j++)
   handle_collision(&a->par[i], &b->par[j]);
}

/*
void draw_body(BITMAP *buffer, BODY *body, int col)
{
 int j;

 for(j = 0; j < body->pnum; j++)
  circle(buffer, body->par[j].pos.x, body->par[j].pos.y, body->par[j].radius, col);
}
*/

void draw_body(BITMAP *buffer, BODY *body)
{
 int i;

 for(i = 0; i < body->cnum; i++)
  if(body->con[i].type == 1)
   connected_circles(buffer, body->par[body->con[i].c0].pos,
                             body->par[body->con[i].c1].pos,
                             body->par[body->con[i].c0].radius,
                             body->par[body->con[i].c1].radius, body->col);
}

void destroy_body(BODY *body)
{
 free(body->par);
 free(body->con);
}

#endif
