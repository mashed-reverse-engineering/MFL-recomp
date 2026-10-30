/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z; } RwpV3;
typedef struct RwpLinkNode { int a, b, c; } RwpLinkNode;
typedef struct RwpSupportShape { RwpV3 center; int f0c; RwpLinkNode node; char pad[0x34]; RwpLinkNode *first; short count; } RwpSupportShape;
/* Initialises a support shape from a centre point: one link node, count 1. (Rwp library) */
void rwp_sphere_0056cc50(int a, int b, RwpV3 *center, RwpSupportShape *out)
{
  out->center = *center;
  out->first = &out->node;
  out->first->c = 0;
  out->first->b = 0;
  out->first->a = 0;
  out->count = 1;
  out->f0c = 0;
}
