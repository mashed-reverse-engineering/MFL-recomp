/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float m[8]; } RwpBox32;
typedef struct RwpBodyState { char pad[0x4c]; RwpBox32 box; } RwpBodyState;
typedef struct RwpGridCell { char pad[0x40]; RwpBodyState *body; } RwpGridCell;
extern void rwp_math_0056f5b0(RwpBox32 *dst, RwpBox32 *src, void *xform);
/* Copies the body box of a grid cell and optionally transforms it in place; returns the destination. (Rwp library) */
RwpBox32 *rwp_grid_00577110(RwpGridCell *cell, void *xform, int unused, RwpBox32 *out)
{
  *out = cell->body->box;
  if (xform) rwp_math_0056f5b0(out, out, xform);
  return out;
}
