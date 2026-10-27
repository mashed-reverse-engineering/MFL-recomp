// cluster-entries: planes_test_bbox
/* Installed cluster planes_test_bbox (tools/cluster_install.py from output/cluster_pilot10/planes_test_bbox.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004880b0*/ extern int __cdecl bbox_plane_corner_test(undefined4 p0, undefined4 p1);
/* planes_test_bbox */

/* "(bbox): true when the bbox is inside all frustum planes (loop of bbox_plane_corner_test)" */

undefined4 planes_test_bbox_n(int in_ebx, int in_eax, undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < in_ebx) {
    do {
      iVar1 = bbox_plane_corner_test(in_eax,param_3);
      if (iVar1 == 0) {
        return 0;
      }
      iVar2 = iVar2 + 1;
      in_eax = in_eax + 0x10;
    } while (iVar2 < in_ebx);
  }
  return 1;
}

/* entry thunk at the original address of planes_test_bbox: register inputs (ebx,eax,s0) -> natural C planes_test_bbox_n; ret 0 */
__declspec(naked) void planes_test_bbox(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push ebx
        call planes_test_bbox_n
        add esp, 12
        ret
    }
}
