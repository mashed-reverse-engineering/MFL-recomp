#include "mfl_tier2.h"
/* course_helper_484b20 */

/* callers course_forest_cb1,course_keys_cb1,course_nt1_cb1,course_nt1_s_cb1 */

undefined4 course_helper_484b20(char *param_1,int param_2)

{
  int iVar1;
  
  param_2 = *(int *)(param_1 + 0xc) + param_2;
  *(int *)(param_1 + 0xc) = param_2;
  if (299 < param_2) {
    iVar1 = **(int **)(param_1 + 4);
    *(int *)(param_1 + 4) = iVar1;
    *(undefined4 *)(param_1 + 0xc) = 0;
    return *(undefined4 *)(iVar1 + 4);
  }
  return *(undefined4 *)(*(char **)(param_1 + 4) + 4);
}


