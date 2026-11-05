/* rwp_trilist_00574050 (0x00574050): RwpTrilist vtable slot (5f6808): bounding sphere of the RenderWare object behind a trilist shape */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflV4 { float x, y, z, w; } MflV4;
typedef struct MflBox16 { MflV3 a; float pad0; MflV3 b; float pad1; } MflBox16;

extern void rpAtomicResyncInterpolatedSphere(void *atomic);
extern void rwp_math_0056f180(MflV4 *sphere, MflBox16 *box);

/* The shape's object (+0x40) and its kind (+0x44): 0 / 1 = an object with a box at +0x50 / +0x48 (sphere around the box, 56f180),
   2 = an atomic (resync its interpolated sphere when flag 2 of +0x4c is set, sphere at +0x1c), 3 = the sphere at (+0x5c)->+4,
   4 = infinite (origin, FLT_MAX).  The shape margin (+0x4c) is added to the radius.  Returns `sphere`, 0 for an unknown kind. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflV4 *rwp_trilist_00574050(unsigned char *shape, MflV4 *sphere)
{
  unsigned char *obj;
  float *bb;
  MflV4 *src;
  MflBox16 box;

  switch (*(int *)(shape + 0x44)) {
  case 0:
  case 1:
    obj = *(unsigned char **)(shape + 0x40);
    bb = (float *)(obj + (*(int *)(shape + 0x44) == 1 ? 0x48 : 0x50));
    if (bb == 0)
      return 0;
    box.b = *(MflV3 *)(bb + 3);
    box.a = *(MflV3 *)bb;
    rwp_math_0056f180(sphere, &box);
    sphere->w = *(float *)(shape + 0x4c) + sphere->w;
    return sphere;
  case 2:
    obj = *(unsigned char **)(shape + 0x40);
    if (obj[0x4c] & 2)
      rpAtomicResyncInterpolatedSphere(obj);
    src = (MflV4 *)(*(unsigned char **)(shape + 0x40) + 0x1c);
    break;
  case 3:
    src = (MflV4 *)(*(unsigned char **)(*(unsigned char **)(shape + 0x40) + 0x5c) + 4);
    break;
  case 4:
    sphere->z = 0.0f;
    sphere->y = 0.0f;
    sphere->x = 0.0f;
    sphere->w = 3.4028235e38f;
    return sphere;
  default:
    return 0;
  }
  if (src == 0)
    return 0;
  *sphere = *src;
  sphere->w = *(float *)(shape + 0x4c) + sphere->w;
  return sphere;
}
