#include "mfl_tier2.h"
/*00429c20*/ extern int vehicle_slot_set();
/*00429cd0*/ extern undefined4 __cdecl load_copter_model(char *p0);
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/* copters_load */

/* "loads the copter models (SetCopter) and binds their general animations" */

void copters_load(void)

{
  int iVar1;
  char *_Str1;
  
  iVar1 = 0;
  if (0 < g_course_counts_data.copter_count) {
    _Str1 = g_course_files_data.copter_files;
    do {
      load_copter_model(_Str1);
      vehicle_slot_set(iVar1,g_course_world.general_anims[g_course_files_data.copter_ids[iVar1]]);
      iVar1 = iVar1 + 1;
      _Str1 = _Str1 + 0x40;
    } while (iVar1 < g_course_counts_data.copter_count);
  }
  return;
}



