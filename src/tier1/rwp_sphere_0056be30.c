/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z; } RwpV3;
typedef struct { float x, y, z, w; } RwpQuat;
typedef struct RwpXform { int kind; RwpV3 origin; float scale; RwpV3 axis; RwpQuat rot; float f30, f34, f38; char pad1[0x0c]; int f48; } RwpXform;
/* Initialises a body transform: identity quaternion, zero origin, unit scale, 0.01 tolerances. (Rwp library) */
RwpXform *rwp_sphere_0056be30(RwpXform *x, int kind)
{
  RwpV3 zero = { 0.0f, 0.0f, 0.0f };
  RwpV3 one = { 1.0f, 1.0f, 1.0f };
  RwpQuat identity = { 0.0f, 0.0f, 0.0f, 1.0f };
  x->kind = kind;
  x->origin = zero;
  x->f34 = 0.01f;
  x->f38 = 0.01f;
  x->scale = 1.0f;
  x->f30 = 1.0f;
  x->axis = one;
  x->f48 = 0;
  x->rot = identity;
  return x;
}
