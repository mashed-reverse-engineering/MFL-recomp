#include "mfl_tier2.h"
/*00409eb0*/ extern float __cdecl player_rank_key(uint car);
/*0040a950*/ extern int __cdecl fx_wander_target(int key, float *a, float *b);
/*0040fdc0*/ extern int race_get_phase();
/*004b2b48*/ extern int _ftol();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd364*/ extern float k_f_n1;
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*005dd7f0*/ extern float k_f_30;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddacc*/ extern float k_f_360;
/*005ddad4*/ extern float k_f_90;
/*005ddc20*/ extern float k_f_45;
/*005e64a4*/ extern float k_f_0;
/*005de8c4*/ extern float k_f_0_016666668;
/*005debe4*/ extern float k_f_0_022222223;
/*005dfd7c*/ extern float k_f_1_6666666en4;
/*006292e4*/ extern float g_race_update_6292e4;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac6c*/ extern float g_fx_62ac6c;
/*0062ac70*/ extern float g_fx_62ac70;
/*0062ac74*/ extern float g_car_62ac74;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01a0*/ extern undefined1 g_cars_7f01a0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008b06e0*/ extern struct Car player_car[16];
/* car_auto_steer_assist_step */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "when the player gives no steering input steers the car matrix toward the route heading
   (fx_wander_target, limit by speed)" */

undefined4 car_auto_steer_assist_step(uint param_1,int param_2)

{
  float *pfVar1;
  double fVar2;
  uint uVar3;
  int race_phase;
  undefined4 uVar4;
  PlayerInput *pPVar5;
  int iVar6;
  double fVar7;
  double dot;
  struct {
    float fStack_14;
    float fStack_10;
    RwV3d local_c;
  } frame;
  double angle;
  double wrapped;
  double spread;
  float t;
  
  uVar3 = param_1;
  if (param_1 < 0x10) {
    iVar6 = (int)param_1 * 0xd04;
    pPVar5 = g_player_inputs + g_player_slots[(int)param_1].controller;
    if (player_car[(int)param_1].mode != 0) {
      pPVar5 = (PlayerInput *)&g_cars_7f01a0;
    }
    if (player_car[(int)param_1].controls_disabled != 0) {
      pPVar5 = (PlayerInput *)&g_cars_7f01a0;
    }
    race_phase = race_get_phase();
    if (((race_phase != 6) && (race_phase = race_get_phase(), race_phase != 0xb)) &&
       (race_phase = race_get_phase(), race_phase != RACE_PHASE_WINNER_DONE)) {
      pPVar5 = (PlayerInput *)&g_cars_7f01a0;
    }
    if ((pPVar5->left == 0) && (pPVar5->right == 0)) {
      pfVar1 = player_car[(int)param_1].forward;
      frame.local_c.x = *pfVar1;
      frame.local_c.z = player_car[(int)param_1].forward[2];
      if ((frame.local_c.x != k_f_0) || (frame.local_c.z != k_f_0)) {
        frame.local_c.y = 0.0;
        RwV3dNormalize(&frame.local_c,&frame.local_c);
        dot = g_fx_62ac70 * frame.local_c.z + g_car_62ac68 * frame.local_c.x + g_fx_62ac6c * frame.local_c.y;      /* kept in the x87 register (unrounded) and passed as a double, not stored to a float */
        if (dot < k_f_n1) {
          dot = k_f_n1;
        }
        else if (dot > float_1_0) {
          dot = float_1_0;
        }
        fVar7 = (double)FUN_004b3284(dot);
        angle = (float)(-(fVar7 * (double)(57.295799255371094) - k_f_0) + (double)k_f_90);
        frame.local_c.x = g_fx_62ac70 * player_car[(int)param_1].forward[1] -
                    g_fx_62ac6c * player_car[(int)param_1].forward[2];
        frame.local_c.y = g_car_62ac68 * player_car[(int)param_1].forward[2] - g_fx_62ac70 * *pfVar1;
        frame.local_c.z = g_fx_62ac6c * *pfVar1 - g_car_62ac68 * player_car[(int)param_1].forward[1];
        t = angle;
        if (frame.local_c.y < k_f_0) {
          t = k_f_360 - angle;
        }
        uVar4 = (int)player_rank_key(param_1);
        fx_wander_target(uVar4,&frame.fStack_14,&frame.fStack_10);
        wrapped = t - frame.fStack_14;      /* double: the original keeps this in st(0) through the wrap loops and the compares (no float store) */
        for (; wrapped < k_f_0; wrapped = wrapped + k_f_90) {
        }
        for (; k_f_90 < wrapped; wrapped = wrapped - k_f_90) {
        }
        if (!(k_f_30 > wrapped)) {
          if (!(k_f_60 > wrapped)) {
            angle = k_f_90 - wrapped;
          }
          else {
            angle = k_f_45 - wrapped;
          }
        }
        else {
          angle = -wrapped;
        }
        t = player_car[uVar3].speed * k_f_1_6666666en4;
        if (k_f_2 < t) {
          t = 2.0f;
        }
        if (t < angle) {
          angle = t;
        }
        if (angle < -t) {
          angle = -t;
        }
        fVar2 = (float)param_2;
        if (param_2 < 0) {
          fVar2 = fVar2 + k_f_4_2949673e9;
        }
        spread = frame.fStack_10 * k_f_4;      /* double: the original keeps it in st(0) (45 - spread cancels, a float store changes the angle by 1e-4) */
        if (spread < k_f_0) {
          spread = -spread;
        }
        if (k_f_45 < spread) {
          spread = k_f_45;
        }
        angle = angle * fVar2 * k_f_0_016666668 * g_race_update_6292e4 * (k_f_45 - spread) *
                k_f_0_022222223;      /* the original's multiplication order (angle * count first), another order rounds differently */
        if (angle != k_f_0) {
          RwMatrixRotate((RwMatrix *)((char *)player_car + player_car[uVar3].matrix_current * 0x40 + 0x928 + iVar6),
                         (RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
        }
      }
    }
    return 1;
  }
  return 0;
}


