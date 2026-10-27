#include "mfl_tier2.h"
/*004068f0*/ extern int __cdecl clump_for_all_atomics_cb_4068a0(struct RpClump *p0, void *p1);
/*004f6bf0*/ extern int FUN_004f6bf0(undefined4 p0);
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a26d0*/ extern undefined4 g_course_clumps_6a26d0;
/*006a26d4*/ extern undefined4 g_course_clumps_6a26d4;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2730*/ extern undefined4 g_course_clumps_6a2730;
/*006a2734*/ extern undefined4 g_course_clumps_6a2734;
/* course_helper_45b670 */

/* callers course_sands_cb0,course_sandsnow_cb0_sub_4584b0 */

void course_helper_45b670(char *param_1)

{
  g_course_clumps_6a26cc = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  g_course_clumps_6a26d0 = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  g_course_clumps_6a26d4 = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  g_course_clumps_6a272c = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  g_course_clumps_6a2730 = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  g_course_clumps_6a2734 = FUN_004f6bf0(*(undefined4 *)(param_1 + 0xc138));
  clump_for_all_atomics_cb_4068a0(*(RpClump **)(param_1 + 0xc158),*(void **)(param_1 + 0xc9d0));
  clump_for_all_atomics_cb_4068a0(*(RpClump **)(param_1 + 0xc15c),*(void **)(param_1 + 0xc9d0));
  clump_for_all_atomics_cb_4068a0(*(RpClump **)(param_1 + 0xc160),*(void **)(param_1 + 0xc9d0));
  clump_for_all_atomics_cb_4068a0(*(RpClump **)(param_1 + 0xc168),*(void **)(param_1 + 0xc9d0));
  return;
}


