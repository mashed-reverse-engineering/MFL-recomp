#include "mfl_tier2.h"
/*005bf4e0*/ extern int __cdecl FUN_005bf4e0(int a1, int a2, void *a3, void *a4);
/* gap_5c3020 */

/* no context */

/* Computes (src_count * entry bytes + result of FUN_005bf4e0) / divisor for table entry `index` of obj's table (inferred). */
uint gap_5c3020(int obj,char *src,int index)

{
  int entry;
  int src_count;
  int bytes_per_entry;
  uint divisor;
  
  bytes_per_entry = *(int *)(*(char **)(obj + 0x118) + 0x18);
  entry = bytes_per_entry + index * 0x28;
  src_count = *(int *)(src + 4);
  bytes_per_entry = (*(uint *)(entry + 0x20) / (uint)*(ushort *)(bytes_per_entry + 0x1a + index * 0x28)) *
          *(int *)(entry + 0xc);
  FUN_005bf4e0(*(undefined4 *)(src + 8),bytes_per_entry,&obj,&index);
  divisor = 1;
  if ((*(byte *)(*(char **)(entry + 0x14) + 0x18) & 4) == 0) {
    divisor = (uint)*(byte *)(*(char **)(entry + 0x14) + 0xd);
  }
  return (uint)(src_count * bytes_per_entry + obj) / divisor;
}


