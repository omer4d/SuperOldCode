#ifndef _MD2_
#define _MD2_

#include "base.h"
#include "common.h"
#include <stdio.h>
#include <malloc.h>

#define MD2_INVALID 2
#define MD2_NOT_FOUND 1
#define MD2_LOADED 0

typedef struct {
 int ident, version;
 int skinwidth, skinheight;
 int framesize;
 int num_skins, num_vertices, num_st, num_tris, num_glcmds, num_frames;
 int offset_skins, offset_st, offset_tris, offset_frames, offset_glcmds, offset_end;
}MD2_HEADER;

typedef struct {
 unsigned char v[3], normal_index;
}MD2_VERTEX;

typedef struct {
 short s, t;
}MD2_TEXCOORD;

typedef struct {
 float scale[3], translate[3];
 char name[16];
 MD2_VERTEX *vert;
}MD2_FRAME;

typedef struct {
 short index_xyz[3], index_st[3];
}MD2_TRIANGLE;

typedef struct {
 char name[64];
}MD2_SKIN;

typedef struct {
 MD2_HEADER header;
 MD2_SKIN *skin;
 MD2_TEXCOORD *texcoord;
 MD2_TRIANGLE *triangle;
 MD2_FRAME *frame;
 int *glcmd;
}MD2_MODEL;

int load_md2(const char *filename, MD2_MODEL *mdl)
{
 FILE *fp;
 int i;

 fp = fopen(filename, "rb");
 if(!fp) return MD2_NOT_FOUND;

 fread(&mdl->header, 1, sizeof(MD2_HEADER), fp);

 if(mdl->header.ident != 844121161 || mdl->header.version != 8)
  {
   fclose(fp);
   return MD2_INVALID;
  }

 mdl->skin = (MD2_SKIN *)malloc(sizeof(MD2_SKIN) * mdl->header.num_skins);
 mdl->texcoord = (MD2_TEXCOORD *)malloc(sizeof(MD2_TEXCOORD) * mdl->header.num_st);
 mdl->triangle = (MD2_TRIANGLE *)malloc(sizeof(MD2_TRIANGLE) * mdl->header.num_tris);
 mdl->frame = (MD2_FRAME *)malloc(sizeof(MD2_FRAME) * mdl->header.num_frames);
 mdl->glcmd = (int *)malloc(sizeof(int) * mdl->header.num_glcmds);

 fseek(fp, mdl->header.offset_skins, SEEK_SET);
 fread(mdl->skin, sizeof(MD2_SKIN), mdl->header.num_skins, fp);

 fseek(fp, mdl->header.offset_st, SEEK_SET);
 fread(mdl->texcoord, sizeof(MD2_TEXCOORD), mdl->header.num_st, fp);

 fseek(fp, mdl->header.offset_tris, SEEK_SET);
 fread(mdl->triangle, sizeof(MD2_TRIANGLE), mdl->header.num_tris, fp);

 fseek(fp, mdl->header.offset_glcmds, SEEK_SET);
 fread(mdl->glcmd, sizeof(int), mdl->header.num_glcmds, fp);

 fseek(fp, mdl->header.offset_frames, SEEK_SET);
 for(i = 0; i < mdl->header.num_frames; i++)
  {
   mdl->frame[i].vert = (MD2_VERTEX *)malloc(sizeof(MD2_VERTEX) * mdl->header.num_vertices);
   fread(mdl->frame[i].scale, sizeof(float [3]), 1, fp);
   fread(mdl->frame[i].translate, sizeof(float [3]), 1, fp);
   fread(mdl->frame[i].name, sizeof(char), 16, fp);
   fread(mdl->frame[i].vert, sizeof(MD2_VERTEX), mdl->header.num_vertices, fp);
  }

 printf("Loaded MD2 model '%s': %d triangles, %d frames.\n", filename, mdl->header.num_tris, mdl->header.num_frames);

 fclose(fp);
 return 0;
}

void free_md2(MD2_MODEL *mdl)
{
 int i;

 free(mdl->skin);
 free(mdl->texcoord);
 free(mdl->triangle);
 free(mdl->glcmd);

 for(i = 0; i < mdl->header.num_frames; i++)
  free(mdl->frame[i].vert);
 free(mdl->frame);
}

