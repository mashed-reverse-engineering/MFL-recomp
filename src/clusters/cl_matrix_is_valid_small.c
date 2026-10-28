// cluster-entries: matrix_is_valid_small
/* Installed cluster matrix_is_valid_small (tools/cluster_install.py from output/cluster_pilot_kb/matrix_is_valid_small.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004821a0*/ extern int __cdecl vec3_is_valid_small(undefined4 p0);
/* matrix_is_valid_small */

/* "vec3_is_valid_small for the four rows (+0x20,+0x10,+0,+0x30)" */

bool matrix_is_valid_small(int matrix, char *unused_2)

{
  int valid;
  
  valid = vec3_is_valid_small(matrix + 0x20);
  if (valid != 0) {
    valid = vec3_is_valid_small(matrix + 0x10);
    if (valid != 0) {
      valid = vec3_is_valid_small(matrix);
      if (valid != 0) {
        valid = vec3_is_valid_small(matrix + 0x30);
        return valid != 0;
      }
    }
  }
  return false;
}

