// cluster-entries: course_neustein_cb2_sub_455060
/* Installed cluster course_neustein_cb2_sub_455060 (tools/cluster_install.py from output/cluster_pilot_kg/course_neustein_cb2_sub_455060.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0048e6f0*/ extern int rwp_object_get_matrix(int handle);
/*004a9e70*/ extern int d3d9_draw_with_transform(void *vertices, int count);
/*006a1d98*/ extern int g_data_6a1d98;
/*006a1da4*/ extern int g_data_6a1da4;
/* course_neustein_cb2_sub_455060 */

/* single caller course_neustein_cb2 */

#define HULL_COUNT 2
#define CORNERS_PER_HULL 8

/* one vertex of the debug wireframe buffer (0x20 bytes) */
typedef struct DebugVertex {
  float x, y, z;
  unsigned char _gap_c[2];
  unsigned char tag;         /* +0x0e, always 0xff */
  unsigned char _gap_f;
  float dx, dy, dz;          /* the edge vector (first two vertices of a hull: the corner relative to the hull position) */
  unsigned char colour[4];   /* +0x1c: 0, 0, 0, 0xff */
} DebugVertex;

/* the two debug hulls: the object handle and the two corners that start the wireframe */
typedef struct DebugHull {
  int handle;
  char _gap_4[8];
  int corner_a;              /* +0x0c */
  int corner_b;              /* +0x10 */
  char _gap_14[0x14];
} DebugHull;

extern DebugHull g_neustein_hulls[HULL_COUNT];                         /* 0x6a188c */
extern DebugVertex g_neustein_vertices[];                              /* 0x6a18c8 */
extern float g_neustein_corners[HULL_COUNT * CORNERS_PER_HULL][3];     /* 0x6a1cc8 */

/* "builds the vertex list that draws the edges of the two debug hulls (the a-b edge, then the chain of the corners that is not the a-b edge)" and draws it */
void course_neustein_cb2_sub_455060(void)

{
  int hull;
  int base;
  int a;
  int b;
  int j;
  int k;
  float *origin;
  float *corner;
  float *next;
  DebugVertex *v;

  if (g_data_6a1d98 != 0) {
    for (hull = 0; hull < HULL_COUNT; hull++) {
      a = g_neustein_hulls[hull].corner_a;
      b = g_neustein_hulls[hull].corner_b;
      origin = (float *)(rwp_object_get_matrix(g_neustein_hulls[hull].handle) + 0x30);
      base = hull * CORNERS_PER_HULL;
      corner = g_neustein_corners[base + a];
      v = &g_neustein_vertices[g_data_6a1da4];
      v->x = corner[0];
      v->y = corner[1];
      v->z = corner[2];
      v->dx = corner[0] - origin[0];
      v->dy = corner[1] - origin[1];
      v->dz = corner[2] - origin[2];
      v->colour[2] = 0;
      v->colour[1] = 0;
      v->colour[0] = 0;
      v->colour[3] = 0xff;
      v->tag = 0xff;
      corner = g_neustein_corners[base + b];
      v = &g_neustein_vertices[g_data_6a1da4 + 1];
      v->x = corner[0];
      v->y = corner[1];
      v->z = corner[2];
      v->dx = corner[0] - origin[0];
      v->dy = corner[1] - origin[1];
      v->dz = corner[2] - origin[2];
      g_data_6a1da4 = g_data_6a1da4 + 2;
      v->colour[2] = 0;
      v->colour[1] = 0;
      v->colour[0] = 0;
      v->colour[3] = 0xff;
      v->tag = 0xff;
      v = &g_neustein_vertices[g_data_6a1da4];
      for (j = 0, k = 1; k < CORNERS_PER_HULL; j++, k++) {
        if (((j != a) || (k != b)) && ((k != a) || (j != b))) {
          corner = g_neustein_corners[base + j];
          next = g_neustein_corners[base + k];
          v->x = corner[0];
          v->y = corner[1];
          v->z = corner[2];
          v->dx = corner[0] - next[0];
          v->dy = corner[1] - next[1];
          g_data_6a1da4 = g_data_6a1da4 + 1;
          v->dz = corner[2] - next[2];
          v->colour[2] = 0;
          v->colour[1] = 0;
          v->colour[0] = 0;
          v->colour[3] = 0xff;
          v->tag = 0xff;
          v = v + 1;
        }
      }
    }
    d3d9_draw_with_transform(g_neustein_vertices, g_data_6a1da4);
    g_data_6a1da4 = 0;
  }
  return;
}

