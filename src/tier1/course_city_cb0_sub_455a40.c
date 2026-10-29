#include "mfl_tier2.h"
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*006a2150*/ extern undefined4 g_course_city_6a2150;
/* course_city_cb0_sub_455a40 */

/* single caller course_city_cb0 */

void course_city_cb0_sub_455a40(char *param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(param_1 + 0xc110 + param_2 * 4))));
  g_course_city_6a2150 =
       object_frames_create_with_matrices
                 (uVar1,*(undefined4 *)(param_1 + 0xc000 + param_3 * 4),param_3 * 0xc00 + param_1);
  return;
}
