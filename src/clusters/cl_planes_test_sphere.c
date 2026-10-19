// cluster-entries: planes_test_sphere
/* Installed cluster planes_test_sphere (tools/cluster_install.py from output/cluster_pilot10/planes_test_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00488140*/ extern int __cdecl sphere_plane_side(undefined4 p0, undefined4 p1);
/* planes_test_sphere */

/* WARNING: Unknown calling convention */
/* "(sphere): loop of sphere_plane_side over the planes" */

undefined4 planes_test_sphere_n(int in_ebx, int in_eax, undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < in_ebx) {
    do {
      iVar1 = sphere_plane_side(in_eax,param_3);
      if (iVar1 != 2) {
        return 0;
      }
      iVar2 = iVar2 + 1;
      in_eax = in_eax + 0x10;
    } while (iVar2 < in_ebx);
  }
  return 1;
}

/* entry thunk at the original address of planes_test_sphere: register inputs (ebx,eax,s0) -> natural C planes_test_sphere_n; ret 0 */
__declspec(naked) void planes_test_sphere(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push ebx
        call planes_test_sphere_n
        add esp, 12
        ret
    }
}
