#include "mfl_tier2.h"
/* course_neustein_cb0_sub_56e990 */

/* single caller course_neustein_cb0_sub_454460 */

char *course_neustein_cb0_sub_56e990(char *param_1,undefined4 *param_2)

{
  *(undefined4 *)(param_1 + 0x10) = *param_2;
  *(undefined4 *)(param_1 + 0x14) = param_2[1];
  *(undefined4 *)(param_1 + 0x18) = param_2[2];
  return param_1;
}


