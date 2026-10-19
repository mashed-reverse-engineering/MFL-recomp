#include "mfl_tier2.h"
/*004492b0*/ extern int lua_register_course_functions();
/*0048cf80*/ extern int lua_run_script_file();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006f2af8*/ extern struct CourseLap *g_course_lap;
/*006f2afc*/ extern undefined4 g_lap_line_count;
/* Bytes of CourseLap cleared on entry (inferred: smaller than sizeof(CourseLap) = 0x16300) */
#define LAP_DATA_CLEAR_SIZE (0x16264)
/* lapdata_script_run */

/* "(file name, CourseLap): runs LAPDATA.LUA"
 * Points g_course_lap at lap_data, resets the lap line count, clears the lap data block,
 * then runs file_name (LAPDATA.LUA) with the course functions registered.
 * file_name: script file name. lap_data: CourseLap block to fill.
 * Returns lap_data (inferred: the same pointer that was passed in). */

CourseLap * lapdata_script_run(undefined4 file_name,CourseLap *lap_data)

{
  CourseLap *result;

  /* keep the input pointer for the return value */
  result = lap_data;
  g_course_lap = lap_data;
  g_lap_line_count = 0;
  /* clear the lap data before the script fills it (inferred) */
  mem_zero(lap_data,LAP_DATA_CLEAR_SIZE);
  /* run the script; &lap_data is passed so the callee sees the block (inferred) */
  lua_run_script_file(file_name,&lap_data,lua_register_course_functions);
  return result;
}


