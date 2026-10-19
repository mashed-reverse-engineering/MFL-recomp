#include "mfl_tier2.h"
/*00484960*/ extern int SUB_00484960();
/*00484b20*/ extern int SUB_00484b20();
/*008c4b08*/ extern undefined4 g_course_forest_8c4b08;
/* course_forest_cb1 */

/* the original ends in a tail jump to SUB_00484960 (clump, SUB_00484b20 result): Ghidra inlined the target */
int course_forest_cb1(int param_1,undefined4 param_2)

{
  undefined4 res;

  res = SUB_00484b20(g_course_forest_8c4b08,param_2);
  return SUB_00484960(*(undefined4 *)(param_1 + 0xc110),res);
}
