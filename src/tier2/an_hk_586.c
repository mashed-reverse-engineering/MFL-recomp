#include "mfl_tier2.h"
extern struct SpringNode g_spring_nodes[460];
/*00481fd0*/ extern double random_range();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005e64a4*/ extern float k_f_0;
/*006f2aa8*/ extern float g_gfx_6f2aa8;
/*006f2aac*/ extern float g_gfx_6f2aac;
/*006f2ab0*/ extern float g_gfx_6f2ab0;
/*006f2abc*/ extern float g_spring_grid_6f2abc;
/*006f2ad0*/ extern float g_spring_grid_6f2ad0;
/*006f2ad4*/ extern undefined4 g_spring_grid_6f2ad4;
/*008922a4*/ extern undefined4 g_spring_grid_8922a4[0x1000];
/*0089de00*/ extern char g_gfx_89de00[];
/*00891c74*/ extern char g_spring_grid_891c74[];
/*00891ca4*/ extern char g_spring_grid_891ca4[];

/* Nodes per row / column of the spring grid (23 x 23 = 460 nodes) */
#define GRID_SIZE (23)
/* SpringNode is 0x44 bytes = 17 floats (pointer stride when walking the force fields) */
#define SPRING_NODE_FLOATS (17)
/* One spring link record is 7 ints: row/col of node a, row/col of node b, rest length, stiffness, damping (inferred) */
#define SPRING_LINK_INTS (7)

/* spring_grid_prepare */

/* "clears the 23 x 23 grid forces and normalises the wind direction (inferred)"
 *
 * Per frame: zeroes the force accumulators (w2c, w30, w34) of every grid node,
 * applies drag from the node velocity (w14..w1c) and a random gust to each
 * unpinned node (w38 == 0), then applies the spring force of every link record
 * in g_spring_grid_8922a4 to its two end nodes (inferred). The wind direction
 * g_gfx_6f2aa8..ab0 is normalised first when it is non-zero. */

void spring_grid_prepare(void)

