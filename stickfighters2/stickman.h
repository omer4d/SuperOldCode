#ifndef _STICKMAN_H_
#define _STICKMAN_H_

#include "vector.h"
#include "particle.h"

#define PARTICLE_RADIUS 0.0125
#define PARTICLE_DIAMETER (PARTICLE_RADIUS * 2.0)
#define HEAD_RADIUS (PARTICLE_RADIUS * 2.2)
#define PARTICLE_RADIUS_4 (PARTICLE_RADIUS * 4.0)
#define PARTICLE_RADIUS_6 (PARTICLE_RADIUS * 6.0)
#define PARTICLE_RADIUS_8 (PARTICLE_RADIUS * 8.0)
#define PARTICLE_RADIUS_9 (PARTICLE_RADIUS * 9.0)
#define PARTICLE_RADIUS_12 (PARTICLE_RADIUS * 12.0)
#define NECK_TO_HEAD ((PARTICLE_RADIUS + HEAD_RADIUS) * 0.5)

#define ARM_ANGLE 20.0
#define LEG_ANGLE 65.0

#define RELAXATION_LOOP_NUM 20

#define FLEXIBILITY 0.01
#define FLEX2 0.1

typedef struct {
 PARTICLE body[26];
 float health;
}STICKMAN;

VECTOR default_pose[26], temp_stickman[26];
float elbow_con1_rest, elbow_con2_rest, knee_con1_rest, knee_con2_rest;

void create_default_pose()
{
 default_pose[25] = ZERO_VECTOR;
 float k = PARTICLE_DIAMETER, ca = cos(DEG(ARM_ANGLE)) * k,
                              sa = sin(DEG(ARM_ANGLE)) * k,
                              cl = cos(DEG(LEG_ANGLE)) * k,
                              sl = sin(DEG(LEG_ANGLE)) * k;

 default_pose[24] = vector(default_pose[25].x, default_pose[25].y + NECK_TO_HEAD);
 default_pose[15] = vector(default_pose[24].x, default_pose[24].y + k);
 default_pose[16] = vector(default_pose[15].x + ca, default_pose[15].y + sa);
 default_pose[17] = vector(default_pose[16].x + ca, default_pose[16].y + sa);
 default_pose[18] = vector(default_pose[17].x + ca, default_pose[17].y + sa);
 default_pose[19] = vector(default_pose[18].x + ca, default_pose[18].y + sa);
 default_pose[20] = vector(default_pose[15].x - ca, default_pose[15].y + sa);
 default_pose[21] = vector(default_pose[20].x - ca, default_pose[20].y + sa);
 default_pose[22] = vector(default_pose[21].x - ca, default_pose[21].y + sa);
 default_pose[23] = vector(default_pose[22].x - ca, default_pose[22].y + sa);
 default_pose[14] = vector(default_pose[15].x, default_pose[15].y + k);
 default_pose[13] = vector(default_pose[14].x, default_pose[14].y + k);
 default_pose[6] = vector(default_pose[13].x, default_pose[13].y + k);
 default_pose[5] = vector(default_pose[6].x + cl, default_pose[6].y + sl);
 default_pose[4] = vector(default_pose[5].x + cl, default_pose[5].y + sl);
 default_pose[3] = vector(default_pose[4].x + cl, default_pose[4].y + sl);
 default_pose[2] = vector(default_pose[3].x + cl, default_pose[3].y + sl);
 default_pose[1] = vector(default_pose[2].x + cl, default_pose[2].y + sl);
 default_pose[0] = vector(default_pose[1].x + cl, default_pose[1].y + sl);
 default_pose[7] = vector(default_pose[6].x - cl, default_pose[6].y + sl);
 default_pose[8] = vector(default_pose[7].x - cl, default_pose[7].y + sl);
 default_pose[9] = vector(default_pose[8].x - cl, default_pose[8].y + sl);
 default_pose[10] = vector(default_pose[9].x - cl, default_pose[9].y + sl);
 default_pose[11] = vector(default_pose[10].x - cl, default_pose[10].y + sl);
 default_pose[12] = vector(default_pose[11].x - cl, default_pose[11].y + sl);

 elbow_con1_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[17], default_pose[21]));
 elbow_con2_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[17], default_pose[6]));
 knee_con1_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[3], default_pose[9]));
 knee_con2_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[3], default_pose[15]));
}

void create_stickman(STICKMAN *stickman, VECTOR pos)
{
 int i;

 for(i = 0; i < 26; i++)
  stickman->body[i] = create_particle(1.0, vector(default_pose[i].x + pos.x,
                                                  default_pose[i].y + pos.y));

 stickman->body[25].inverse_mass = 1.0 / 10.0;
 stickman->body[0].inverse_mass = 1.0 / 2.0;
 stickman->body[12].inverse_mass = 1.0 / 2.0;
 stickman->body[23].inverse_mass = 1.0 / 2.0;
 stickman->body[19].inverse_mass = 1.0 / 2.0;

 stickman->health = 1.0;
}

