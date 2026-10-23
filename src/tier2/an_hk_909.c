#include "mfl_tier2.h"
extern struct FxSlot g_fx_slots[10];
/*00419fa0*/ extern double ai_get_float_8c9980();
/*0045f5a0*/ extern int explosion_queue(void *p0, int p1, int p2, int p3, int p4);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, void *p1);
/*00469cb0*/ extern int sfx_move_playing_voice();
/*00472bb0*/ extern int sfx_voice_is_active();
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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "advances the effect timers (7.0 s lifetime), updates positions and sounds, queues explosions" */

void fx_update_all(void)

{
  float fbits1;
  float fVar1;
  float fVar2;
  RwMatrix *pRVar3;
  undefined4 uVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  double fVar8;
  undefined4 *puVar9;
  undefined1 *puVar10;
  struct {
    RwV3d local_5c;
    undefined4 local_50;
    undefined4 local_4c;
    undefined4 local_48;
    float local_44;
    undefined1 local_40 [64];
  } frame;
  
  uVar7 = 0;
  puVar6 = &g_fx_slots[0].phase;
  do {
    if (puVar6[1] != 0) {
      if (float_7_0 < *(float *)puVar6) {
        puVar6[1] = 0;
      }
      *(float *)puVar6 = g_tick_seconds + *(float *)puVar6;
      pRVar3 = RwFrameGetLTM(*(RwFrame **)(((char *)((FxSlot *)(puVar6 + -2))->clump) + 4));
      fVar2 = g_tick_seconds * *(float *)&puVar6[-1];
      frame.local_5c.x = fVar2 * (pRVar3->at).x;
      frame.local_5c.y = (pRVar3->at).y * fVar2;
      frame.local_5c.z = fVar2 * (pRVar3->at).z + *(float *)&puVar6[5];
      frame.local_5c.x = frame.local_5c.x + *(float *)&puVar6[3];
      frame.local_5c.y = frame.local_5c.y + *(float *)&puVar6[4];
      *(float *)&puVar6[3] = (*(float *)&puVar6[3] * k_f_0_99);
      *(float *)&puVar6[4] = (*(float *)&puVar6[4] * k_f_0_99);
      *(float *)&puVar6[5] = (*(float *)&puVar6[5] * k_f_0_99);
      RwFrameTranslate(*(RwFrame **)(((char *)((FxSlot *)(puVar6 + -2))->clump) + 4),&frame.local_5c,rwCOMBINEPOSTCONCAT);
      fx_ribbon_add_point(puVar6[2],*(int *)(((char *)((FxSlot *)(puVar6 + -2))->clump) + 4) + 0x40,0,0);
      g_fx_8d98b0 = uVar7;
      if ((char *)&g_car_62ac74 + 0x26d73 < (char *)puVar6) {
        g_fx_8d98b0 = uVar7 - (uVar7 & 0xfffffffc);
      }
      sfx_move_playing_voice(0x14,*(int *)(((char *)((FxSlot *)(puVar6 + -2))->clump) + 4) + 0x40,g_fx_8d98b0)
      ;
      frame.local_44 = (frame.local_5c.z * frame.local_5c.z + frame.local_5c.x * frame.local_5c.x + frame.local_5c.y * frame.local_5c.y) *
                 k_f_1_5 + k_f_0_05;
      iVar5 = *(int *)(((char *)((FxSlot *)(puVar6 + -2))->clump) + 4);
      frame.local_50 = *(undefined4 *)(iVar5 + 0x40);
      frame.local_4c = *(undefined4 *)(iVar5 + 0x44);
      frame.local_48 = *(undefined4 *)(iVar5 + 0x48);
      puVar10 = frame.local_40;
      puVar9 = &frame.local_50;
      uVar4 = weapon_get_rw_801b14();
      iVar5 = FUN_004c4e20(uVar4,puVar9,puVar10);
      if (iVar5 != 0) {
        iVar5 = rocket_surface_hit_test(frame.local_40,&frame.local_50);
        if (iVar5 == 0) {
          frame.local_44 = 1.2;
          fVar8 = (double)ai_get_float_8c9980(0x43480000,0x43160000,2);
          explosion_queue(&frame.local_50,(fbits1 = (float)(fVar8 + (double)k_f_15), *(int *)&fbits1),0x43480000,0x43160000,2);
          fx_emitter_spawn_a_default(&frame.local_50,&g_car_62ac74,0x43b40000);
          g_fx_8d98b0 = uVar7;
          if ((char *)&g_car_62ac74 + 0x26d73 < (char *)puVar6) {
            g_fx_8d98b0 = uVar7 - (uVar7 & 0xfffffffc);
          }
          sfx_voice_is_active(0x14,g_fx_8d98b0);
        }
      }
    }
    puVar6 = puVar6 + 8;
    uVar7 = uVar7 + 1;
  } while ((char *)puVar6 < (char *)&g_car_62ac74 + 0x26e34);
  return;
}


