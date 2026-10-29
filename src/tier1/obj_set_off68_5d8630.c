#include "mfl_tier2.h"
/* obj_set_off68_5d8630 */

/* setter of field +0x68 */

char *obj_set_off68_5d8630(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x68) = *param_3;
  return param_1;
}
