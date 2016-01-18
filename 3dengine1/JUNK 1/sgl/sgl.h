#include "math3d.h"

BITMAP *buffer;

#define FOCAL_DISTANCE (512.0)
#define SGL_MODELVIEW (0)
#define SGL_POINTS (0)
#define SGL_LINES (1)

// STRUCTS:

struct VTX3F_NODE_tag {
 float x, y, z;
 struct VTX3F_NODE_tag *next;
};

struct VTX2F_NODE_tag {
 float x, y;
 struct VTX2F_NODE_tag *next;
};

typedef struct VTX3F_NODE_tag VTX3F_NODE;
typedef struct VTX2F_NODE_tag VTX2F_NODE;

// CODE:

MAT16F *curr_mat, modelview_mat;

VTX3F_NODE **curr_vtx3f_stack_base = NULL, **curr_vtx3f_stack = NULL;
VTX3F_NODE *point_stack_base = NULL, *point_stack_node = NULL;
VTX3F_NODE *line_stack_base = NULL, *line_stack_node = NULL;

VTX2F_NODE **curr_vtx2f_stack_base = NULL, **curr_vtx2f_stack = NULL;
VTX2F_NODE *scr_point_stack_base = NULL, *scr_point_stack_node = NULL;
VTX2F_NODE *scr_line_stack_base = NULL, *scr_line_stack_node = NULL;

void sglVertex3f(float x, float y, float z)
{
 *curr_vtx3f_stack = (VTX3F_NODE *)malloc(sizeof(VTX3F_NODE));
 (**curr_vtx3f_stack).x = x;
 (**curr_vtx3f_stack).y = y;
 (**curr_vtx3f_stack).z = z;
 (**curr_vtx3f_stack).next = *curr_vtx3f_stack_base;
 *curr_vtx3f_stack_base = *curr_vtx3f_stack;
}

void pop_vertex3f(float *x, float *y, float *z)
{
 if(*curr_vtx3f_stack != NULL)
  {
   *x = (**curr_vtx3f_stack).x;
   *y = (**curr_vtx3f_stack).y;
   *z = (**curr_vtx3f_stack).z;

   VTX3F_NODE *temp = (**curr_vtx3f_stack).next;
   free(*curr_vtx3f_stack);
   *curr_vtx3f_stack = temp;
  }
}

void sglVertex2f(float x, float y)
{
 *curr_vtx2f_stack = (VTX2F_NODE *)malloc(sizeof(VTX2F_NODE));
 (**curr_vtx2f_stack).x = x;
 (**curr_vtx2f_stack).y = y;
 (**curr_vtx2f_stack).next = *curr_vtx2f_stack_base;
 *curr_vtx2f_stack_base = *curr_vtx2f_stack;
}

void pop_vertex2f(float *x, float *y)
{
 if(*curr_vtx2f_stack != NULL)
  {
   *x = (**curr_vtx2f_stack).x;
   *y = (**curr_vtx2f_stack).y;

   VTX2F_NODE *temp = (**curr_vtx2f_stack).next;
   free(*curr_vtx2f_stack);
   *curr_vtx2f_stack = temp;
  }
}

void sglMatrixMode(int mode)
{
 if(mode == SGL_MODELVIEW)
  curr_mat = &modelview_mat;
}

void sglLoadIdentity()
{
 reset_mat16f(*curr_mat);
}

void sglRotatef(float rx, float ry, float rz)
{
 rotate_mat16f(*curr_mat, rx, ry, rz);
}

void sglTranslatef(float tx, float ty, float tz)
{
 translate_mat16f(*curr_mat, tx, ty, tz);
}

void transform_vtx3f()
{
 VEC3F source, temp;
 pop_vertex3f(&source[0], &source[1], &source[2]);

 temp[0] = source[0] * (*curr_mat)[0][0] +
           source[1] * (*curr_mat)[1][0] +
           source[2] * (*curr_mat)[2][0] +
                       (*curr_mat)[3][0];
 temp[1] = source[0] * (*curr_mat)[0][1] +
           source[1] * (*curr_mat)[1][1] +
           source[2] * (*curr_mat)[2][1] +
                       (*curr_mat)[3][1];
 temp[2] = source[0] * (*curr_mat)[0][2] +
           source[1] * (*curr_mat)[1][2] +
           source[2] * (*curr_mat)[2][2] +
                       (*curr_mat)[3][2];

 float s = FOCAL_DISTANCE / (temp[2] + 512.0);
 sglVertex2f(320.0 + temp[0] * s, 240.0 + temp[1] * s);
}

void sglBegin(int mode)
{
 if(mode == SGL_POINTS)
  {
   curr_vtx3f_stack_base = &point_stack_base;
   curr_vtx3f_stack = &point_stack_node;
   curr_vtx2f_stack_base = &scr_point_stack_base;
   curr_vtx2f_stack = &scr_point_stack_node;
 }

 if(mode == SGL_LINES)
  {
   curr_vtx3f_stack_base = &line_stack_base;
   curr_vtx3f_stack = &line_stack_node;
   curr_vtx2f_stack_base = &scr_line_stack_base;
   curr_vtx2f_stack = &scr_line_stack_node;
  }

 *curr_vtx3f_stack_base = NULL;
}

void sglEnd()
{
 while(*curr_vtx3f_stack)
  transform_vtx3f();
}

void sglRenderPoints()
{
 VEC2F p;

 curr_vtx2f_stack_base = &scr_point_stack_base;
 curr_vtx2f_stack = &scr_point_stack_node;
 *curr_vtx2f_stack_base = NULL;

 while(*curr_vtx2f_stack)
  {
   pop_vertex2f(&p[0], &p[1]);
   putpixel(buffer, p[0], p[1], makecol(255, 255, 255));
  }
}

void sglRenderLines()
{
 VEC2F p1, p2;

 curr_vtx2f_stack_base = &scr_line_stack_base;
 curr_vtx2f_stack = &scr_line_stack_node;
 *curr_vtx2f_stack_base = NULL;

 while(*curr_vtx2f_stack)
  {
   pop_vertex2f(&p1[0], &p1[1]);
   pop_vertex2f(&p2[0], &p2[1]);
   line(buffer, p1[0], p1[1], p2[0], p2[1], makecol(255, 255, 255));
  }
}

#include "sglmodel.h"
