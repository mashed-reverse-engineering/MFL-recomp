#include "mfl_tier2.h"
/* gap_5b7640 */

/* adds param_1 to *param_2 and returns the previous value (the original leaves it in EAX; a caller or the game hangs without it) */

int gap_5b7640(int param_1,int *param_2)

{
  int uVar1;

  uVar1 = *param_2;
  *param_2 = param_1 + uVar1;
  return uVar1;
}
