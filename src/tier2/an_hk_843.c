#include "mfl_tier2.h"
extern struct TrailSlot g_trails[6];
/*004146f0*/ extern int race_event_has_panel();
/*0042b200*/ extern char *course_get_68b8b4_ptr();
/*0042b210*/ extern char *course_get_record_point();
/*00467ea0*/ extern char *weapon_get_rw_801b14();
/*0048e650*/ extern int rwp_object_set_in_world();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00492d10*/ extern int rwp_object_set_active();
/*004c42f0*/ extern void FUN_004c42f0(void *p0, void *p1, void *p2);
/*004c4460*/ extern void FUN_004c4460(void *p0, void *p1, void *p2);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd41c*/ extern float float_5_0;
/*005dd634*/ extern float k_f_2;
/*005de194*/ extern float k_f_0_13;
/*00603920*/ extern undefined4 g_trail_603920;
/*0062ac74*/ extern float g_car_62ac74;
/*00652fb0*/ extern undefined g_trail_652fb0;
/*00652fb4*/ extern undefined g_trail_652fb4;
/*00652fb8*/ extern undefined g_trail_652fb8;
/*00652fbc*/ extern undefined g_trail_update_652fbc;
/*00652fc0*/ extern undefined g_trail_update_652fc0;
/*00652fc4*/ extern undefined g_trail_update_652fc4;
#define TRAIL_COUNT (6)
/* bytes per trail in the g_trail_652fb0 table: two RwV3d positions (object_a at +0, object_b at +0xc) */
#define TRAIL_POS_STRIDE (0x18)
/* RwMatrix flags for an identity matrix (inferred: rwMATRIXINTERNALIDENTITY | type bits 3) */
#define RW_MATRIX_IDENTITY_FLAGS (0x20003)
/* trail_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per frame update of the trail records (inferred)" */

/* Per frame: hides all TRAIL_COUNT trail slots, then for each enabled slot (< enabled_trail_count) finds its
   two course record points (0 and 3, slot 0 always uses point 0), probes the world under them, positions the
   slot's frame at the start point and aims it at the end point, stores the trail length in w20 and updates the
   object_a / object_b matrices and their saved positions. Takes no parameters, returns nothing (inferred). */
void trail_update(void)

