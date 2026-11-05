/* rwp_trilist_00574290 (0x00574290): RwpTrilist vtable slot (5f6810): axis-aligned bounding box of the RenderWare object behind a trilist shape */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;

extern void rpAtomicResyncInterpolatedSphere(void *atomic);
extern void rwp_box_extent_in_frame_transposed(float *out, float *box, float *m);

/* Object kinds as in rwp_trilist_00574050: 0 / 1 = box at +0x50 / +0x48 (taken over, then moved into `frame` by 56f5b0), 2 / 3 = bounding
   sphere (centre moved by the frame position, radius grown by the margin +0x4c), 4 = the fixed box +-10000.  box[0..2] = max corner,
   box[4..6] = min corner.  Returns `box`, 0 for an unknown kind or a missing object box. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float *rwp_trilist_00574290(unsigned char *shape, float *frame, void *unused, float *box)
{
  unsigned char *obj;
  float *bb, *s;
  MflV3 c;
  float r;

  switch (*(int *)(shape + 0x44)) {
  case 0:
  case 1:
    obj = *(unsigned char **)(shape + 0x40);
    bb = (float *)(obj + (*(int *)(shape + 0x44) == 1 ? 0x48 : 0x50));
    if (bb == 0)
      return 0;
    *(MflV3 *)(box + 4) = *(MflV3 *)(bb + 3);
    *(MflV3 *)box = *(MflV3 *)bb;
    if (frame != 0)
      rwp_box_extent_in_frame_transposed(box, box, frame);
    return box;
  case 2:
    obj = *(unsigned char **)(shape + 0x40);
    if (obj[0x4c] & 2)
      rpAtomicResyncInterpolatedSphere(obj);
    s = (float *)(*(unsigned char **)(shape + 0x40) + 0x1c);
    break;
  case 3:
    s = (float *)(*(unsigned char **)(*(unsigned char **)(shape + 0x40) + 0x5c) + 4);
    break;
  case 4:
    box[4] = -10000.0f;
    box[5] = -10000.0f;
    box[6] = -10000.0f;
    box[0] = 10000.0f;
    box[1] = 10000.0f;
    box[2] = 10000.0f;
    return box;
  default:
    return 0;
  }
  if (s == 0)
    return 0;
  if (frame != 0) {
    c.x = frame[12] + s[0];
    c.y = s[1] + frame[13];
    c.z = s[2] + frame[14];
  } else {
    c = *(MflV3 *)s;
  }
  r = *(float *)(shape + 0x4c) + s[3];
  box[0] = c.x + r;
  box[1] = c.y + r;
  box[2] = c.z + r;
  box[4] = c.x - r;
  box[5] = c.y - r;
  box[6] = c.z - r;
  return box;
}
