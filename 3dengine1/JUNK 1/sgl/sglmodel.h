typedef struct {
 int vnum, tnum;
 int *a, *b, *c;
 VEC3F *v, *tr;
 VEC2F *s;
}SGL_MODEL;

int SGL_modelCounter = 0, SGL_currentModel = -1, SGL_modelFreeSpot = -1;
SGL_MODEL *SGL_modelCache;

#define sglCurrentModel SGL_modelCache[SGL_currentModel]
#define sglModelVertexXf(i) SGL_modelCache[SGL_currentModel].v[i][0]
#define sglModelVertexYf(i) SGL_modelCache[SGL_currentModel].v[i][1]
#define sglModelVertexZf(i) SGL_modelCache[SGL_currentModel].v[i][2]
#define SGL_modelTriA(i) sglCurrentModel.s[sglCurrentModel.a[i]]
#define SGL_modelTriB(i) sglCurrentModel.s[sglCurrentModel.b[i]]
#define SGL_modelTriC(i) sglCurrentModel.s[sglCurrentModel.c[i]]

void sglUploadModelVertex3f(int i, float x, float y, float z)
{
 if(SGL_currentModel > -1 && i < SGL_modelCache[SGL_currentModel].vnum && i > -1)
  {
   sglModelVertexXf(i) = x;
   sglModelVertexYf(i) = y;
   sglModelVertexZf(i) = z;  
  }
}

void sglUploadModelTri3i(int i, int a, int b, int c)
{
 if(SGL_currentModel > -1 && i < SGL_modelCache[SGL_currentModel].tnum && i > -1)
  {
   sglCurrentModel.a[i] = a;
   sglCurrentModel.b[i] = b;
   sglCurrentModel.c[i] = c;
  }
}

int sglAllocModel(int vnum, int tnum)
{
 SGL_modelCounter++;

 if(SGL_modelFreeSpot == -1)
  {
   SGL_currentModel = SGL_modelCounter - 1;
   SGL_modelCache = (SGL_MODEL *)realloc(SGL_modelCache, SGL_modelCounter * sizeof(SGL_MODEL));
  }

 else
  {
   SGL_currentModel = SGL_modelFreeSpot;
   SGL_modelFreeSpot = -1;
  }

 SGL_modelCache[SGL_currentModel].vnum = vnum;
 SGL_modelCache[SGL_currentModel].tnum = tnum;

 SGL_modelCache[SGL_currentModel].a = (int *)realloc(SGL_modelCache[SGL_currentModel].a, tnum * sizeof(int));
 SGL_modelCache[SGL_currentModel].b = (int *)realloc(SGL_modelCache[SGL_currentModel].b, tnum * sizeof(int));
 SGL_modelCache[SGL_currentModel].c = (int *)realloc(SGL_modelCache[SGL_currentModel].c, tnum * sizeof(int));

 SGL_modelCache[SGL_currentModel].v = (VEC3F *)realloc(SGL_modelCache[SGL_currentModel].v, vnum * sizeof(VEC3F));
 SGL_modelCache[SGL_currentModel].tr = (VEC3F *)realloc(SGL_modelCache[SGL_currentModel].tr, vnum * sizeof(VEC3F));
 SGL_modelCache[SGL_currentModel].s = (VEC2F *)realloc(SGL_modelCache[SGL_currentModel].s, vnum * sizeof(VEC2F));

 return SGL_currentModel;
}

void sglBindModel(int id)
{
 if(id > -1)
  SGL_currentModel = id;
}

void SGL_tr_model(int i)
{
 VEC3F source, temp;

 source[0] = SGL_modelCache[SGL_currentModel].v[i][0];
 source[1] = SGL_modelCache[SGL_currentModel].v[i][1];
 source[2] = SGL_modelCache[SGL_currentModel].v[i][2];

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

 SGL_modelCache[SGL_currentModel].s[i][0] = 320.0 + temp[0] * s;
 SGL_modelCache[SGL_currentModel].s[i][1] = 240.0 + temp[1] * s;
}

void sglRenderModel(int mode)
{
 int i;

 for(i = 0; i < sglCurrentModel.vnum; i++)
  SGL_tr_model(i);

 if(mode == SGL_POINTS)
  for(i = 0; i < sglCurrentModel.vnum; i++)
   putpixel(buffer, (int)sglCurrentModel.s[i][0], (int)sglCurrentModel.s[i][1], makecol(255, 255, 255));

 if(mode == SGL_LINES)
  for(i = 0; i < sglCurrentModel.tnum; i++)
   {
    line(buffer, SGL_modelTriA(i)[0], SGL_modelTriA(i)[1], SGL_modelTriB(i)[0], SGL_modelTriB(i)[1], makecol(255, 255, 255));
    line(buffer, SGL_modelTriB(i)[0], SGL_modelTriB(i)[1], SGL_modelTriC(i)[0], SGL_modelTriC(i)[1], makecol(255, 255, 255));
    line(buffer, SGL_modelTriC(i)[0], SGL_modelTriC(i)[1], SGL_modelTriA(i)[0], SGL_modelTriA(i)[1], makecol(255, 255, 255));
   }
}

int sglFreeModel(int id)
{
 if(id > -1)
  {
   free(SGL_modelCache[id].v);
   free(SGL_modelCache[id].tr);
   free(SGL_modelCache[id].s);
   free(SGL_modelCache[id].a);
   free(SGL_modelCache[id].b);
   free(SGL_modelCache[id].c);
   SGL_modelCounter--;
   SGL_modelFreeSpot = id;
   SGL_currentModel = id - 1;
  }

 return -1;
}
