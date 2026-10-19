// cluster-entries: triangle_normal_cross_array
/* Installed cluster triangle_normal_cross_array (tools/cluster_install.py from output/cluster_pilot11/triangle_normal_cross_array.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* triangle_normal_cross_array */

/* "normal of the triangle given as 9 floats" */

void triangle_normal_cross_array_n(float *in_eax, RwV3d *in_ecx, int param_3)

{
  float e0 = in_eax[3] - in_eax[0];
  float e1 = in_eax[4] - in_eax[1];
  float e2 = in_eax[5] - in_eax[2];
  float d2x = in_eax[6] - in_eax[0];
  float d2y = in_eax[7] - in_eax[1];
  float d2z = in_eax[8] - in_eax[2];

  in_ecx->x = d2z * e1 - d2y * e2;
  in_ecx->y = d2x * e2 - d2z * e0;
  in_ecx->z = d2y * e0 - d2x * e1;
  if (param_3 != 0) {
    RwV3dNormalize(in_ecx,in_ecx);
  }
  return;
}

/* entry thunk at the original address of triangle_normal_cross_array: register inputs (eax,ecx,s0) -> natural C triangle_normal_cross_array_n; ret 0 */
__declspec(naked) void triangle_normal_cross_array(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        push eax
        call triangle_normal_cross_array_n
        add esp, 12
        ret
    }
}
