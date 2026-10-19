// cluster-entries: hudpanel_marker_calc_pixel_ray
/* Installed cluster hudpanel_marker_calc_pixel_ray (tools/cluster_install.py from output/cluster_pilot11/hudpanel_marker_calc_pixel_ray.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005698c0*/ extern undefined __cdecl RwCameraCalcPixelRay(undefined4 p0, undefined4 p1, undefined4 p2);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* hudpanel_marker_calc_pixel_ray */

/* WARNING: Unknown calling convention */
/* "RwCameraCalcPixelRay for a marker of type 2" */

void hudpanel_marker_calc_pixel_ray_n(int in_eax)

{
  if (*(int *)(in_eax + 0x34) == 2) {
    RwCameraCalcPixelRay(RwCameraGetCurrentCamera(),in_eax + 0x1c,in_eax + 0x14);
  }
  return;
}

/* entry thunk at the original address of hudpanel_marker_calc_pixel_ray: register inputs (eax) -> natural C hudpanel_marker_calc_pixel_ray_n; ret 0 */
__declspec(naked) void hudpanel_marker_calc_pixel_ray(void)
{
    __asm {
        push eax
        call hudpanel_marker_calc_pixel_ray_n
        add esp, 4
        ret
    }
}
