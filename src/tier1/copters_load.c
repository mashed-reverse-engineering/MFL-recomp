#include "mfl_tier2.h"
/*00429c20*/ extern int vehicle_slot_set();
/*00429cd0*/ extern undefined4 __cdecl load_copter_model(char *p0);
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
#define COPTER_FILE_STRIDE (0x40) /* bytes per file name in copter_files */
/* copters_load */

/* "loads the copter models (SetCopter) and binds their general animations" */

void copters_load(void)

{
  int copter_index;
  char *file_name;
  
  copter_index = 0;
  if (0 < g_course_counts_data.copter_count) {
    file_name = g_course_files_data.copter_files;
    do {
      load_copter_model(file_name);
      vehicle_slot_set(copter_index,g_course_world.general_anims[g_course_files_data.copter_ids[copter_index]]);
      copter_index = copter_index + 1;
      file_name = file_name + COPTER_FILE_STRIDE;
    } while (copter_index < g_course_counts_data.copter_count);
  }
  return;
}



