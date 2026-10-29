#include "mfl_tier2.h"
/* obj_set_off30_5d8eb0 */

/* setter of field +0x30 */

char *obj_set_off30_5d8eb0(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x30) = *param_3;
  return param_1;
}
