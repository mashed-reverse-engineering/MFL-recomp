// cluster-entries: spring_grid_build
/* Installed cluster spring_grid_build (tools/cluster_install.py from output/cluster_pilot_uc3/spring_grid_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddb00*/ extern float k_f_0_025;
/*005dd340*/ extern float k_f_0_5;
/*005e0408*/ extern float k_f_1_1961722;
/*005dd634*/ extern float k_f_2;
/*005dd32c*/ extern float k_f_3;
/*005dd7f0*/ extern float k_f_30;
/*005dd814*/ extern float k_f_6;
extern float k_f_1_5789474, k_f_0_033333335, k_f_1_8181819, k_f_0_33333334;
extern SpringNode g_spring_nodes[460];
/* DECLS-END */
extern unsigned int g_spring_grid_6f2ac0, g_spring_grid_6f2ac4, g_spring_grid_6f2ac8;
extern float g_spring_grid_6f2acc;
extern unsigned int g_gfx_6f2aa8, g_gfx_6f2aac, g_gfx_6f2ab0;
extern float float_1_0;
extern int g_spring_grid_8922a0;

/* One spring between two grid nodes (28 bytes): the node (row, col) and the node (row2, col2), two constants copied from the spring table (stiffness / damping) and the rest length. */
typedef struct SpringLink {
  int row;
  int col;
  int row2;
  int col2;
  unsigned int stiffness;
  unsigned int damping;
  float length;
} SpringLink;

#define NODE(r, c) g_spring_nodes[(r) * 0x17 + (c)]
#define LINKS ((SpringLink *)&g_spring_grid_8922a0)

/* Distance of two grid nodes (the spring's rest length). */
static float node_distance(int r, int c, int r2, int c2)
{
  RwV3d d;

  d.x = NODE(r, c).w08 - NODE(r2, c2).w08;
  d.y = NODE(r, c).w0c - NODE(r2, c2).w0c;
  d.z = NODE(r, c).w10 - NODE(r2, c2).w10;
  return RwV3dLength(&d);
}

/* Adds a spring from (r, c) to (r2, c2) to the link table. */
static void link_add(int *count, int r, int c, int r2, int c2, unsigned int stiffness, unsigned int damping)
{
  SpringLink *l = &LINKS[*count];

  l->row2 = r2;
  l->col2 = c2;
  l->stiffness = stiffness;
  l->damping = damping;
  l->row = r;
  l->col = c;
  l->length = node_distance(r, c, r2, c2);
  *count = *count + 1;
}

/* Builds the 23 x 20 grid of cloth nodes (mass, position, pinned flags) and the table of springs between neighbouring nodes. */
void spring_grid_build(void)
{
  int r;
  int c;
  int n;
  float y;
  float z;
  float mass;
  float w;
  SpringNode *node;

  for (r = 0; r < 0x14; r++) {
    y = k_f_30 - (float)r * k_f_1_5789474;
    z = g_spring_grid_6f2acc + y;
    y = (float_1_0 - y * k_f_0_033333335) - k_f_0_5;
    for (c = 0; c < 0x17; c++) {
      if (r == 0) {
        mass = (c == 0) ? float_1_0 : ((c == 0x16) ? k_f_2 : k_f_3);
      }
      else if (r == 0x13) {
        mass = (c == 0) ? k_f_2 : ((c == 0x16) ? float_1_0 : k_f_3);
      }
      else {
        mass = k_f_6;
      }
      node = &NODE(r, c);
      w = mass * k_f_1_1961722 * k_f_0_33333334;
      node->w0c = z;
      node->w10 = 0.0f;
      node->w14 = 0.0f;
      node->w00 = w;
      node->w18 = 0.0f;
      node->w1c = 0.0f;
      node->w20 = 0;
      node->w24 = 0;
      node->w28 = 0;
      node->w2c = 0.0f;
      node->w30 = 0.0f;
      node->w34 = 0.0f;
      node->w04 = float_1_0 / w;
      w = (float)c * k_f_1_8181819;
      node->w08 = w;
      node->w3c = w * k_f_0_025 - k_f_0_5;
      node->w40 = y;
      if (c == 0 || c == 0x16) {
        node->w38 = (r == 0 || r == 0x13 || r == 9) ? 1 : 0;
      }
      else {
        node->w38 = (c == 0xb && (r == 0 || r == 0x13 || r == 0xd)) ? 1 : 0;
      }
    }
  }
  n = 0;
  for (r = 0; r < 0x14; r++) {
    for (c = 0; c < 0x17; c++) {
      if (c < 0x16) {
        link_add(&n, r, c, r, c + 1, g_spring_grid_6f2ac0, g_spring_grid_6f2ac8);
      }
      if (r < 0x13) {
        link_add(&n, r, c, r + 1, c, g_spring_grid_6f2ac0, g_spring_grid_6f2ac8);
        if (c < 0x16) {
          link_add(&n, r, c, r + 1, c + 1, g_spring_grid_6f2ac4, g_spring_grid_6f2ac8);
        }
      }
      if (c > 0 && r < 0x13) {
        link_add(&n, r, c, r + 1, c - 1, g_spring_grid_6f2ac4, g_spring_grid_6f2ac8);
      }
    }
  }
  g_gfx_6f2aa8 = 0x41200000;
  g_gfx_6f2aac = 0;
  g_gfx_6f2ab0 = 0;
}

