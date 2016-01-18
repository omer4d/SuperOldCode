#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "vector.h"

#define WORLD_SIZE 1.5
#define BORDER_MINX (-1.0 * WORLD_SIZE)
#define BORDER_MINY (-0.7 * WORLD_SIZE)
#define BORDER_MAXX (1.0 * WORLD_SIZE)
#define BORDER_MAXY (0.7 * WORLD_SIZE)

float TIMESCALE;

typedef struct {
 float inverse_mass;
 VECTOR force, old_accel, vel, pos;
}PARTICLE;

PARTICLE create_particle(float mass, VECTOR pos)
{
 PARTICLE p;

 p.inverse_mass = 1.0 / (mass + 0.001);
 p.force = p.old_accel = p.vel = ZERO_VECTOR;
 p.pos = pos;

 return p;
}

void control_particle(PARTICLE *p, float force, int up, int down, int left, int right)
{
 if(key[up]) { p->force.y -= force; }
 if(key[down]) { p->force.y += force; }
 if(key[left]) { p->force.x -= force; }
 if(key[right]) { p->force.x += force; }
}

void simulate_particle(PARTICLE *p)
{
 VECTOR accel = USCALE_VECTOR(p->force, p->inverse_mass);
 float TIMESTEP = 0.01 * TIMESCALE;

 p->pos.x += p->vel.x * TIMESTEP + 0.5 * p->old_accel.x * TIMESTEP * TIMESTEP;
 p->pos.y += p->vel.y * TIMESTEP + 0.5 * p->old_accel.y * TIMESTEP * TIMESTEP;
 p->vel.x += 0.5 * (p->old_accel.x + accel.x) * TIMESTEP;
 p->vel.y += 0.5 * (p->old_accel.y + accel.y) * TIMESTEP;
 p->old_accel = accel;
 p->force = ZERO_VECTOR;
}

VECTOR vel_from_pos_diff(VECTOR pos_diff, VECTOR old_accel)
{
 VECTOR v;
 float TIMESTEP = 0.01 * TIMESCALE;

 v.x = (pos_diff.x - 0.5 * old_accel.x * TIMESTEP * TIMESTEP) / TIMESTEP;
 v.y = (pos_diff.y - 0.5 * old_accel.y * TIMESTEP * TIMESTEP) / TIMESTEP;

 return v;
}

void connect_particles(PARTICLE *a, PARTICLE *b, float rest_length, float k)
{
 VECTOR delta = VECTOR_DIFF(b->pos, a->pos);
 float delta_length = sqrt(VECTOR_DOT_PRODUCT(delta, delta)),
       diff = (delta_length - rest_length) / (delta_length * (a->inverse_mass + b->inverse_mass));
 VECTOR t1 = a->pos, t2 = b->pos;

 if(TIMESCALE < 1.0) k *= TIMESCALE;

 delta = USCALE_VECTOR(delta, diff * k);
 a->pos.x += a->inverse_mass * delta.x;
 a->pos.y += a->inverse_mass * delta.y;
 b->pos.x -= b->inverse_mass * delta.x;
 b->pos.y -= b->inverse_mass * delta.y;
}

int particle_border_collision(PARTICLE *p, float radius, float e)
{
 int flag = 0;

 if(p->pos.x < BORDER_MINX + radius)
 {
  p->pos.x = BORDER_MINX + radius;
  p->vel = vector(-e * p->vel.x, p->vel.y);
  flag = 1;
 }

 if(p->pos.x > BORDER_MAXX - radius)
 {
  p->pos.x = BORDER_MAXX - radius;
  p->vel = vector(-e * p->vel.x, p->vel.y);
  flag = 1;
 }

 if(p->pos.y < BORDER_MINY + radius)
 {
  p->pos.y = BORDER_MINY + radius;
  p->vel = vector(p->vel.x, -e * p->vel.y);
  flag = 1;
 }

 if(p->pos.y > BORDER_MAXY - radius)
 {
  p->pos.y = BORDER_MAXY - radius;
  p->vel = vector(p->vel.x, -e * p->vel.y);
  flag = 1;
 }

 return flag;
}

#endif
