// cluster-entries: matrix_is_valid_small
/* Installed cluster matrix_is_valid_small (tools/cluster_install.py from output/cluster_pilot_kb/matrix_is_valid_small.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004821a0*/ extern int __cdecl vec3_is_valid_small(undefined4 p0);
/* matrix_is_valid_small */

/* "vec3_is_valid_small for the four rows (+0x20,+0x10,+0,+0x30)" */

bool matrix_is_valid_small(int param_1, char *unused_2)

{
  int iVar1;
  
  iVar1 = vec3_is_valid_small(param_1 + 0x20);
  if (iVar1 != 0) {
    iVar1 = vec3_is_valid_small(param_1 + 0x10);
    if (iVar1 != 0) {
      iVar1 = vec3_is_valid_small(param_1);
      if (iVar1 != 0) {
        iVar1 = vec3_is_valid_small(param_1 + 0x30);
        return iVar1 != 0;
      }
    }
  }
  return false;
}

