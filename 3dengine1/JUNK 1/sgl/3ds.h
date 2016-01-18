#include <stdio.h>
#include <math.h>

typedef struct {
 unsigned char name[20];
 unsigned short vnum, tnum;
 float *x, *y, *z;
 float *nx, *ny, *nz;
 unsigned short *a, *b, *c;
 float *u, *v;
}MODEL_3DS;
/*
typedef struct {
 float x, y, z;
}VEC3F;

void normalize_vec3f(VEC3F *v)
{
 float len = (float)(sqrt(v->x * v->x + v->y * v->y + v->z * v->z));

 if(len == 0.0f)
  len = 1.0f;

 v->x /= len;
 v->y /= len;
 v->z /= len;
}

VEC3F surface_normal(VEC3F v[3])
{
 VEC3F a, b, n;

 a.x = v[0].x - v[1].x;
 a.y = v[0].y - v[1].y;
 a.z = v[0].z - v[1].z;

 b.x = v[1].x - v[2].x;
 b.y = v[1].y - v[2].y;
 b.z = v[1].z - v[2].z;

 n.x = (a.y * b.z) - (a.z * b.y);
 n.y = (a.z * b.x) - (a.x * b.z);
 n.z = (a.x * b.y) - (a.y * b.x);

 normalize_vec3f(&n);

 return n;
}

VEC3F make_vec3f(float x, float y, float z)
{
 VEC3F v;

 v.x = x;
 v.y = y;
 v.z = z;

 return v;
}
*/
int load_3ds(MODEL_3DS *model, char *filename, float scale)
{
 int i;
 FILE *file;

 unsigned short l_chunk_id;
 unsigned int l_chunk_length;
 unsigned char l_char;
 unsigned short l_qty;
 unsigned short l_face_flags;

 if((file = fopen(filename, "rb")) == NULL) return -1;

 while(ftell(file) < filelength(fileno(file)))
  {
   fread(&l_chunk_id, 2, 1, file);
   fread(&l_chunk_length, 4, 1, file);

   switch(l_chunk_id)
    {
     case 0x4D4D: 
     break;

     case 0x3D3D:
     break;

     case 0x4000: // Read model name:
      i = 0;
      while(l_char != '\0' && i < 20)
       {
        fread (&l_char, 1, 1, file);
        model->name[i] = l_char;
        i++;
       }
     break;

     case 0x4100:
     break;

     case 0x4110: // Read model vertices:
      fread(&l_qty, sizeof(unsigned short), 1, file);
      model->vnum = l_qty;
      model->x = (float *)malloc(l_qty * sizeof(float));
      model->y = (float *)malloc(l_qty * sizeof(float));
      model->z = (float *)malloc(l_qty * sizeof(float));
      for(i = 0; i < l_qty; i++)
       {
        fread(&model->x[i], sizeof(float), 1, file);
        fread(&model->y[i], sizeof(float), 1, file);
        fread(&model->z[i], sizeof(float), 1, file);
       }
     break;

     case 0x4120: // Read model polygons:
      fread(&l_qty, sizeof(unsigned short), 1, file);
      model->tnum = l_qty;
      model->a = (unsigned short *)malloc(l_qty * sizeof(unsigned short));
      model->b = (unsigned short *)malloc(l_qty * sizeof(unsigned short));
      model->c = (unsigned short *)malloc(l_qty * sizeof(unsigned short));
      model->nx = (float *)malloc(l_qty * sizeof(float));
      model->ny = (float *)malloc(l_qty * sizeof(float));
      model->nz = (float *)malloc(l_qty * sizeof(float));
      for(i = 0; i < l_qty; i++)
       {
        fread(&model->a[i], sizeof(unsigned short), 1, file);
        fread(&model->b[i], sizeof(unsigned short), 1, file);
        fread(&model->c[i], sizeof(unsigned short), 1, file);
        fread (&l_face_flags, sizeof(unsigned short), 1, file);
       }
     break;

     case 0x4140:
      fread(&l_qty, sizeof(unsigned short), 1, file);
      model->u = (float *)malloc(l_qty * sizeof(float));
      model->v = (float *)malloc(l_qty * sizeof(float));
      for(i = 0; i < l_qty; i++)
       {
        fread(&model->u[i], sizeof(float), 1, file);
        fread(&model->v[i], sizeof(float), 1, file);
       }
     break;

     default:
      fseek(file, l_chunk_length - 6, SEEK_CUR);
    }
  }

 for(i = 0; i < model->vnum; i++)
  {
   model->x[i] *= scale;
   model->y[i] *= scale;
   model->z[i] *= scale;
  }
/*
 VEC3F v[3], n;

 for(i = 0; i < model->tnum; i++)
  {
   v[0] = make_vec3f(model->x[model->a[i]], model->y[model->a[i]], model->z[model->a[i]]);
   v[1] = make_vec3f(model->x[model->b[i]], model->y[model->b[i]], model->z[model->b[i]]);
   v[2] = make_vec3f(model->x[model->c[i]], model->y[model->c[i]], model->z[model->c[i]]);
   n = surface_normal(v);
   model->nx[i] = n.x; 
   model->ny[i] = n.y;
   model->nz[i] = n.z;
  }
*/
 printf("Loaded model: '%s', %d vertices, %d polygons.", model->name, model->vnum, model->tnum);

 fclose(file);
 return 0;
}

void destroy_3ds(MODEL_3DS *model)
{
 free(model->x);
 free(model->y);
 free(model->z);
 free(model->nx);
 free(model->ny);
 free(model->nz);
 free(model->a);
 free(model->b);
 free(model->c);
 free(model->u);
 free(model->v);
}
