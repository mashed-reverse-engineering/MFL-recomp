#include "mfl_tier2.h"
/* gap_5cf5b0 */

/* no context */

undefined4 __stdcall gap_5cf5b0(char *param_1, uint param_2)

{
  if (param_2 > *(uint *)(param_1 + 0xc)) {
    return 0x80070057;
  }
  *(uint *)(param_1 + 0xc) = param_2;
  return 0;
}