{
  int record_x_bits, record_z_bits;
  float *end_point_ptr;
  uint object_a_handle;
  RwFrame *frame;
  float *point_ptr;
  char *world_ptr;
  int scratch;
  int trail_index;
  RwReal trail_length;
  undefined1 *intersection_ptr;
  struct {
    float ray_top_x;
    float ray_top_y;
    float ray_top_z;
    float ray_bottom_x;
    float ray_bottom_y;
    float ray_bottom_z;
    int saved_trail_index;
    int record_index;
    RwReal ray_length;
    RwV3d ray_delta;
    RwMatrix trail_matrix;
    undefined1 pad_60[4];
    float record_y;
    undefined1 pad_58[4];
    int enabled_trail_count;
    char *course_record_table;
    RwV3d trail_delta;
    undefined1 intersection_buf [56];
    float local_8;
    int pad_top;
  } frame_obj;

  /* setup: slot counter, course record table, number of slots enabled this frame (inferred) */
  trail_index = 0;
  frame_obj.saved_trail_index = 0;
  frame_obj.course_record_table = course_get_68b8b4_ptr();
  frame_obj.enabled_trail_count = race_event_has_panel();
  /* loop over all trail slots: hide object_a / object_b of each, then show it again if enabled */
  do {
    object_a_handle = g_trails[trail_index].object_a;
    g_trails[trail_index].active = 0;
    rwp_object_set_in_world(object_a_handle,0);
    rwp_object_set_active(g_trails[trail_index].object_a,0);
    rwp_object_set_in_world(g_trails[trail_index].object_b,0);
    rwp_object_set_active(g_trails[trail_index].object_b,0);
    /* enabled slot: slot 0 starts at record point 0, others take their start from the course record table */
    if (trail_index < frame_obj.enabled_trail_count) {
      if (trail_index == 0) {
        frame_obj.record_index = 0;
      }
      else {
        frame_obj.record_index = *(int *)(frame_obj.course_record_table + -4 + trail_index * 4);
        if (frame_obj.record_index < 1) goto LAB_00414c52;
      }
      object_a_handle = g_trails[trail_index].object_a;
      g_trails[trail_index].active = 1;
      rwp_object_set_in_world(object_a_handle,1);
      rwp_object_set_active(g_trails[trail_index].object_a,1);
      rwp_object_set_in_world(g_trails[trail_index].object_b,1);
      rwp_object_set_active(g_trails[trail_index].object_b,1);
      /* probe 1: vertical segment at record point 0 (top = point + float_5_0 in y, bottom = point - k_f_2 in y);
         if it hits, the slot start becomes the point, with y = (top_y - local_8 * ray_length) + k_f_0_13.
         local_8 is never written in this function (inferred: stale stack value, check disassembly) */
      point_ptr = (float *)course_get_record_point(frame_obj.record_index,0);
      record_x_bits = ((int *)point_ptr)[0];
      *(int *)&frame_obj.record_y = ((int *)point_ptr)[1];
      record_z_bits = ((int *)point_ptr)[2];
      *(int *)&frame_obj.ray_top_y = *(int *)&frame_obj.record_y;
      *(int *)&frame_obj.ray_bottom_y = *(int *)&frame_obj.record_y;
      *(int *)&frame_obj.ray_top_x = record_x_bits;
      frame_obj.ray_top_y = frame_obj.record_y + float_5_0;
      *(int *)&frame_obj.ray_bottom_x = record_x_bits;
      frame_obj.ray_bottom_y = frame_obj.record_y - k_f_2;
      *(int *)&frame_obj.ray_top_z = record_z_bits;
      *(int *)&frame_obj.ray_bottom_z = record_z_bits;
      frame_obj.ray_delta.x = frame_obj.ray_top_x - frame_obj.ray_bottom_x;
      frame_obj.ray_delta.y = frame_obj.ray_top_y - frame_obj.ray_bottom_y;
      frame_obj.ray_delta.z = frame_obj.ray_top_z - frame_obj.ray_bottom_z;
      frame_obj.ray_length = RwV3dLength(&frame_obj.ray_delta);
      intersection_ptr = frame_obj.intersection_buf;
      point_ptr = &frame_obj.ray_top_x;
      world_ptr = weapon_get_rw_801b14();
      scratch = world_intersect_line((RpWorld *)world_ptr,(undefined4)point_ptr,(undefined4)intersection_ptr);
      if (scratch != 0) {
        *(int *)&g_trails[trail_index].w08 = record_x_bits;
        *(int *)&g_trails[trail_index].w0c = *(int *)&frame_obj.record_y;
        *(int *)&g_trails[trail_index].w10 = record_z_bits;
        g_trails[trail_index].w0c = (frame_obj.ray_top_y - frame_obj.local_8 * frame_obj.ray_length) + k_f_0_13;
      }
      /* probe 2: same vertical probe at record point 3, result stored as the slot end (w14 / w18 / w1c) */
      point_ptr = (float *)course_get_record_point(frame_obj.record_index,3);
      record_x_bits = ((int *)point_ptr)[0];
      *(int *)&frame_obj.record_y = ((int *)point_ptr)[1];
      record_z_bits = ((int *)point_ptr)[2];
      *(int *)&frame_obj.ray_top_y = *(int *)&frame_obj.record_y;
      *(int *)&frame_obj.ray_bottom_y = *(int *)&frame_obj.record_y;
      *(int *)&frame_obj.ray_top_x = record_x_bits;
      frame_obj.ray_top_y = frame_obj.record_y + float_5_0;
      *(int *)&frame_obj.ray_bottom_x = record_x_bits;
      frame_obj.ray_bottom_y = frame_obj.record_y - k_f_2;
      *(int *)&frame_obj.ray_top_z = record_z_bits;
      *(int *)&frame_obj.ray_bottom_z = record_z_bits;
      frame_obj.ray_delta.x = frame_obj.ray_top_x - frame_obj.ray_bottom_x;
      frame_obj.ray_delta.y = frame_obj.ray_top_y - frame_obj.ray_bottom_y;
      frame_obj.ray_delta.z = frame_obj.ray_top_z - frame_obj.ray_bottom_z;
      frame_obj.ray_length = RwV3dLength(&frame_obj.ray_delta);
      intersection_ptr = frame_obj.intersection_buf;
      point_ptr = &frame_obj.ray_top_x;
      world_ptr = weapon_get_rw_801b14();
      scratch = world_intersect_line((RpWorld *)world_ptr,(undefined4)point_ptr,(undefined4)intersection_ptr);
      if (scratch != 0) {
        *(int *)&g_trails[trail_index].w14 = record_x_bits;
        *(int *)&g_trails[trail_index].w18 = *(int *)&frame_obj.record_y;
        *(int *)&g_trails[trail_index].w1c = record_z_bits;
        g_trails[trail_index].w18 = (frame_obj.ray_top_y - frame_obj.local_8 * frame_obj.ray_length) + k_f_0_13;
      }
      /* place the slot's frame at the start point, aim it at the end point, then scale it (inferred) */
      frame = *(RwFrame **)(g_trails[trail_index].w00 + 4);
      point_ptr = &g_trails[trail_index].w08;
      RwFrameTranslate(frame,(RwV3d *)point_ptr,rwCOMBINEREPLACE);
      end_point_ptr = &g_trails[trail_index].w14;
      FUN_004c42f0(frame,end_point_ptr,&g_car_62ac74);
      RwFrameScale(frame,(RwV3d *)&g_trail_603920,rwCOMBINEPRECONCAT);
      /* trail length = |start - end|, stored as a float in w20 */
      frame_obj.trail_delta.x = *point_ptr - *end_point_ptr;
      frame_obj.trail_delta.y = g_trails[trail_index].w0c - g_trails[trail_index].w18;
      frame_obj.trail_delta.z = g_trails[trail_index].w10 - g_trails[trail_index].w1c;
      trail_length = RwV3dLength(&frame_obj.trail_delta);
      *(float *)&g_trails[trail_index].w20 = trail_length;
      frame_obj.trail_matrix.flags = frame_obj.trail_matrix.flags | RW_MATRIX_IDENTITY_FLAGS;
      g_trails[trail_index].active = 1;
      frame_obj.trail_matrix.at.z = 1.0;
      frame_obj.trail_matrix.up.y = 1.0;
      frame_obj.trail_matrix.right.x = 1.0;
      frame_obj.trail_matrix.up.x = 0.0;
      frame_obj.trail_matrix.right.z = 0.0;
      frame_obj.trail_matrix.right.y = 0.0;
      frame_obj.trail_matrix.at.y = 0.0;
      frame_obj.trail_matrix.at.x = 0.0;
      frame_obj.trail_matrix.up.z = 0.0;
      frame_obj.trail_matrix.pos.z = 0.0;
      frame_obj.trail_matrix.pos.y = 0.0;
      frame_obj.trail_matrix.pos.x = 0.0;
      /* object_a matrix: identity at the start point, oriented by FUN_004c4460 (inferred), then set on object_a */
      RwMatrixTranslate(&frame_obj.trail_matrix,(RwV3d *)point_ptr,rwCOMBINEREPLACE);
      FUN_004c4460(&frame_obj.trail_matrix,end_point_ptr,&g_car_62ac74);
      rwp_object_set_matrix(g_trails[trail_index].object_a,&frame_obj.trail_matrix);
      /* save object_a position in g_trail_652fb0 (scratch = byte offset of this slot's row) */
      scratch = frame_obj.saved_trail_index * TRAIL_POS_STRIDE;
      *(RwReal *)((char *)&g_trail_652fb0 + scratch) = frame_obj.trail_matrix.pos.x;
      *(RwReal *)((char *)&g_trail_652fb4 + scratch) = frame_obj.trail_matrix.pos.y;
      *(RwReal *)((char *)&g_trail_652fb8 + scratch) = frame_obj.trail_matrix.pos.z;
      frame_obj.trail_matrix.at.z = 1.0;
      frame_obj.trail_matrix.up.y = 1.0;
      frame_obj.trail_matrix.right.x = 1.0;
      frame_obj.trail_matrix.flags = frame_obj.trail_matrix.flags | RW_MATRIX_IDENTITY_FLAGS;
      frame_obj.trail_matrix.up.x = 0.0;
      frame_obj.trail_matrix.right.z = 0.0;
      frame_obj.trail_matrix.right.y = 0.0;
      frame_obj.trail_matrix.at.y = 0.0;
      frame_obj.trail_matrix.at.x = 0.0;
      frame_obj.trail_matrix.up.z = 0.0;
      frame_obj.trail_matrix.pos.z = 0.0;
      frame_obj.trail_matrix.pos.y = 0.0;
      frame_obj.trail_matrix.pos.x = 0.0;
      RwMatrixTranslate(&frame_obj.trail_matrix,(RwV3d *)end_point_ptr,rwCOMBINEREPLACE);
      FUN_004c4460(&frame_obj.trail_matrix,point_ptr,&g_car_62ac74);
      /* object_b matrix: same construction at the end point, set on object_b, position saved to g_trail_update_652fbc */
      rwp_object_set_matrix(g_trails[trail_index].object_b,&frame_obj.trail_matrix);
      *(RwReal *)((char *)&g_trail_update_652fbc + scratch) = frame_obj.trail_matrix.pos.x;
      *(RwReal *)((char *)&g_trail_update_652fc0 + scratch) = frame_obj.trail_matrix.pos.y;
      *(RwReal *)((char *)&g_trail_update_652fc4 + scratch) = frame_obj.trail_matrix.pos.z;
      trail_index = frame_obj.saved_trail_index;
    }
LAB_00414c52:
    /* next slot; also the jump target when a slot's record index is below 1 (skips that slot's body) */
    trail_index = trail_index + 1;
    frame_obj.saved_trail_index = trail_index;
  } while (trail_index < TRAIL_COUNT);
  return;
}