void convert_md2_to_base(VERTEX **model_vertex, POLY3D **model_tri, MD2_MODEL *mdl, float scale)
{
 int i;
 MD2_FRAME *pframe = &mdl->frame[0];
 MD2_VERTEX *pvert;

 *model_vertex = (VERTEX *)malloc(sizeof(VERTEX) * mdl->header.num_vertices);
 *model_tri = (POLY3D *)malloc(sizeof(POLY3D) * mdl->header.num_tris);

 for(i = 0; i < mdl->header.num_vertices; i++)
  {
   pvert = &pframe->vert[i];
   (*model_vertex)[i].object = vec3f((pvert->v[0] * pframe->scale[0] + pframe->translate[0]) * scale,
                                     (pvert->v[1] * pframe->scale[1] + pframe->translate[1]) * scale,
                                     (pvert->v[2] * pframe->scale[2] + pframe->translate[2]) * scale);
  }

 for(i = 0; i < mdl->header.num_tris; i++)
  {
   (*model_tri)[i].vnum = 3;
   (*model_tri)[i].vind = (long *)malloc(3 * sizeof(long));
   (*model_tri)[i].texcoord = (VEC2F *)malloc(3 * sizeof(VEC2F));
   (*model_tri)[i].col = random_color();
   (*model_tri)[i].vind[0] = mdl->triangle[i].index_xyz[0];
   (*model_tri)[i].vind[1] = mdl->triangle[i].index_xyz[1];
   (*model_tri)[i].vind[2] = mdl->triangle[i].index_xyz[2];

   (*model_tri)[i].texcoord[0].x = (float)mdl->texcoord[mdl->triangle[i].index_st[0]].s / (float)mdl->header.skinwidth;
   (*model_tri)[i].texcoord[0].y = (float)mdl->texcoord[mdl->triangle[i].index_st[0]].t / (float)mdl->header.skinheight;
   (*model_tri)[i].texcoord[1].x = (float)mdl->texcoord[mdl->triangle[i].index_st[1]].s / (float)mdl->header.skinwidth;
   (*model_tri)[i].texcoord[1].y = (float)mdl->texcoord[mdl->triangle[i].index_st[1]].t / (float)mdl->header.skinheight;
   (*model_tri)[i].texcoord[2].x = (float)mdl->texcoord[mdl->triangle[i].index_st[2]].s / (float)mdl->header.skinwidth;
   (*model_tri)[i].texcoord[2].y = (float)mdl->texcoord[mdl->triangle[i].index_st[2]].t / (float)mdl->header.skinheight;
  }
}

void convert_md2_frame_to_base(VERTEX *model_vertex, MD2_MODEL *mdl, float n, float scale)
{
 int i, f1 = (int)n, f2 = (int)n + 1;
 float k = n - f1;

 MD2_FRAME *pframe1 = &mdl->frame[wrap_int(f1, 0, mdl->header.num_frames)];
 MD2_FRAME *pframe2 = &mdl->frame[wrap_int(f2, 0, mdl->header.num_frames)];
 MD2_VERTEX *pvert1;
 MD2_VERTEX *pvert2;
 VEC3F rv1, rv2;

 for(i = 0; i < mdl->header.num_vertices; i++)
  {
   pvert1 = &pframe1->vert[i];
   pvert2 = &pframe2->vert[i];

   rv1 = vec3f((pvert1->v[0] * pframe1->scale[0] + pframe1->translate[0]) * scale,
               (pvert1->v[1] * pframe1->scale[1] + pframe1->translate[1]) * scale,
               (pvert1->v[2] * pframe1->scale[2] + pframe1->translate[2]) * scale);

   rv2 = vec3f((pvert2->v[0] * pframe2->scale[0] + pframe2->translate[0]) * scale,
               (pvert2->v[1] * pframe2->scale[1] + pframe2->translate[1]) * scale,
               (pvert2->v[2] * pframe2->scale[2] + pframe2->translate[2]) * scale);

   model_vertex[i].object = VEC3F_BLEND(rv1, rv2, (1.0 - k));
  }
}

#endif
