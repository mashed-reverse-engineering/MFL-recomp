#include "mfl_tier2.h"
/*00405c80*/ extern undefined __cdecl clump_for_all_atomics_with_camera_matrix(struct RpClump *p0);
/*00405cd0*/ extern undefined __cdecl clump_clear_atomic_plugin_flag_1(struct RpClump *p0);
/*00409eb0*/ extern double player_rank_key(int p0);
/*0047a830*/ extern int car_is_active();
/*00668c18*/ extern struct CourseWorld g_course_world;

#define MAX_CARS (4)
#define RANK_KEY_NONE (0xffffffff)
#define RANK_KEY_SLOT_BASE (9)
#define CLUMP_COUNT (0x80)
#define CLUMP_RANGE_HI_OFFSET (0x2dc)
#define CLUMP_RANGE_STRIDE (5)


/* course_world_uva_dicts_set_ranks */

/* "stores the rank key of each active car in g_course_world.uva_dicts and handles the lists at
   0xC110" */

/* Stores the rank key of each car in g_course_world.uva_dicts (RANK_KEY_NONE for inactive cars),
   then walks the clump table: for each enabled entry it clears the atomic plugin flag of the
   clump and, when a car's rank key lies in the entry's [lo, hi] range, calls
   clump_for_all_atomics_with_camera_matrix on that clump (inferred).
   Parameters: course_world = the CourseWorld (clumps at 0xC110), clump_table = 5-int range
   entries of the clump table (flag, lo, hi, ...) (inferred). Returns nothing. */

void course_world_uva_dicts_set_ranks(char *course_world,char *clump_table)

{
  int in_range;
  int scratch;
  uint rank_key;
  undefined4 *clump_slot;
  int counter;
  int *clump_range_hi;

  /* loop 1: rank key per car slot, RANK_KEY_NONE unless car_is_active(car) == 1 */
  counter = 0;
  do {
    g_course_world.uva_dicts[counter + RANK_KEY_SLOT_BASE] = RANK_KEY_NONE;
    scratch = car_is_active(counter);
    if (scratch == 1) {
      rank_key = (int)player_rank_key(counter);
      g_course_world.uva_dicts[counter + RANK_KEY_SLOT_BASE] = rank_key;
    }
    counter = counter + 1;
  } while (counter < MAX_CARS);
  clump_slot = (undefined4 *)((struct CourseWorld *)course_world)->clumps;
  clump_range_hi = (int *)(clump_table + CLUMP_RANGE_HI_OFFSET);
  /* loop 2: one iteration per clump (CLUMP_COUNT down to 0); skips entries whose flag
     (clump_range_hi[-2]) is 0 */
  counter = CLUMP_COUNT;
  do {
    if (clump_range_hi[-2] != 0) {
      in_range = 0;
      clump_clear_atomic_plugin_flag_1((RpClump *)*clump_slot);
      scratch = clump_range_hi[-1];
      /* (inferred) in_range = some car's rank key is within [scratch (lo), *clump_range_hi (hi)] */
      if (((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 0]) >= scratch) &&
         ((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 0]) <= *clump_range_hi)) {
        in_range = 1;
      }
      if (((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 1]) >= scratch) &&
         ((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 1]) <= *clump_range_hi)) {
        in_range = 1;
      }
      if (((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 2]) >= scratch) &&
         ((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 2]) <= *clump_range_hi)) {
        in_range = 1;
      }
      if ((((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 3]) >= scratch) &&
          ((int)(g_course_world.uva_dicts[RANK_KEY_SLOT_BASE + 3]) <= *clump_range_hi)) || (in_range)) {
        clump_for_all_atomics_with_camera_matrix((RpClump *)*clump_slot);
      }
    }
    clump_slot = clump_slot + 1;
    clump_range_hi = clump_range_hi + CLUMP_RANGE_STRIDE;
    counter = counter + -1;
  } while (counter != 0);
  return;
}
