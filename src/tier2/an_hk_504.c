#include "mfl_tier2.h"
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*006a07c4*/ extern undefined4 g_course_6a07c4;
/*006a07c8*/ extern undefined4 g_course_6a07c8;
/*006a07cc*/ extern undefined4 g_course_6a07cc;
/*006a07d0*/ extern undefined4 g_course_6a07d0;
/*008c5728*/ extern undefined4 g_course_dump_8c5728;
/* course_dump_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Course dump callback 0 (inferred: gathers course/track data for the course dump).
 * course: the CourseWorld block (clump slots, matrix lists, lights) of the loaded course.
 * Copies the word at +0xc110 into g_course_dump_8c5728. Then, for four clumps stored at
 * +0xc154, +0xc158, +0xc15c and +0xc128, takes each clump's first atomic and passes it to
 * func_0x004a5d50 together with a per-slot word and a block address inside course;
 * the result goes to g_course_6a07c4, g_course_6a07c8, g_course_6a07cc and g_course_6a07d0.
 * Returns nothing. */
void course_dump_cb0(struct CourseWorld *course)

{
  undefined4 atomic;

  g_course_dump_8c5728 = course->clumps[0];
  /* One block per clump slot: first atomic of the clump -> func_0x004a5d50 -> global. */
  atomic = clump_first_atomic(course->clumps[17]);
  g_course_6a07c4 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[4],COURSE_MTS_LIST(course, 4),
                       course->ambient_light,course->directional_light);
  atomic = clump_first_atomic(course->clumps[18]);
  g_course_6a07c8 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[5],COURSE_MTS_LIST(course, 5),
                       course->ambient_light,course->directional_light);
  atomic = clump_first_atomic(course->clumps[19]);
  g_course_6a07cc =
       func_0x004a5d50(atomic,course->mts_matrix_counts[6],COURSE_MTS_LIST(course, 6),
                       course->ambient_light,course->directional_light);
  atomic = clump_first_atomic(course->clumps[6]);
  g_course_6a07d0 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[0],course,
                       course->ambient_light,course->directional_light);
  return;
}


