#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define NODE_SIZE (0x18) /* bytes allocated for one node */
#define OWNER_SCALE_OFFSET (0x80) /* float at owner+0x80, its inverse is stored in the node (inferred) */
/* node_create_from_two_vectors */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "allocates a 0x18 byte node and fills it from two vec2" */
/* Allocates a NODE_SIZE node through the engine allocator and fills it from two vec2:
 *   node[0..1] = vec_b (x, y), node[2] = vec_a.x, node[3] = 1 / owner's scale float,
 *   node[4..5] = 0. Returns the node, or NULL if the allocation failed.
 *   owner: object whose scale sits at OWNER_SCALE_OFFSET (inferred) */
undefined4 * __cdecl node_create_from_two_vectors(char *owner,undefined4 *vec_a,undefined4 *vec_b)
{
  float inv_scale, s;
  undefined4 *node;

  node = RwMalloc(NODE_SIZE,0);
  /* fill the node only when the allocation succeeded */
  if (node != (undefined4 *)0x0) {
    *node = *vec_b;
    node[1] = vec_b[1];
    node[2] = *vec_a;
    s = *(float *)(owner + OWNER_SCALE_OFFSET);
    inv_scale = float_1_0 / s;
    node[4] = 0;
    node[5] = 0;
    *(float *)&node[3] = inv_scale;
    return node;
  }
  return (undefined4 *)0x0;
}
