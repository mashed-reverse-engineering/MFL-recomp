// cluster-entries: car_wheel_contact_update_cb_477170
/* Installed cluster car_wheel_contact_update_cb_477170 (tools/cluster_install.py from output/cluster_pilot_kg/car_wheel_contact_update_cb_477170.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008bd74c*/ extern int g_collision_triangle_count;
/*008bd794*/ extern int g_car_8bd794;
/* car_wheel_contact_update_cb_477170 */

/* function pointer pushed/stored by car_wheel_contact_update (PUSH 0x477170) */

/* one collected triangle (0x90 bytes) */
typedef struct ContactTriangle {
  float v[3][3];         /* the three vertices */
  float normal[3];       /* +0x24 */
  int material;          /* +0x30 the triangle's material index */
  int surface;           /* +0x34 the surface code of that material (BSP surface table) */
  char _gap_38[4];
  float distance;        /* +0x3c 100000.0 until a test shortens it */
  float hit[3];          /* +0x40 cleared */
  char _gap_4c[0x20];
  float edge_normal[3][3]; /* +0x6c, +0x78, +0x84: normal x edge of v0-v1, v1-v2, v2-v0 */
} ContactTriangle;

/* rounds an extended precision value to float, like the spill of the original */
static float float_barrier(float value)
{
  return value;
}

/* triangle callback of the wheel contact search: `hit` = { normal[3], ?, ?, triangle index, vertex pointers [3] }, `sector` = the world sector (+4 triangles);
   appends the triangle (vertices, normal, material, edge normals) to the output buffer at ctx_out[g_collision_triangle_count] */
void car_wheel_contact_update_cb_477170(int unused_1, int sector, float *hit, int unused_4, ContactTriangle *out)

{
  ContactTriangle *t;
  float *vertex;
  int material;
  double ex;
  double ey;
  double ez;
  float rounded;
  float nx;
  float ny;
  float nz;

  t = out + g_collision_triangle_count;
  material = *(unsigned short *)(*(int *)(sector + 4) + 6 + *(int *)&hit[6] * 8);
  t->surface = *(int *)(*(int *)(*(int *)(*(int *)(g_car_8bd794 + 8) + 0x10) + material * 4) + 4);
  vertex = *(float **)&hit[7];
  t->v[0][0] = vertex[0];
  t->v[0][1] = vertex[1];
  t->v[0][2] = vertex[2];
  vertex = *(float **)&hit[8];
  t->v[1][0] = vertex[0];
  t->v[1][1] = vertex[1];
  t->v[1][2] = vertex[2];
  vertex = *(float **)&hit[9];
  t->v[2][0] = vertex[0];
  t->v[2][1] = vertex[1];
  t->v[2][2] = vertex[2];
  t->normal[0] = hit[0];
  t->normal[1] = hit[1];
  t->normal[2] = hit[2];
  t->material = material;
  t->hit[2] = 0.0f;
  t->hit[1] = 0.0f;
  t->hit[0] = 0.0f;
  t->distance = 100000.0f;
  nx = t->normal[0];
  ny = t->normal[1];
  nz = t->normal[2];
  /* the original keeps the edge components in extended precision, only the ones it spilled (dz of edge 0, dx of edges 1 and 2) are rounded to float */
  ex = t->v[1][0] - t->v[0][0];
  ey = t->v[1][1] - t->v[0][1];
  ez = t->v[1][2] - t->v[0][2];
  rounded = float_barrier(ez);
  t->edge_normal[0][0] = ez * ny - ey * nz;
  t->edge_normal[0][1] = ex * nz - rounded * nx;
  t->edge_normal[0][2] = ey * nx - ex * ny;
  rounded = float_barrier(t->v[2][0] - t->v[1][0]);
  ey = t->v[2][1] - t->v[1][1];
  ez = t->v[2][2] - t->v[1][2];
  t->edge_normal[1][0] = ny * ez - ey * nz;
  t->edge_normal[1][1] = rounded * nz - ez * nx;
  t->edge_normal[1][2] = ey * nx - rounded * ny;
  rounded = float_barrier(t->v[0][0] - t->v[2][0]);
  ey = t->v[0][1] - t->v[2][1];
  ez = t->v[0][2] - t->v[2][2];
  t->edge_normal[2][0] = ny * ez - ey * nz;
  t->edge_normal[2][1] = rounded * nz - ez * nx;
  t->edge_normal[2][2] = ey * nx - rounded * ny;
  g_collision_triangle_count = g_collision_triangle_count + 1;
  return;
}

