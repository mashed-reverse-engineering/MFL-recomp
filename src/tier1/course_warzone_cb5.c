#include "mfl_tier2.h"
/*0045c790*/ extern int course_atomics_destroy_3();
/*0045d890*/ extern int course_effect_destroy();
/*00484ad0*/ extern int pool_list_destroy();
/*0048e280*/ extern int ptank_atomic_destroy();
/*004a5da0*/ extern int object_destroy_frames();
/*0053e3b0*/ extern void FUN_0053e3b0(undefined4 p0);
/*006a27a0*/ extern undefined4 g_course_warzone_6a27a0;
/*006a2998*/ extern undefined4 g_course_warzone_6a2998;
/*006a299c*/ extern undefined4 g_course_warzone_6a299c;
/*006a29a0*/ extern undefined4 g_course_warzone_6a29a0;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*008bf044*/ extern undefined4 g_course_warzone_8bf044;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*008bf080*/ extern char g_course_8bf080[];
/* course_warzone_cb5: tears down the warzone course's runtime objects (inferred from the callees).
 * Takes no parameters and returns nothing. */
void course_warzone_cb5(void)
{
  undefined4 *effect_slot;

  effect_slot = &g_course_warzone_8bf060;
  /* walk the effect table from g_course_warzone_8bf060 up to g_course_8bf080, destroying each entry (inferred) */
  do {
    course_effect_destroy(*effect_slot);
    effect_slot = effect_slot + 1;
  } while ((LONG_PTR)effect_slot < ((LONG_PTR)&g_course_8bf080));
  ptank_atomic_destroy();
  FUN_0053e3b0(g_course_warzone_8bf044);
  pool_list_destroy(g_course_warzone_6a27a0);
  object_destroy_frames(g_course_warzone_6a2998);
  object_destroy_frames(g_course_warzone_6a299c);
  object_destroy_frames(g_course_warzone_6a29a0);
  object_destroy_frames(g_course_warzone_6a29a4);
  course_atomics_destroy_3();
  return;
}
