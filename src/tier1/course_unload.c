#include "mfl_tier2.h"

#define SPLIT_SECTOR_UNKNOWN 8

/*00408900*/ extern int fx_objects_destroy();
/*004219f0*/ extern undefined __cdecl course_call_cb5(void *p0);
/*00421aa0*/ extern int course_select();
/*00421b10*/ extern int course_has_cb5();
/*0042a110*/ extern int course_destroy_extra_clumps();
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00473a50*/ extern int course_unload_atomics_6ae24c();
/*00474880*/ extern int course_objects_destroy_atomics();
/*00475510*/ extern int course_grid_buffers_free();
/*00488da0*/ extern undefined __cdecl course_destroy_world(struct CourseWorld *p0, struct CourseCounts *p1);
/*00490ef0*/ extern int rwp_world_flush();
/*00490f30*/ extern int rwp_world_destroy();
/*004a0850*/ extern int fx_buffer_free();
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;


/* course_unload */

/* Unloads the active course and cleans up all associated world, physics, audio, and FX resources. */
/* "(course id): cb5, then course_destroy and the per-course resets" */

void __cdecl course_unload(int course_id)
{
  int has_cb5;
  
  course_objects_destroy_atomics();
  course_unload_atomics_6ae24c(&g_course_world);
  course_select(course_id);
  rwp_world_flush();
  has_cb5 = course_has_cb5();
  if (has_cb5 != 0) {
    /* Invoke course callback 5 if registered */
    course_call_cb5(&g_course_world);
  }
  fx_buffer_free();
  course_destroy_world(&g_course_world,&g_course_counts_data);
  course_destroy_extra_clumps();
  fx_objects_destroy();
  sound_stop_all_voices_and_streams();
  rwp_world_destroy();
  course_grid_buffers_free();
  /* Reset split sector tracker (specific index purpose unknown) */
  g_course_lap_data.split_sectors[SPLIT_SECTOR_UNKNOWN] = 0;
  return;
}