void form_stickman(PARTICLE p[26])
{
 int i, connection[48] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
                          6, 6, 7, 7, 8, 11, 12, 8, 9, 9, 10,
                          10, 11, 6, 13, 13, 14, 14, 15, 15, 16,
                          16, 17, 17, 18, 18, 19, 15, 20, 20, 21,
                          21, 22, 22, 23, 15, 24};

 for(i = 0; i < 47; i += 2)
  connect_particles(&p[connection[i]], &p[connection[i + 1]], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[24], &p[25], NECK_TO_HEAD, 1.0);

 connect_particles(&p[0], &p[3], PARTICLE_RADIUS_6, FLEX2);
 connect_particles(&p[9], &p[12], PARTICLE_RADIUS_6, FLEX2);
 connect_particles(&p[3], &p[6], PARTICLE_RADIUS_6, FLEX2);
 connect_particles(&p[9], &p[6], PARTICLE_RADIUS_6, FLEX2);
 connect_particles(&p[6], &p[25], PARTICLE_RADIUS_8 + NECK_TO_HEAD, FLEX2);
 connect_particles(&p[15], &p[17], PARTICLE_RADIUS_4, FLEX2);
 connect_particles(&p[17], &p[19], PARTICLE_RADIUS_4, FLEX2);
 connect_particles(&p[15], &p[21], PARTICLE_RADIUS_4, FLEX2);
 connect_particles(&p[21], &p[23], PARTICLE_RADIUS_4, FLEX2);

 connect_particles(&p[9], &p[3], knee_con1_rest, FLEXIBILITY);
 connect_particles(&p[3], &p[15], knee_con2_rest, FLEXIBILITY);
 connect_particles(&p[9], &p[15], knee_con2_rest, FLEXIBILITY);
 connect_particles(&p[21], &p[17], elbow_con1_rest, FLEXIBILITY);
 connect_particles(&p[21], &p[6], elbow_con2_rest, FLEXIBILITY);
 connect_particles(&p[17], &p[6], elbow_con2_rest, FLEXIBILITY);

 connect_particles(&p[6], &p[0], PARTICLE_RADIUS_12, FLEXIBILITY);
 connect_particles(&p[6], &p[12], PARTICLE_RADIUS_12, FLEXIBILITY);
 connect_particles(&p[15], &p[19], PARTICLE_RADIUS_8, FLEXIBILITY);
 connect_particles(&p[15], &p[23], PARTICLE_RADIUS_8, FLEXIBILITY);
}

void handle_stickman(STICKMAN *p)
{
 int i;

 for(i = 0; i < 26; i++)
  {
   simulate_particle(&p->body[i]);
   temp_stickman[i] = p->body[i].pos;
  }

 if(p->health > 0.001)
  for(i = 0; i < RELAXATION_LOOP_NUM; i++)
   form_stickman(p->body);

 for(i = 0; i < 26; i++)
  p->body[i].vel = VECTOR_SUM(p->body[i].vel,
  vel_from_pos_diff(VECTOR_DIFF(p->body[i].pos, temp_stickman[i]), p->body[i].old_accel));

 for(i = 0; i < 25; i++)
  particle_border_collision(&p->body[i], PARTICLE_RADIUS, 0.1);
 particle_border_collision(&p->body[i], HEAD_RADIUS, 0.1);
}

void draw_stickman(STICKMAN stickman)
{
 int i, c, connection[48] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
                             6, 6, 7, 7, 8, 11, 12, 8, 9, 9, 10,
                             10, 11, 6, 13, 13, 14, 14, 15, 15, 16,
                             16, 17, 17, 18, 18, 19, 15, 20, 20, 21,
                             21, 22, 22, 23, 15, 24};

 GFX_POINT_RADIUS = HEAD_RADIUS;
 vector_point(stickman.body[25].pos);

 if(stickman.health > 0.001)
  {
   GFX_SEGMENT_WIDTH = PARTICLE_DIAMETER;
   GFX_CAPPED_SEGMENT = 1;
   for(i = 0; i < 47; i += 2)
    vector_segment(stickman.body[connection[i]].pos, stickman.body[connection[i + 1]].pos);

   GFX_CAPPED_SEGMENT = 0;
   c = GFX_COLOR;
   GFX_COLOR = blend(c, RED, stickman.health);
   vector_segment(stickman.body[17].pos, stickman.body[18].pos);
   vector_segment(stickman.body[21].pos, stickman.body[22].pos);
  }

 else
  {
   GFX_POINT_RADIUS = PARTICLE_RADIUS;
   for(i = 0; i < 25; i++)
    vector_point(stickman.body[i].pos);
  }
}

#endif
