#ifndef _STICKMAN_H_
#define _STICKMAN_H_

#include "particle.h"
#include "graphics.h"

#define PARTICLE_RADIUS 4.0
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

#define FLEXIBILITY 0.02

VECTOR default_pose[26];
float elbow_con1_rest, elbow_con2_rest, knee_con1_rest, knee_con2_rest;

void create_default_pose()
{
 default_pose[25] = ZERO_VECTOR;
 float ca = cos(DEG(ARM_ANGLE)), sa = sin(DEG(ARM_ANGLE)), cl = cos(DEG(LEG_ANGLE)), sl = sin(DEG(LEG_ANGLE)),
       k = PARTICLE_DIAMETER;

 default_pose[24] = vector(default_pose[25].x, default_pose[25].y + NECK_TO_HEAD);
 default_pose[15] = vector(default_pose[24].x, default_pose[24].y + k);
 default_pose[16] = vector(default_pose[15].x + k * ca, default_pose[15].y + k * sa);
 default_pose[17] = vector(default_pose[16].x + k * ca, default_pose[16].y + k * sa);
 default_pose[18] = vector(default_pose[17].x + k * ca, default_pose[17].y + k * sa);
 default_pose[19] = vector(default_pose[18].x + k * ca, default_pose[18].y + k * sa);
 default_pose[20] = vector(default_pose[15].x - k * ca, default_pose[15].y + k * sa);
 default_pose[21] = vector(default_pose[20].x - k * ca, default_pose[20].y + k * sa);
 default_pose[22] = vector(default_pose[21].x - k * ca, default_pose[21].y + k * sa);
 default_pose[23] = vector(default_pose[22].x - k * ca, default_pose[22].y + k * sa);
 default_pose[14] = vector(default_pose[15].x, default_pose[15].y + k);
 default_pose[13] = vector(default_pose[14].x, default_pose[14].y + k);
 default_pose[6] = vector(default_pose[13].x, default_pose[13].y + k);
 default_pose[5] = vector(default_pose[6].x + k * cl, default_pose[6].y + k * sl);
 default_pose[4] = vector(default_pose[5].x + k * cl, default_pose[5].y + k * sl);
 default_pose[3] = vector(default_pose[4].x + k * cl, default_pose[4].y + k * sl);
 default_pose[2] = vector(default_pose[3].x + k * cl, default_pose[3].y + k * sl);
 default_pose[1] = vector(default_pose[2].x + k * cl, default_pose[2].y + k * sl);
 default_pose[0] = vector(default_pose[1].x + k * cl, default_pose[1].y + k * sl);
 default_pose[7] = vector(default_pose[6].x - k * cl, default_pose[6].y + k * sl);
 default_pose[8] = vector(default_pose[7].x - k * cl, default_pose[7].y + k * sl);
 default_pose[9] = vector(default_pose[8].x - k * cl, default_pose[8].y + k * sl);
 default_pose[10] = vector(default_pose[9].x - k * cl, default_pose[9].y + k * sl);
 default_pose[11] = vector(default_pose[10].x - k * cl, default_pose[10].y + k * sl);
 default_pose[12] = vector(default_pose[11].x - k * cl, default_pose[11].y + k * sl);

 elbow_con1_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[17], default_pose[21]));
 elbow_con2_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[17], default_pose[6]));
 knee_con1_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[3], default_pose[9]));
 knee_con2_rest = VECTOR_LENGTH(VECTOR_DIFF(default_pose[3], default_pose[15]));
}

