#include "mfl_tier2.h"
extern int _rpUVAnimDictSchema;
/*00484910*/ extern undefined __cdecl clump_for_all_materials_uv_anim(struct RpClump *p0, undefined4 p1);
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
#define COURSE_UV_DICT_OFFSET (0xca18)
#define CLUMP_TABLE_SMALL_OFFSET (0xc0f0)
#define CLUMP_TABLE_SMALL_COUNT (8)
#define CLUMP_TABLE_LARGE_OFFSET (0xc110)
#define CLUMP_TABLE_LARGE_COUNT (0x80)
#define ACTIVE_FLAG_WORD_INDEX (0x88)
/* course_uv_anims_update */

/* "updates the UV animations of the course world with the tick time (FUN_00569270 'UVAnim')" */

/* Advances the UV animations of every clump in the course's two clump tables for this tick.
 * course_state: course world state; fields are read at byte offsets (defines above), not via a struct yet (inferred).
 * tick_time: tick time in ms (inferred); negative values are read as unsigned (+2^32), then scaled by 1/3000.
 * The scaled float is passed to clump_for_all_materials_uv_anim as its raw bits (int). Returns nothing.
 * A slot is processed only when its active flag (ACTIVE_FLAG_WORD_INDEX words past the slot) is nonzero and the clump pointer is not NULL. */

void course_uv_anims_update(char *course_state,int tick_time)

{
  float anim_time;
  undefined4 *clump_slot;
  int slots_left;

  /* tick_time as float, unsigned wrap for negatives, then scaled */
  anim_time = (float)tick_time;
  if (tick_time < 0) {
    anim_time = anim_time + k_f_4_2949673e9;
  }
  anim_time = anim_time * k_f_3_3333333en4;
  RtDictSchemaSetCurrentDict(&_rpUVAnimDictSchema,*(undefined4 *)(course_state + COURSE_UV_DICT_OFFSET));
  /* first loop: large clump table (CLUMP_TABLE_LARGE_COUNT slots, inferred name) */
  clump_slot = (undefined4 *)(course_state + CLUMP_TABLE_LARGE_OFFSET);
  slots_left = CLUMP_TABLE_LARGE_COUNT;
  do {
    if ((clump_slot[ACTIVE_FLAG_WORD_INDEX] != 0) && ((RpClump *)*clump_slot != (RpClump *)0x0)) {
      clump_for_all_materials_uv_anim((RpClump *)*clump_slot,*(int *)&anim_time);
    }
    clump_slot = clump_slot + 1;
    slots_left = slots_left + -1;
  } while (slots_left != 0);
  /* second loop: small clump table (CLUMP_TABLE_SMALL_COUNT slots, inferred name) */
  clump_slot = (undefined4 *)(course_state + CLUMP_TABLE_SMALL_OFFSET);
  slots_left = CLUMP_TABLE_SMALL_COUNT;
  do {
    if ((clump_slot[ACTIVE_FLAG_WORD_INDEX] != 0) && ((RpClump *)*clump_slot != (RpClump *)0x0)) {
      clump_for_all_materials_uv_anim((RpClump *)*clump_slot,*(int *)&anim_time);
    }
    clump_slot = clump_slot + 1;
    slots_left = slots_left + -1;
  } while (slots_left != 0);
  return;
}


