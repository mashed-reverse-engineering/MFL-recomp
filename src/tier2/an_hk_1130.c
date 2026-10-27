#include "mfl_tier2.h"
/* obj_set_off78_5b6ae0 */

/* setter of field +0x78 */

char *obj_set_off78_5b6ae0(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x78) = *param_3;
  return param_1;
}


