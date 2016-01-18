#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "vector.h"

#define BORDER_MINX (1)
#define BORDER_MINY (1)
#define BORDER_MAXX (SCREEN_W - 1)
#define BORDER_MAXY (SCREEN_H - 1)

float GLOBAL_TIMESCALE;

typedef struct {
 float inverse_mass, radius;
 VECTOR force, old_pos, pos;
}PARTICLE;

void position_particle(PARTICLE *p, VECTOR pos)
{
 p->pos = p->old_pos = pos;
}

PARTICLE create_particle(float mass, float radius, VECTOR pos)
{
 PARTICLE p;

 p.radius = radius;
 p.inverse_mass = 1.0 / (mass + 0.0001);
 p.force = ZERO_VECTOR;
 position_particle(&p, pos);

 return p;
}

void simulate_particle(PARTICLE *p)
{
 float k = GLOBAL_TIMESCALE * GLOBAL_TIMESCALE * p->inverse_mass;
 VECTOR accel = USCALE_VECTOR(p->force, k), temp = p->pos;

 p->pos = VECTOR_SUM(p->pos, USCALE_VECTOR(vector(p->pos.x - p->old_pos.x + accel.x,
                                           p->pos.y - p->old_pos.y + accel.y), 0.999));

 p->old_pos = temp;
 p->force = ZERO_VECTOR;
}

void connect_equal_mass_particles(PARTICLE *p1, PARTICLE *p2, float rest_length, float k)
{
 VECTOR delta = VECTOR_DIFF(p1->pos, p2->pos);
 float delta_length = VECTOR_LENGTH(delta), diff = (delta_length - rest_length) / delta_length;

 delta = USCALE_VECTOR(delta, 0.5 * k * diff);

 p1->pos = VECTOR_DIFF(p1->pos, delta);
 p2->pos = VECTOR_SUM(p2->pos, delta);
}

VECTOR particle_velocity(PARTICLE p)
{
 return VECTOR_DIFF(p.pos, p.old_pos);
}

void set_particle_velocity(PARTICLE *p, VECTOR v)
{
 p->old_pos = VECTOR_DIFF(p->pos, v);
}

void circle_collision_response(PARTICLE *p1, PARTICLE *p2, VECTOR v1, VECTOR v2)
{
 VECTOR  n = VECTOR_DIFF(p1->pos, p2->pos);
 VECTOR vt1, vt2;
 float a1, a2, p;

 n = USCALE_VECTOR(n, 1.0 / VECTOR_LENGTH(n));
 a1 = VECTOR_DOT_PRODUCT(n, v1);
 a2 = VECTOR_DOT_PRODUCT(n, v2);
 p = (2.0 * (a1 - a2)) / ((1.0 / p1->inverse_mass) + (1.0 / p2->inverse_mass));

 vt1 = VECTOR_DIFF(v1, USCALE_VECTOR(n, p * (1.0 / p2->inverse_mass)));
 vt2 = VECTOR_SUM(v2, USCALE_VECTOR(n, p * (1.0 / p1->inverse_mass)));

 set_particle_velocity(p1, vt1);
 set_particle_velocity(p2, vt2);
}

int border_collision(PARTICLE *p, float e)
{
 VECTOR v;
 int flag = 0;

 if(p->pos.x < BORDER_MINX + p->radius)
 {
  v = particle_velocity(*p);
  p->pos.x = BORDER_MINX + p->radius;
  set_particle_velocity(p, vector(-e * v.x, v.y));
  flag = 1;
 }

 if(p->pos.x > BORDER_MAXX - p->radius)
 {
  v = particle_velocity(*p);
  p->pos.x = BORDER_MAXX - p->radius;
  set_particle_velocity(p, vector(-e * v.x, v.y));
  flag = 1;
 }

 if(p->pos.y < BORDER_MINY + p->radius)
 {
  v = particle_velocity(*p);
  p->pos.y = BORDER_MINY + p->radius;
  set_particle_velocity(p, vector(v.x, -e * v.y));
  flag = 1;
 }

 if(p->pos.y > BORDER_MAXY - p->radius)
 {
  v = particle_velocity(*p);
  p->pos.y = BORDER_MAXY - p->radius;
  set_particle_velocity(p, vector(v.x, -e * v.y));
  flag = 1;
 }

 return flag;
}

#endif
