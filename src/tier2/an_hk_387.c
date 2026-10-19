#include "mfl_tier2.h"
/*004492b0*/ extern int lua_register_course_functions();
/*00488d60*/ extern int table_clear_eb8();
/*0048cf80*/ extern int lua_run_script_file();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*006f2af0*/ extern undefined4 g_course_script_6f2af0;
/*006f2af4*/ extern undefined4 g_sky_count;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0088a924*/ extern char g_course_script_88a924[];
#define COURSE_COUNTS_SIZE (0xd1c)
#define RWP_OBJECTS_BYTES (0x2200)
#define RWP_OBJECT_STRIDE_FLOATS (0x44)
/* course_script_run */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(file name, CourseFiles, course id): runs COURSE.LUA, which fills the CourseFiles / CourseCounts
   blocks" */

/* Resets the course state and runs COURSE.LUA for the current course.
   script_file_name: passed to lua_run_script_file as the script to run (inferred).
   course_files: receives the course file names (cleared first).
   course_counts: receives the per-course counts (zeroed, then defaults set).
   Returns course_files. */
CourseFiles * course_script_run(undefined4 script_file_name,CourseFiles *course_files,CourseCounts *course_counts)

{
  CourseFiles *files;
  CourseCounts *counts;
  float *rwp_field;
  
  counts = course_counts;
  files = course_files;
  g_course_files = course_files;
  g_course_counts = course_counts;
  g_course_script_6f2af0 = 0;
  g_sky_count = 0;
  /* clear the file table and the counts block, then set the defaults:
     shininess range [0.0, 1.0], shine and shadow scale 1.0 */
  table_clear_eb8(course_files);
  mem_zero(counts,COURSE_COUNTS_SIZE);
  counts->vehicle_shininess_range[0] = 0.0;
  counts->vehicle_shininess_range[1] = 1.0;
  counts->shine_scale = 1.0;
  counts->shadow_scale = 1.0;
  mem_zero(g_rwp_objects,RWP_OBJECTS_BYTES);
  /* default every RWP object (32 entries, stride 0x110 bytes): shape 0 (unused),
     property_d 1.0, property_b and property_c 0.5, property_a 0.01, centre of mass 0 (inferred from offsets) */
  rwp_field = &g_rwp_objects[0].property_d;
  do {
    ((RwpObjectDesc *)(rwp_field + -0x39))->shape = 0;
    rwp_field[4] = 0.5;
    *rwp_field = 1.0;
    rwp_field[1] = 0.0;
    rwp_field[2] = 0.0;
    rwp_field[3] = 0.0;
    rwp_field[5] = 0.5;
    rwp_field[6] = 0.01;
    rwp_field = rwp_field + RWP_OBJECT_STRIDE_FLOATS;
  } while ((LONG_PTR)rwp_field < ((LONG_PTR)&g_course_script_88a924));
  /* run COURSE.LUA; its Lua calls fill course_files and course_counts */
  lua_run_script_file(script_file_name,&course_files,lua_register_course_functions);
  return files;
}


