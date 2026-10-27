#include "mfl_tier2.h"
/*00457070*/ extern undefined __cdecl spline_object_init(undefined4 *p0, struct RpSpline *p1, void *p2, undefined4 p3, int p4);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*004f6bf0*/ extern void * FUN_004f6bf0(undefined4 p0);
/*0060d8fc*/ extern undefined4 g_course_spline_60d8fc;
/*008c5500*/ extern undefined g_course_spline_8c5500;
#define SPLINE_OBJECT_COUNT (3)
#define SPLINE_OBJECT_STRIDE_DWORDS (0x16) /* 0x58 bytes per spline object */
#define WORLD_RECORD_C118_OFFSET (0xc118) /* inferred: unknown pointer, see an_hk_245/562 */
#define STATE_WORLD_OFFSET (0xc9d4) /* RpWorld the clump is added to */
#define STATE_COURSE_SPLINE_OFFSET (0xc0b8) /* RpSpline passed to spline_object_init */
/* course_spline_objects_create */
/* "creates the 3 spline following objects from the clump of the world record" */
/* Creates the SPLINE_OBJECT_COUNT spline following objects of the course.
 * game_state: the game state block (fields at the STATE_*_OFFSET values).
 * For each object: fetch the clump (inferred: by id from game_state), add it to
 * the world if it is not in one yet, then initialise the spline object with it.
 * Returns nothing. */
void course_spline_objects_create(char *game_state)
{
  RpClump *clump;
  int clump_world;
  undefined4 *spline_object;
  int spline_index;

  /* loop over the spline objects, one per index */
  spline_index = 0;
  spline_object = (undefined4 *)&g_course_spline_8c5500;
  do {
    clump = (RpClump *)FUN_004f6bf0(*(undefined4 *)(game_state + WORLD_RECORD_C118_OFFSET));
    clump_world = clump_get_world(clump);
    if (clump_world == 0) {
      RpWorldAddClump(*(RpWorld **)(game_state + STATE_WORLD_OFFSET),clump);
    }
    spline_object_init(spline_object,*(RpSpline **)(game_state + STATE_COURSE_SPLINE_OFFSET),clump,
                       (&g_course_spline_60d8fc)[spline_index],spline_index);
    spline_index = spline_index + 1;
    spline_object = spline_object + SPLINE_OBJECT_STRIDE_DWORDS;
  } while (spline_index < SPLINE_OBJECT_COUNT);
  return;
}
