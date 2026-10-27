// cluster-entries: array_find_index
/* Installed cluster array_find_index (tools/cluster_install.py from output/cluster_pilot_kd/array_find_index.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* array_find_index */

/* "(value, array, count): index of value or -1" */

/* (inferred) cursor advances 10 ints (0x28 bytes) per element: finds the first element whose first int equals value */
int array_find_index(int value, int *cursor, int count)

{
  int i;
  
  i = 0;
  if (0 < count) {
    do {
      if (*cursor == value) {
        return i;
      }
      i = i + 1;
      cursor = cursor + 10;
    } while (i < count);
  }
  return -1;
}

