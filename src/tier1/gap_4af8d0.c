#include "mfl_tier2.h"
/* gap_4af8d0 */

/* no context */

undefined4 __stdcall gap_4af8d0(char *param_1, undefined4 *param_2)

{
  if (param_2 == (undefined4 *)0x0) {
    return 0x80004003;
  }
  *param_2 = *(undefined4 *)(param_1 + 0x10);
  return 0;
}
