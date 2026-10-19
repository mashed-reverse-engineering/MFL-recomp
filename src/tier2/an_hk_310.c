#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005ddc28*/ extern float k_f_0;

#define EDGE_TABLE_OFFSET (0xf0)
#define EDGE_ENTRY_SIZE (4)
#define EDGE_NORMAL_A_OFFSET (0xf2)
#define EDGE_NORMAL_B_OFFSET (0xf3)
#define VEC3_SIZE (0xc)
#define VERTEX_ARRAY_FLOAT_INDEX (0x24)

/* course_edge_side_test */

/*
 * Tests which side of a course edge a point is on. Normalised direction from the
 * edge's start vertex to the point, dotted with the two edge normals (inferred).
 *   edge_block : per-course edge data block (inferred). Each edge is a 4-byte entry at
 *                EDGE_TABLE_OFFSET + edge_index * EDGE_ENTRY_SIZE: start vertex index,
 *                then the two normal indices (EDGE_NORMAL_A/B_OFFSET). Vertices and
 *                normals are VEC3_SIZE-byte float triples.
 *   edge_index : which edge of edge_block to test.
 *   point      : world position (3 floats) to test.
 *   out_side   : optional (may be NULL). Receives 1 if dot_a <= dot_b, else 0.
 * Returns 1 if the point is across the edge (dot_b * dot_a < k_f_0), else 0.
 */

undefined4 __cdecl course_edge_side_test(char *edge_block,int edge_index,float *point,uint *out_side)

{
  char *vertex_addr;
  float *normal_a;
  float *normal_b;
  float dot_a;
  float dot_b;
  uint start_vertex;
  RwV3d dir;

  /* direction from the edge's start vertex to the point, normalised in place */
  start_vertex = (uint)*(byte *)(edge_block + EDGE_TABLE_OFFSET + edge_index * EDGE_ENTRY_SIZE);
  vertex_addr = edge_block + start_vertex * VEC3_SIZE;
  dir.x = *point - *(float *)(edge_block + (start_vertex * 3 + VERTEX_ARRAY_FLOAT_INDEX) * 4);
  dir.y = point[1] - *(float *)(vertex_addr + 0x94);
  dir.z = point[2] - *(float *)(vertex_addr + 0x98);
  RwV3dNormalize(&dir,&dir);
  /* project the direction onto both edge normals */
  normal_a = (float *)(edge_block + (uint)*(byte *)(edge_block + EDGE_NORMAL_A_OFFSET + edge_index * EDGE_ENTRY_SIZE) * VEC3_SIZE);
  normal_b = (float *)(edge_block + (uint)*(byte *)(edge_block + EDGE_NORMAL_B_OFFSET + edge_index * EDGE_ENTRY_SIZE) * VEC3_SIZE);
  dot_a = normal_a[2] * dir.z + dir.x * *normal_a + normal_a[1] * dir.y;
  dot_b = normal_b[2] * dir.z + dir.x * *normal_b + normal_b[1] * dir.y;
  /* optional output: 1 if dot_a <= dot_b, else 0 */
  if (out_side != (uint *)0x0) {
    *out_side = (uint)(dot_a <= dot_b);
  }
  /* opposite signs (product below threshold) means the point is across the edge */
  if (dot_b * dot_a < k_f_0) {
    return 1;
  }
  return 0;
}


