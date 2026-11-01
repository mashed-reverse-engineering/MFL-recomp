#include "mfl_tier2.h"
#include "rwp_common.h"
extern int RtQuatConvertFromMatrix();

/* Body 4x3 frame (rows right/up/at at +0x00/+0x10/+0x20, position +0x30), shape vtable at +0x5c. */
typedef struct MflQuat { float x, y, z, w; } MflQuat;

/* rwp_sphere_0056c1c0: the shape's vtable slot 3 returns the shape radius and fills `centre` (local space);
   unless the shape flag 2 is set the quaternion q of the body frame is multiplied into the quaternion kept
   at a2 + 0xc (v = v * q) and *out receives the centre in world space. */
/* TIER2: unvalidated 2026-11-01 authority=diffemu */
float rwp_sphere_0056c1c0(int a1, int a2, float *out)
{
  float centre[3];
  MflQuat q, cr;      /* cr = cross(v.xyz, q.xyz) */
  volatile MflQuat r;      /* the original keeps r.x and the z cross term in float frame slots */
  float *p;      /* v = quaternion at a2 + 0xc: p[0] = x, then y z w at a2 + 0x10 .. 0x18 */
#define VX p[0]
#define VY (*(float *)(a2 + 0x10))
#define VZ (*(float *)(a2 + 0x14))
#define VW (*(float *)(a2 + 0x18))
  float radius;
  float cx, cy, y1, z1;

  radius = ((float (*)())*(int *)(*(int *)(a1 + 0x5c) + 0xc))(a1, a2, centre);
  p = (float *)(a2 + 0xc);
  if (!(*(int *)(*(int *)(a1 + 0x5c) + 0x40) & 2)) {
    RtQuatConvertFromMatrix(&q, a1);
    r.w = VW * q.w - ((VY * q.y + VX * q.x) + VZ * q.z);
    cr.x = VZ * q.y - VY * q.z;
    cr.y = VX * q.z - VZ * q.x;
    r.z = VY * q.x - VX * q.y;
    r.x = VX * q.w + cr.x;
    y1 = VY * q.w + cr.y;
    z1 = VZ * q.w + r.z;
    r.x = VW * q.x + r.x;
    r.y = VW * q.y + y1;
    r.z = VW * q.z + z1;
    VX = r.x; VY = r.y; VZ = r.z; VW = r.w;
    cx = (centre[2] * *(float *)(a1 + 0x20) + centre[1] * *(float *)(a1 + 0x10)) + centre[0] * *(float *)(a1);
    cy = (centre[2] * *(float *)(a1 + 0x24) + centre[1] * *(float *)(a1 + 0x14)) + centre[0] * *(float *)(a1 + 0x4);
    centre[2] = (centre[2] * *(float *)(a1 + 0x28) + centre[1] * *(float *)(a1 + 0x18)) + centre[0] * *(float *)(a1 + 0x8);
    out[0] = *(float *)(a1 + 0x30) + cx;
    out[1] = *(float *)(a1 + 0x34) + cy;
    out[2] = *(float *)(a1 + 0x38) + centre[2];
  }
  return radius;
}
