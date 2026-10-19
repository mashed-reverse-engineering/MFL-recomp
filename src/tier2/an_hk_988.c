#include "mfl_tier2.h"
/*00487ee0*/ extern undefined __cdecl triangle_normal_cross(struct RwV3d *p0, float *p1, float *p2, float *p3);
/*00487f80*/ extern undefined __cdecl vector_normalize_copy(struct RwV3d *p0, float *p1, RwReal *p2);
/*0048cff0*/ extern int __cdecl course_edge_side_test(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005dd330*/ extern float float_1_0;
/* course_collision_object_push_vectors */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "for each face of the object near the point accumulates push direction and writes the face
   normals" */

#define OBJ_FACE_COUNT (0x148)
#define OBJ_FACE_EDGES (0xf0)      /* 4 bytes per face, the first two are vertex indices */
#define OBJ_VERTEX_BASE (0xc)      /* vertices from obj + 12 * 12 */
#define OBJ_PUSH_ORIGIN (0x138)

void __cdecl course_collision_object_push_vectors(char *obj,float *point,RwV3d *normals,int *hit_count)

{
  uint face;
  int hits;
  int side;
  float *vertex;
  byte first;
  byte second;
  RwV3d sum;
  float10 scale;
  float scaled_y;
  float scaled_z;
  RwV3d target;
  char *edges;
  
  sum.x = 0.0;
  sum.y = 0.0;
  sum.z = 0.0;
  hits = 0;
  if (*(uint *)(obj + OBJ_FACE_COUNT) != 0) {
    face = 0;
    edges = obj + OBJ_FACE_EDGES;
    do {
      if (course_edge_side_test((undefined4)obj,face,(undefined4)point,(undefined4)&side) != 0) {
        hits = hits + 1;
        if (side == 0) {
          first = edges[0];
          second = edges[1];
        }
        else {
          first = edges[1];
          second = edges[0];
        }
        vertex = (float *)(obj + (first + OBJ_VERTEX_BASE) * 12);
        sum.x = sum.x + vertex[0];
        sum.y = vertex[1] + sum.y;
        sum.z = vertex[2] + sum.z;
        triangle_normal_cross((RwV3d *)normals,point,vertex,(float *)(obj + (second + OBJ_VERTEX_BASE) * 12));
        normals = (RwV3d *)((char *)normals + 0x10);
      }
      face = face + 1;
      edges = edges + 4;
    } while (face < *(uint *)(obj + OBJ_FACE_COUNT));
    if (hits != 0) {
      scale = float_1_0 / (float10)hits;
      scaled_y = (float)(scale * sum.y);
      scaled_z = (float)(scale * sum.z);
      target.x = (float)((float10)point[0] - scale * sum.x);
      target.y = point[1] - scaled_y;
      target.z = point[2] - scaled_z;
      vector_normalize_copy((RwV3d *)((char *)normals + 0),(float *)(obj + OBJ_PUSH_ORIGIN),&target.x);
      hits = hits + 1;
    }
  }
  if (hit_count != (int *)0x0) {
    *hit_count = hits;
  }
  return;
}
