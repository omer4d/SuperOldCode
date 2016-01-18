#include <stdio.h>

typedef struct {
 float x, y;
}_2D;

typedef struct {
 float x, y, z;
}_3D;

typedef struct {
 _3D local;
 _2D screen[2];
}VERTEX;

typedef struct{
   float x,y,z;
}vertex_type;

typedef struct{
   int a,b,c;
}polygon_type;

typedef struct {
 int u, v;
}dummy;

#define MAX_VERTICES 8000
#define MAX_POLYGONS 8000

typedef struct{
   VERTEX vertex[MAX_VERTICES];
   polygon_type polygon[MAX_POLYGONS];
 dummy mapcoord[MAX_VERTICES];
 char name[100];
 long vertices_qty;
 long polygons_qty;
}obj_type,*obj_type_ptr;

char Load3DS (obj_type_ptr p_object, char *p_filename)
{
   int i;
   FILE *l_file;
   unsigned short l_chunk_id;
   unsigned int l_chunk_length;
   unsigned char l_char;
   unsigned short l_qty;
   unsigned short l_face_flags;
   
      if ((l_file=fopen (p_filename, "rb"))== NULL) return 0; //Open the file
   while (ftell (l_file) < filelength (fileno (l_file))) //Loop to scan the whole file 
   {
         
          fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
      fread (&l_chunk_length, 4, 1, l_file); //Read the length of the chunk
      
           switch (l_chunk_id)
      {
         case 0x4d4d: 
         break;
         
           case 0x3d3d:
         break;
         
               case 0x4000: 
            i=0;
            do
            {
               fread (&l_char, 1, 1, l_file);
               p_object->name[i]=l_char;
               i++;
            }while(l_char != '\0' && i<20);
         break;
         
                 case 0x4100:
         break;
         
                case 0x4110: 
            fread (&l_qty, sizeof (unsigned short), 1, l_file);
            p_object->vertices_qty = l_qty;
            printf("Number of vertices: %d\n",l_qty);
            for (i=0; i<l_qty; i++)
            {
               fread (&p_object->vertex[i].local.x, sizeof(float), 1, l_file);
               fread (&p_object->vertex[i].local.y, sizeof(float), 1, l_file);
               fread (&p_object->vertex[i].local.z, sizeof(float), 1, l_file);
            }
         break;
         
            case 0x4120:
            fread (&l_qty, sizeof (unsigned short), 1, l_file);
            p_object->polygons_qty = l_qty;
            printf("Number of polygons: %d\n",l_qty); 
            for (i=0; i<l_qty; i++)
            {
               fread (&p_object->polygon[i].a, sizeof (unsigned short), 1, l_file);
               fread (&p_object->polygon[i].b, sizeof (unsigned short), 1, l_file);
               fread (&p_object->polygon[i].c, sizeof (unsigned short), 1, l_file);
               fread (&l_face_flags, sizeof (unsigned short), 1, l_file);
            }
         break;
         
         
               case 0x4140:
            fread (&l_qty, sizeof (unsigned short), 1, l_file);
            for (i=0; i<l_qty; i++)
            {
               fread (&p_object->mapcoord[i].u, sizeof (float), 1, l_file);
               fread (&p_object->mapcoord[i].v, sizeof (float), 1, l_file);
            }
         break;
         
                default:
            fseek(l_file, l_chunk_length-6, SEEK_CUR);
       
      }
       
   }
   
      fclose (l_file); // Closes the file stream
   return (1); // Returns ok
}
