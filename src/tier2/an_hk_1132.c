#include "mfl_tier2.h"
/* obj_set_off7c_5b6b00 */

/* setter of field +0x7c */

char *obj_set_off7c_5b6b00(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x7c) = *param_3;
  return param_1;
}


