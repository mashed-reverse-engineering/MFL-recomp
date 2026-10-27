#include "mfl_tier2.h"
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* course_call_cb2 */
/* "jmp [entry+0x1C]: called by course_render" */
/* course_call_cb3 */
/* "jmp [entry+0x20]: called by course_render" */
/* course_call_cb4 */
/* "jmp [entry+0x24]: called by course_race_start" */
/* course_call_cb5 */
/* "jmp [entry+0x28]: called by course_unload before the world is destroyed" */
/* course_call_cb6 */
/* "jmp [entry+0x2C]" */
/* course_call_cb7 */
/* "jmp [entry+0x30]" */
/* course_call_cb8 */
/* "jmp [entry+0x34]" */
/* course_call_cb9 */
/* "jmp [entry+0x38]" */
/* course_call_cb10 */
/* "jmp [entry+0x3C]" */
/* course_call_cb11 */
/* "jmp [entry+0x40]" */
/* course_call_world_setup */
/* "jmp [entry+0x44] (cb12), before the world is created" */
/* course_table_find_entry */
/* "(course id): CourseEntry* in g_course_table or 0" */
/* course_has_cb0 */
/* "[entry+0x14] != 0" */
/* course_has_cb1 */
/*  */
/* course_has_cb2 */
/*  */
/* course_has_cb3 */
/*  */
/* course_has_cb4 */
/*  */
/* course_has_cb5 */
/*  */
/* course_has_cb6 */
/*  */
/* course_has_cb7 */
/*  */
/* course_has_cb8 */
/*  */
/* course_has_cb9 */
/*  */
/* course_has_cb10 */
/*  */
/* course_has_cb11 */
/*  */
/* course_has_world_setup */
/* "returns the callback at +0x44 of the current record (cb12)" */
/* game_set_particles_60761c */
/* "g_particles_60761c = param_1; return;" */
/* game_set_particles_607620 */
/* "g_particles_607620 = param_1; return;" */
/* game_set_65a9b8_607624 */
/* "stores two values" */
/* game_set_65a9c0_65a9c4 */
/* "stores two values" */
/* vehicle_set_flag_10 */
/* (vehicle, on): sets or clears the front (0x10) and back (0x20) flags in
   g_vehicle_table[vehicle_index].flags (+0x294). Also sets 0x1000 when the
   front flag is set and 0x2000 when the back flag is set (inferred: meaning
   of 0x1000 / 0x2000 not in dossier). Parameters: vehicle_index = slot 0..3,
   front_on / back_on = non-zero to set, zero to clear. Returns nothing. */
void vehicle_set_flag_10(int vehicle_index,int front_on,int back_on)
{
  struct VehicleRecord *v = &g_vehicle_table[vehicle_index];

  if (front_on != 0) {
    v->flags |= VEHICLE_FLAG_FRONT;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_FRONT;
  }
  if (back_on != 0) {
    v->flags |= VEHICLE_FLAG_BACK;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_BACK;
  }
  if ((v->flags & VEHICLE_FLAG_FRONT) != 0) {
    v->flags |= VEHICLE_FLAG_1000;
  }
  if ((v->flags & VEHICLE_FLAG_BACK) != 0) {
    v->flags |= VEHICLE_FLAG_2000;
  }
  return;
}
