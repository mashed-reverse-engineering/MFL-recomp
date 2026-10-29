#include "mfl_tier2.h"
extern struct FxSlot g_fx_slots[10];
/*00419fa0*/ extern double ai_get_float_8c9980();
/*0045f5a0*/ extern int explosion_queue(void *p0, int p1, int p2, int p3, int p4);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, void *p1);
/*00469cb0*/ extern int sfx_move_playing_voice();
/*00472bb0*/ extern int sfx_voice_stop();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0049bf00*/ extern int fx_ribbon_add_point();
/*004c4e20*/ extern int FUN_004c4e20(undefined4 p0, void *p1, void *p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd40c*/ extern float k_f_1_5;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb0c*/ extern float k_f_15;
/*005ddb10*/ extern float k_f_0_99;
/*005ddb14*/ extern float float_7_0;
/*0062ac74*/ extern float g_car_62ac74;
/*007ef7f4*/ extern float g_tick_seconds;
/*008d98b0*/ extern undefined4 g_fx_8d98b0;
/* fx_update_all */

/* "advances the effect timers (7.0 s lifetime), updates positions and sounds, queues explosions" */

void fx_update_all(void)

{
  float fbits1;
  float fVar1;
  float step;
  RwMatrix *clump_ltm;
  undefined4 world_ptr;
  int clump_frame_or_hit;
  uint *phase_ptr;
  uint slot_index;
  double fVar8;
  undefined4 *ray_origin_ptr;
  undefined1 *hit_ptr;
  struct {
    RwV3d move;
    undefined4 ray_x;
    undefined4 ray_y;
    undefined4 ray_z;
    float ray_len;
    undefined1 hit_record [64];
  } frame;
  
  slot_index = 0;
  phase_ptr = &g_fx_slots[0].phase;
  do {
    if (phase_ptr[1] != 0) {
      if (float_7_0 < *(float *)phase_ptr) {
        phase_ptr[1] = 0;
      }
      *(float *)phase_ptr = g_tick_seconds + *(float *)phase_ptr;
      clump_ltm = RwFrameGetLTM(*(RwFrame **)(((char *)((FxSlot *)(phase_ptr + -2))->clump) + 4));
      step = g_tick_seconds * *(float *)&phase_ptr[-1];
      frame.move.x = step * (clump_ltm->at).x;
      frame.move.y = (clump_ltm->at).y * step;
      frame.move.z = step * (clump_ltm->at).z + *(float *)&phase_ptr[5];
      frame.move.x = frame.move.x + *(float *)&phase_ptr[3];
      frame.move.y = frame.move.y + *(float *)&phase_ptr[4];
      *(float *)&phase_ptr[3] = (*(float *)&phase_ptr[3] * k_f_0_99);
      *(float *)&phase_ptr[4] = (*(float *)&phase_ptr[4] * k_f_0_99);
      *(float *)&phase_ptr[5] = (*(float *)&phase_ptr[5] * k_f_0_99);
      RwFrameTranslate(*(RwFrame **)(((char *)((FxSlot *)(phase_ptr + -2))->clump) + 4),&frame.move,rwCOMBINEPOSTCONCAT);
      fx_ribbon_add_point(phase_ptr[2],*(int *)(((char *)((FxSlot *)(phase_ptr + -2))->clump) + 4) + 0x40,0,0);
      g_fx_8d98b0 = slot_index;
      if ((char *)&g_car_62ac74 + 0x26d73 < (char *)phase_ptr) {
        g_fx_8d98b0 = slot_index - (slot_index & 0xfffffffc);
      }
      sfx_move_playing_voice(0x14,*(int *)(((char *)((FxSlot *)(phase_ptr + -2))->clump) + 4) + 0x40,g_fx_8d98b0)
      ;
      frame.ray_len = (frame.move.z * frame.move.z + frame.move.x * frame.move.x + frame.move.y * frame.move.y) *
                 k_f_1_5 + k_f_0_05;
      clump_frame_or_hit = *(int *)(((char *)((FxSlot *)(phase_ptr + -2))->clump) + 4);
      frame.ray_x = *(undefined4 *)(clump_frame_or_hit + 0x40);
      frame.ray_y = *(undefined4 *)(clump_frame_or_hit + 0x44);
      frame.ray_z = *(undefined4 *)(clump_frame_or_hit + 0x48);
      hit_ptr = frame.hit_record;
      ray_origin_ptr = &frame.ray_x;
      world_ptr = weapon_get_rw_801b14();
      clump_frame_or_hit = FUN_004c4e20(world_ptr,ray_origin_ptr,hit_ptr);
      if (clump_frame_or_hit != 0) {
        clump_frame_or_hit = rocket_surface_hit_test(frame.hit_record,&frame.ray_x);
        if (clump_frame_or_hit == 0) {
          frame.ray_len = 1.2;
          fVar8 = (double)ai_get_float_8c9980(0x43480000,0x43160000,2);
          explosion_queue(&frame.ray_x,(fbits1 = (float)(fVar8 + (double)k_f_15), *(int *)&fbits1),0x43480000,0x43160000,2);
          fx_emitter_spawn_a_default(&frame.ray_x,&g_car_62ac74,0x43b40000);
          g_fx_8d98b0 = slot_index;
          if ((char *)&g_car_62ac74 + 0x26d73 < (char *)phase_ptr) {
            g_fx_8d98b0 = slot_index - (slot_index & 0xfffffffc);
          }
          sfx_voice_stop(0x14,g_fx_8d98b0);
        }
      }
    }
    phase_ptr = phase_ptr + 8;
    slot_index = slot_index + 1;
  } while ((char *)phase_ptr < (char *)&g_car_62ac74 + 0x26e34);
  return;
}