{
  float spring_z;
  float inv_len;
  double ta;
  double tb;
  double tc;
  double pp;
  float *force_y_ptr;
  int node_a;
  int *spring_link;
  SpringNode *node;
  int node_b;
  double gust;
  double rand_z;
  RwReal magnitude;
  struct {
    RwV3d drag;
    RwV3d delta_pos;
    float delta_vel_x;
    float delta_vel_y;
    float delta_vel_z;
    float spring_x;
    float spring_y;
    undefined1 pad_10[4];
    float scratch_x;
    float scratch_y;
    float scratch_z;
  } frame;

  /* clear the x, y, z force fields of every node (walks the w30 field, 17 floats per node) */
  force_y_ptr = &g_spring_nodes[0].w30;
  do {
    node_a = GRID_SIZE;
    do {
      force_y_ptr[-1] = 0.0;
      *force_y_ptr = 0.0;
      force_y_ptr[1] = 0.0;
      force_y_ptr = force_y_ptr + SPRING_NODE_FLOATS;
      node_a = node_a + -1;
    } while (node_a != 0);
  } while ((LONG_PTR)force_y_ptr < ((LONG_PTR)&g_spring_grid_891ca4 + 0x1));
  /* normalise the wind direction when it is non-zero */
  if (g_gfx_6f2ab0 + g_gfx_6f2aac + g_gfx_6f2aa8 != k_f_0) {
    RwV3dNormalize((RwV3d *)&g_gfx_6f2aa8,(RwV3d *)&g_gfx_6f2aa8);
  }
  /* per node: drag from the velocity, then a random gust on unpinned nodes */
  node = g_spring_nodes;
  do {
    node_a = GRID_SIZE;
    do {
      if (node->w38 == 0) {
        node->w30 = g_spring_grid_6f2abc * node->w00 + node->w30;
        frame.drag.x = node->w14 * k_f_n1;
        frame.drag.y = node->w18 * k_f_n1;
        frame.drag.z = node->w1c * k_f_n1;
        if (frame.drag.z + frame.drag.y + frame.drag.x != k_f_0) {
          RwV3dNormalize(&frame.drag,&frame.drag);
          frame.drag.x = g_spring_grid_6f2ad0 * frame.drag.x;
          frame.drag.y = g_spring_grid_6f2ad0 * frame.drag.y;
          frame.drag.z = g_spring_grid_6f2ad0 * frame.drag.z;
          magnitude = RwV3dLength((RwV3d *)&node->w14);
          frame.drag.x = frame.drag.x * magnitude;
          frame.drag.y = frame.drag.y * magnitude;
          frame.drag.z = magnitude * frame.drag.z;
          node->w2c = frame.drag.x + node->w2c;
          node->w30 = frame.drag.y + node->w30;
          node->w34 = frame.drag.z + node->w34;
        }
        /* random gust: uniform sample scaled per axis by the wind direction */
        gust = (double)random_range(0,g_spring_grid_6f2ad4);
        frame.scratch_x = (float)(gust * (double)g_gfx_6f2aa8);
        gust = (double)random_range(0,g_spring_grid_6f2ad4);
        frame.scratch_y = (float)(gust * (double)g_gfx_6f2aac);
        rand_z = (double)random_range(0,g_spring_grid_6f2ad4);
        gust = (double)g_gfx_6f2ab0;
        node->w2c = frame.scratch_x + node->w2c;
        node->w30 = frame.scratch_y + node->w30;
        node->w34 = (float)(rand_z * gust + (double)node->w34);
      }
      node = node + 1;
      node_a = node_a + -1;
    } while (node_a != 0);
  } while ((LONG_PTR)node < ((LONG_PTR)&g_spring_grid_891c74 + 0x1));
  /* spring links: Hooke force along the segment plus damping on the relative velocity, equal and opposite on the two ends (inferred) */
  spring_link = g_spring_grid_8922a4;
  do {
    node_a = spring_link[-1] * GRID_SIZE + *spring_link;
    node_b = spring_link[1] * GRID_SIZE + spring_link[2];
    spring_z = *(float *)&spring_link[5];
    frame.delta_pos.x = g_spring_nodes[node_a].w08 - g_spring_nodes[node_b].w08;
    frame.delta_pos.y = g_spring_nodes[node_a].w0c - g_spring_nodes[node_b].w0c;
    frame.delta_pos.z = g_spring_nodes[node_a].w10 - g_spring_nodes[node_b].w10;
    frame.delta_vel_x = g_spring_nodes[node_a].w14 - g_spring_nodes[node_b].w14;
    frame.delta_vel_y = g_spring_nodes[node_a].w18 - g_spring_nodes[node_b].w18;
    frame.delta_vel_z = g_spring_nodes[node_a].w1c - g_spring_nodes[node_b].w1c;
    magnitude = RwV3dLength(&frame.delta_pos);
    inv_len = float_1_0 / magnitude;
    /* spring_z holds the rest length on entry; it becomes the z force below */
    spring_z = (magnitude - spring_z) * *(float *)&spring_link[3];
    pp = frame.delta_vel_x * frame.delta_pos.x;
    ta = pp * inv_len;
    frame.spring_x = -(inv_len * frame.delta_pos.x * (ta * *(float *)&spring_link[4] + spring_z));
    pp = frame.delta_vel_y * frame.delta_pos.y;
    tb = pp * inv_len;
    frame.spring_y = -(inv_len * frame.delta_pos.y * (tb * *(float *)&spring_link[4] + spring_z));
    pp = frame.delta_vel_z * frame.delta_pos.z;
    tc = pp * inv_len;
    spring_z = -(inv_len * frame.delta_pos.z * (tc * *(float *)&spring_link[4] + spring_z));
    frame.scratch_x = frame.spring_x * k_f_n1;
    frame.scratch_y = frame.spring_y * k_f_n1;
    frame.scratch_z = k_f_n1 * spring_z;
    /* add to node a (only if unpinned), then the opposite force to node b */
    if (g_spring_nodes[node_a].w38 == 0) {
      g_spring_nodes[node_a].w2c = frame.spring_x + g_spring_nodes[node_a].w2c;
      g_spring_nodes[node_a].w30 = frame.spring_y + g_spring_nodes[node_a].w30;
      g_spring_nodes[node_a].w34 = spring_z + g_spring_nodes[node_a].w34;
    }
    if (g_spring_nodes[node_b].w38 == 0) {
      g_spring_nodes[node_b].w2c = frame.scratch_x + g_spring_nodes[node_b].w2c;
      g_spring_nodes[node_b].w30 = frame.scratch_y + g_spring_nodes[node_b].w30;
      g_spring_nodes[node_b].w34 = frame.scratch_z + g_spring_nodes[node_b].w34;
    }
    spring_link = spring_link + SPRING_LINK_INTS;
  } while ((LONG_PTR)spring_link < ((LONG_PTR)&g_gfx_89de00));
  return;
}
