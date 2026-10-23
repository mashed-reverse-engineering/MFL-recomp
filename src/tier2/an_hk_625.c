#include "mfl_tier2.h"
/*00425ce0*/ extern undefined __cdecl vehicle_prepare_clear_660568(undefined4 p0);
/*00426580*/ extern int vehicle_render_state_clear();
/*0042a450*/ extern int course_is_outdoor();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*0054d670*/ extern void FUN_0054d670(undefined4 p0, undefined4 p1);
/*0054d6a0*/ extern int __cdecl FUN_0054d6a0(int a1);
/*0064d88c*/ extern undefined4 g_game_64d88c;
/*00659d08*/ extern undefined4 g_game_659d08;
/*00659d50*/ extern undefined g_game_659d50;
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/*0065a9c8*/ extern undefined4 g_game_65a9c8;

#define VEHICLE_PART_SCAN_COUNT (0x80) /* loop bound over part_frames (inferred) */
#define PART_PREPARE_LO_EXCL (0x3a) /* prepared parts start after this index */
#define PART_PREPARE_HI_EXCL (0x3f) /* ... and end before this one (0x3b..0x3e) */
#define PART_PREPARE_EXTRA (0x40) /* one more prepared part after the range */
#define PART_FRAME_OBJECT_OFFSET (0x18) /* object pointer inside a part frame (inferred) */
#define VEHICLE_RECORD_SIZE (0x2ac) /* sizeof(struct VehicleRecord) */

/* vehicle_prepare_parts */

/* Tears down vehicle slot vehicle_index (0..3): releases the prepared parts 0x3B..0x3E and 0x40
   of its part_frames (inferred), destroys its clump, zeroes its VehicleRecord, clears the render
   state for indoor or outdoor courses, frees the shared texture dictionary when g_game_65a9c8 == 1,
   and decrements the vehicle count g_game_65a9c8 (inferred). Returns nothing.
   (was: "(vehicle): walks the 0xAB part frames of the vehicle record and prepares parts 0x3B..0x3E /
   0x40" - stale, the loop walks 0x80 slots) */

void vehicle_prepare_parts(int vehicle_index)

{
  int part_frame;
  int loop_index;
  undefined4 part_object;

  /* walk every part_frames slot; release the prepared parts only */
  loop_index = 0;
  do {
    part_frame = g_vehicle_table[vehicle_index].part_frames[loop_index];
    if ((PART_PREPARE_LO_EXCL < loop_index) && (((loop_index < PART_PREPARE_HI_EXCL || (loop_index == PART_PREPARE_EXTRA)) && (part_frame != 0)))) {
      part_object = *(undefined4 *)(part_frame + PART_FRAME_OBJECT_OFFSET);
      FUN_0054d6a0(*(undefined4 *)(part_frame + PART_FRAME_OBJECT_OFFSET));
      FUN_0054d670(*(undefined4 *)(part_frame + PART_FRAME_OBJECT_OFFSET),part_object);
    }
    loop_index = loop_index + 1;
  } while (loop_index < VEHICLE_PART_SCAN_COUNT);
  /* destroy the vehicle's clump, then wipe its record */
  if ((RpClump *)g_vehicle_table[vehicle_index].frame != (RpClump *)0x0) {
    RpClumpDestroy((RpClump *)g_vehicle_table[vehicle_index].frame);
  }
  mem_zero(g_vehicle_table + vehicle_index,VEHICLE_RECORD_SIZE);
  /* render state clear; loop_index is reused here to hold course_is_outdoor()'s result */
  if (g_game_64d88c == 0) {
    loop_index = course_is_outdoor();
    if (loop_index == 0) {
      vehicle_prepare_clear_660568(vehicle_index);
    }
    else {
      vehicle_render_state_clear(vehicle_index);
    }
  }
  /* last vehicle (count == 1): free the shared texture dictionary */
  if (g_game_65a9c8 == 1) {
    RwTexDictionaryDestroy_004d5ac0(g_game_659d08);
    g_game_659d08 = (RwTexDictionary *)0x0;
  }
  /* clear this slot's entry in g_game_659d50, then count one vehicle fewer */
  ((undefined4 *)&g_game_659d50)[vehicle_index] = 0;
  g_game_65a9c8 = g_game_65a9c8 + -1;
  return;
}


