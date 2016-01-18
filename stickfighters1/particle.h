#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "vector.h"

#define BORDER_MINX 1
#define BORDER_MINY 1
#define BORDER_MAXX SCREEN_W - 1
#define BORDER_MAXY SCREEN_H - 1

float LAST_TIMESCALE;
float GLOBAL_TIMESCALE;
float GLOBAL_DAMPING;
float OLD_TIMESCALE;

typedef struct {
 float inverse_mass;
 VECTOR force, old_pos, pos;
}PARTICLE;

void position_particle(PARTICLE *p, VECTOR pos)
{
 p->pos = p->old_pos = pos;
}

PARTICLE create_particle(float mass, VECTOR pos)
{
 PARTICLE p;
 p.inverse_mass = 1.0 / (mass + 0.0001);
 p.force = ZERO_VECTOR;
 position_particle(&p, pos);

 return p;
}

/*
void simulate_particle(PARTICLE *p)
{
 float k = GLOBAL_TIMESCALE * GLOBAL_TIMESCALE * p->inverse_mass;
 VECTOR accel = USCALE_VECTOR(p->force, k), temp = p->pos;

 p->pos = VECTOR_SUM(p->pos, USCALE_VECTOR(vector(p->pos.x - p->old_pos.x + accel.x,
                                           p->pos.y - p->old_pos.y + accel.y), GLOBAL_DAMPING));

 p->old_pos = temp;
 p->force = ZERO_VECTOR;
}
*/

void simulate_particle(PARTICLE *p)
{
 VECTOR temp = p->pos;
 VECTOR v = USCALE_VECTOR(VECTOR_DIFF(p->pos, p->old_pos), 1.0);
 VECTOR a = USCALE_VECTOR(p->force, p->inverse_mass);
 p->pos.x += v.x + a.x * GLOBAL_TIMESCALE * GLOBAL_TIMESCALE;
 p->pos.y += v.y + a.y * GLOBAL_TIMESCALE * GLOBAL_TIMESCALE;
 p->old_pos = temp;
 p->force = ZERO_VECTOR;
}

void connect_particles(PARTICLE *a, PARTICLE *b, float rest_length, float k)
{
 VECTOR delta = VECTOR_DIFF(b->pos, a->pos);
 float delta_length = sqrt(VECTOR_DOT_PRODUCT(delta, delta)),
       diff = (delta_length - rest_length) / (delta_length * (a->inverse_mass + b->inverse_mass));

 delta = USCALE_VECTOR(delta, diff * k);
 a->pos.x += a->inverse_mass * delta.x;
 a->pos.y += a->inverse_mass * delta.y;
 b->pos.x -= b->inverse_mass * delta.x;
 b->pos.y -= b->inverse_mass * delta.y;
}

VECTOR particle_velocity(PARTICLE p)
{
 return VECTOR_DIFF(p.pos, p.old_pos);
}

void set_particle_velocity(PARTICLE *p, VECTOR v)
{
 p->old_pos = VECTOR_DIFF(p->pos, v);
}

int border_collision(PARTICLE *p, float radius, float e)
{
 VECTOR v;
 int flag = 0;

 if(p->pos.x < BORDER_MINX + radius)
 {
  v = particle_velocity(*p);
  p->pos.x = BORDER_MINX + radius;
  set_particle_velocity(p, vector(-e * v.x, v.y));
  flag = 1;
 }

 if(p->pos.x > BORDER_MAXX - radius)
 {
  v = particle_velocity(*p);
  p->pos.x = BORDER_MAXX - radius;
  set_particle_velocity(p, vector(-e * v.x, v.y));
  flag = 1;
 }

 if(p->pos.y < BORDER_MINY + radius)
 {
  v = particle_velocity(*p);
  p->pos.y = BORDER_MINY + radius;
  set_particle_velocity(p, vector(v.x, -e * v.y));
  flag = 1;
 }

 if(p->pos.y > BORDER_MAXY - radius)
 {
  v = particle_velocity(*p);
  p->pos.y = BORDER_MAXY - radius;
  set_particle_velocity(p, vector(v.x, -e * v.y));
  flag = 1;
 }

 return flag;
}

#endif
