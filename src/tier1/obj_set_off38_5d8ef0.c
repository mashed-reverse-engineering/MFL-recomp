#include "mfl_tier2.h"
/* obj_set_off38_5d8ef0 */

/* setter of field +0x38 */

char *obj_set_off38_5d8ef0(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x38) = *param_3;
  return param_1;
}