void form_stickman(PARTICLE p[26])
{
 connect_particles(&p[0], &p[1], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[1], &p[2], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[2], &p[3], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[3], &p[4], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[4], &p[5], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[5], &p[6], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[6], &p[7], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[7], &p[8], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[8], &p[9], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[9], &p[10], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[10], &p[11], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[11], &p[12], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[6], &p[13], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[13], &p[14], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[14], &p[15], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[15], &p[16], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[16], &p[17], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[17], &p[18], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[18], &p[19], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[15], &p[20], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[20], &p[21], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[21], &p[22], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[22], &p[23], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[15], &p[24], PARTICLE_DIAMETER, 1.0);
 connect_particles(&p[24], &p[25], NECK_TO_HEAD, 1.0);

 connect_particles(&p[0], &p[3], PARTICLE_RADIUS_6, 1.0);
 connect_particles(&p[9], &p[12], PARTICLE_RADIUS_6, 1.0);
 connect_particles(&p[3], &p[6], PARTICLE_RADIUS_6, 1.0);
 connect_particles(&p[9], &p[6], PARTICLE_RADIUS_6, 1.0);
 connect_particles(&p[6], &p[25], PARTICLE_RADIUS_8 + NECK_TO_HEAD, 1.0);
 connect_particles(&p[15], &p[17], PARTICLE_RADIUS_4, 1.0);
 connect_particles(&p[17], &p[19], PARTICLE_RADIUS_4, 1.0);
 connect_particles(&p[15], &p[21], PARTICLE_RADIUS_4, 1.0);
 connect_particles(&p[21], &p[23], PARTICLE_RADIUS_4, 1.0);

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

void draw_stickman(BITMAP *buffer, PARTICLE p[26], int color, int wristband_color)
{
 int outline = 3;
     
 vector_point(buffer, p[25].pos, HEAD_RADIUS + outline, 0);
 vector_segment(buffer, p[0].pos, p[1].pos, PARTICLE_RADIUS + outline, 0, 1);
 vector_segment(buffer, p[1].pos, p[2].pos, PARTICLE_RADIUS + outline, 0, 0);
 vector_segment(buffer, p[2].pos, p[3].pos, PARTICLE_RADIUS + outline, 0, 0);
 vector_segment(buffer, p[3].pos, p[4].pos, PARTICLE_RADIUS + outline, 0, 0);
 vector_segment(buffer, p[4].pos, p[5].pos, PARTICLE_RADIUS + outline, 0, 0);
 vector_segment(buffer, p[5].pos, p[6].pos, PARTICLE_RADIUS + outline, 0, 0);
 vector_segment(buffer, p[6].pos, p[7].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[7].pos, p[8].pos, PARTICLE_RADIUS+ outline, 0, 0);
  vector_segment(buffer, p[11].pos, p[12].pos, PARTICLE_RADIUS+ outline, 0, 1);
 vector_segment(buffer, p[8].pos, p[9].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[9].pos, p[10].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[10].pos, p[11].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[6].pos, p[13].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[13].pos, p[14].pos, PARTICLE_RADIUS+ outline,0, 0);
 vector_segment(buffer, p[14].pos, p[15].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[15].pos, p[16].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[16].pos, p[17].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[17].pos, p[18].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[18].pos, p[19].pos, PARTICLE_RADIUS+ outline, 0, 1);
 vector_segment(buffer, p[15].pos, p[20].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[20].pos, p[21].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[21].pos, p[22].pos, PARTICLE_RADIUS+ outline, 0, 0);
 vector_segment(buffer, p[22].pos, p[23].pos, PARTICLE_RADIUS+ outline, 0, 1);
 vector_segment(buffer, p[15].pos, p[24].pos, PARTICLE_RADIUS+ outline, 0, 0);
 
 vector_point(buffer, p[25].pos, HEAD_RADIUS, color);
 vector_segment(buffer, p[0].pos, p[1].pos, PARTICLE_RADIUS, color, 1);
 vector_segment(buffer, p[1].pos, p[2].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[2].pos, p[3].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[3].pos, p[4].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[4].pos, p[5].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[5].pos, p[6].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[6].pos, p[7].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[7].pos, p[8].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[11].pos, p[12].pos, PARTICLE_RADIUS, color, 1);
 vector_segment(buffer, p[8].pos, p[9].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[9].pos, p[10].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[10].pos, p[11].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[6].pos, p[13].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[13].pos, p[14].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[14].pos, p[15].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[15].pos, p[16].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[16].pos, p[17].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[17].pos, p[18].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[18].pos, p[19].pos, PARTICLE_RADIUS, color, 1);
 vector_segment(buffer, p[15].pos, p[20].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[20].pos, p[21].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[21].pos, p[22].pos, PARTICLE_RADIUS, color, 0);
 vector_segment(buffer, p[22].pos, p[23].pos, PARTICLE_RADIUS, color, 1);
 vector_segment(buffer, p[15].pos, p[24].pos, PARTICLE_RADIUS, color, 0);
}

void draw_stickman2(BITMAP *buffer, PARTICLE p[26], int color, int wristband_color)
{
 int outline = 0;
     
 vector_point(buffer, p[25].pos, HEAD_RADIUS + outline, color);
 circle(buffer, (int)p[25].pos.x, (int)p[25].pos.y, HEAD_RADIUS, 0);
 outlined_vector_segment(buffer, p[0].pos, p[1].pos, PARTICLE_RADIUS + outline, color, 1);
 outlined_vector_segment(buffer, p[1].pos, p[2].pos, PARTICLE_RADIUS + outline, color, 0);
 outlined_vector_segment(buffer, p[2].pos, p[3].pos, PARTICLE_RADIUS + outline, color, 0);
 outlined_vector_segment(buffer, p[3].pos, p[4].pos, PARTICLE_RADIUS + outline, color, 0);
 outlined_vector_segment(buffer, p[4].pos, p[5].pos, PARTICLE_RADIUS + outline, color, 0);
 outlined_vector_segment(buffer, p[5].pos, p[6].pos, PARTICLE_RADIUS + outline, color, 0);
 outlined_vector_segment(buffer, p[6].pos, p[7].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[7].pos, p[8].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[11].pos, p[12].pos, PARTICLE_RADIUS+ outline, color, 1);
 outlined_vector_segment(buffer, p[8].pos, p[9].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[9].pos, p[10].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[10].pos, p[11].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[6].pos, p[13].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[13].pos, p[14].pos, PARTICLE_RADIUS+ outline,color, 0);
 outlined_vector_segment(buffer, p[14].pos, p[15].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[15].pos, p[16].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[16].pos, p[17].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[17].pos, p[18].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[18].pos, p[19].pos, PARTICLE_RADIUS+ outline, color, 1);
 outlined_vector_segment(buffer, p[15].pos, p[20].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[20].pos, p[21].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[21].pos, p[22].pos, PARTICLE_RADIUS+ outline, color, 0);
 outlined_vector_segment(buffer, p[22].pos, p[23].pos, PARTICLE_RADIUS+ outline, color, 1);
 outlined_vector_segment(buffer, p[15].pos, p[24].pos, PARTICLE_RADIUS+ outline, color, 0);
}

#endif
