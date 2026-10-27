#include "mfl_tier2.h"
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*006a2668*/ extern undefined4 g_data_6a2668;
/*006a266c*/ extern undefined4 g_data_6a266c;
/*006a2670*/ extern undefined4 g_data_6a2670;
/*006a2674*/ extern undefined4 g_data_6a2674;
/*006a2678*/ extern undefined4 g_data_6a2678;
/* course_forest_cb0_sub_458e70 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_forest_cb0 */

void course_forest_cb0_sub_458e70(char *param_1)

{
  undefined4 uVar1;
  
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(param_1 + 0xc118))));
  g_data_6a2668 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc000),param_1);
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(param_1 + 0xc11c))));
  g_data_6a2674 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc004),param_1 + 0xc00);
  g_data_6a2678 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc018),param_1 + 0x4800);
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(char **)(param_1 + 0xc124))));
  g_data_6a266c =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc00c),param_1 + 0x2400);
  g_data_6a2670 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc014),param_1 + 0x3c00);
  return;
}


