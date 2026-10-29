#include "mfl_tier2.h"
/* gap_5bac10 */

/* no context */

undefined4 gap_5bac10(int *param_1,undefined4 *param_2)

{
  if (*param_1 == param_2[1]) {
    *param_2 = (undefined4)param_1;
    return 0;
  }
  return 1;
}
