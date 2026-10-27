// cluster-entries: triangle_normal
/* Installed cluster triangle_normal (tools/cluster_install.py from output/cluster_pilot_hb1/triangle_normal.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/* triangle_normal */

/* "(p0, p1, p2 -> out): normalised cross product" */

void triangle_normal_n(float *in_eax, float *in_edx, float *in_ecx, float *in_esi)

{
  float ax = in_eax[0] - in_edx[0];
  float ay = in_eax[1] - in_edx[1];
  float az = in_eax[2] - in_edx[2];
  float bx = in_eax[0] - in_ecx[0];
  float by = in_eax[1] - in_ecx[1];
  float bz = in_eax[2] - in_ecx[2];
  float nx = bz * ay - by * az;
  float ny = bx * az - bz * ax;
  float nz = by * ax - bx * ay;
  float num = nx * nx + ny * ny + nz * nz;
  RwReal len;

  if ((k_f_0 <= num) && (len = _rwSqrt(num), len != k_f_0)) {
    num = float_1_0 / len;
    *in_esi = nx * num;
    in_esi[1] = ny * num;
    in_esi[2] = num * nz;
  }
  return;
}

/* entry thunk at the original address of triangle_normal: register inputs (eax,edx,ecx,esi) -> natural C triangle_normal_n; ret 0 */
__declspec(naked) void triangle_normal(void)
{
    __asm {
        push esi
        push ecx
        push edx
        push eax
        call triangle_normal_n
        add esp, 16
        ret
    }
}
