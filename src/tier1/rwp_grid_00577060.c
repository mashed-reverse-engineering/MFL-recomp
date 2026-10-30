/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z, w; } RwpQuat;
typedef struct RwpBodyState { char pad[0x3c]; RwpQuat rotation; } RwpBodyState;
typedef struct RwpGridCell { char pad[0x40]; RwpBodyState *body; } RwpGridCell;
/* Copies the body rotation quaternion of a grid cell; returns the destination. (Rwp library) */
RwpQuat *rwp_grid_00577060(RwpGridCell *cell, RwpQuat *out)
{
  *out = cell->body->rotation;
  return out;
}
