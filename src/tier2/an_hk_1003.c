#include "mfl_tier2.h"
/*00416e20*/ extern int __cdecl point_project_onto_line_2d(int car_index, float *a, float *b, float *c, float scale, float *out);
/*00416fb0*/ extern int __cdecl ai_bot_weapon_logic(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*004220f0*/ extern int vehicle_get_position_158();
/*0044c180*/ extern double player_distance_get();
/*0047b520*/ extern int car_current_matrix();
/*004967d0*/ extern int triggers_get_array();
/*0049cb60*/ extern int sphere_vs_box_test(float *p1, float *p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005dd36c*/ extern float k_f_0_4;
/*005dda24*/ extern float k_f_0_75;
/*005ddb50*/ extern float k_f_8;
/*005de230*/ extern float k_f_n0_75;
/*005de234*/ extern float k_f_0_875;
/*005e64a4*/ extern float k_f_0;
/* ai_trigger_zone_target_for_car */

/* "scans the trigger array for type 7 boxes the car is in and projects a target onto the line" */

undefined4 __cdecl ai_trigger_zone_target_for_car(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  float fbits1;
  float fVar1;
  int type_id;
  char *owner;
  int iVar2;
  float *pfVar3;
  float *pfVar4;
  float *pfVar5;
  float10 fVar6;
  RwReal RVar7;
  struct {
    float local_68;
    int local_64;
    int local_60;
    int local_5c;
    float local_58;
    float local_54;
    undefined4 local_50;
    undefined4 local_4c;
    float local_48;
    float local_44;
    float local_40;
    RwV3d local_3c;
    undefined1 local_30 [16];
    float local_20 [5];
    float local_c;
  } frame;
  
  car_current_matrix(&frame.local_64,param_4);
  frame.local_48 = *(float *)(frame.local_64 + 0x30);
  frame.local_44 = *(float *)(frame.local_64 + 0x34);
  frame.local_40 = *(float *)(frame.local_64 + 0x38);
  iVar2 = triggers_get_array(&frame.local_5c);
  frame.local_60 = 0;
  if (0 < frame.local_5c) {
    pfVar3 = (float *)(iVar2 + 0xc);
    do {
      type_id = *(int *)&pfVar3[1];
      if (type_id < 0xff00) {
        if (type_id == 21) {
          owner = *(char **)&pfVar3[2];
          vehicle_get_position_158(param_4,frame.local_30);
          pfVar4 = (float *)(owner + 0x68);
          pfVar5 = frame.local_20;
          for (iVar2 = 6; iVar2 != 0; iVar2 = iVar2 + -1) {
            *pfVar5 = *pfVar4;
            pfVar4 = pfVar4 + 1;
            pfVar5 = pfVar5 + 1;
          }
          fVar1 = (frame.local_20[2] - frame.local_c) * k_f_0_875;
          frame.local_20[2] = frame.local_20[2] + fVar1;
          frame.local_c = frame.local_c - fVar1;
          iVar2 = sphere_vs_box_test(frame.local_20,(float *)frame.local_30);
          if (iVar2 == 1) {
            frame.local_50 = *(undefined4 *)(frame.local_64 + 0x30);
            frame.local_4c = *(undefined4 *)(frame.local_64 + 0x38);
            frame.local_58 = pfVar3[-3];
            frame.local_54 = pfVar3[-1];
            iVar2 = point_project_onto_line_2d(param_4,(float *)&frame.local_50,(float *)param_2,&frame.local_58,*pfVar3 * k_f_0_33333334 + k_f_0_5,(float *)param_3);
joined_r0x00417a28:
            if (iVar2 != 0) {
              return 1;
            }
          }
        }
        else if ((((type_id != 10) || (pfVar3[2] == 0.0)) ||
                 (iVar2 = *(int *)(*(int *)&pfVar3[2] + 4), iVar2 == 0)) ||
                (k_f_0_4 <= *(float *)(iVar2 + 0x10))) {
          frame.local_68 = *pfVar3;
          if (type_id == 13) {
            frame.local_68 = frame.local_68 + frame.local_68;
          }
          frame.local_3c.x = frame.local_48 - pfVar3[-3];
          frame.local_3c.y = frame.local_44 - pfVar3[-2];
          frame.local_3c.z = frame.local_40 - pfVar3[-1];
          RVar7 = RwV3dLength(&frame.local_3c);
          if ((frame.local_3c.y < frame.local_68) && (RVar7 < k_f_8)) {
            frame.local_50 = *(undefined4 *)(frame.local_64 + 0x30);
            frame.local_4c = *(undefined4 *)(frame.local_64 + 0x38);
            frame.local_58 = pfVar3[-3];
            frame.local_54 = pfVar3[-1];
            if (*(int *)&pfVar3[1] != 7) {
              iVar2 = point_project_onto_line_2d(param_4,(float *)&frame.local_50,(float *)param_2,&frame.local_58,frame.local_68 + k_f_0_5,(float *)param_3);
              goto joined_r0x00417a28;
            }
            fVar6 = (float10)player_distance_get(param_4);
            if (((fVar6 == k_f_0) &&
                (iVar2 = ai_bot_weapon_logic(param_4,(undefined4)&frame.local_50,param_2,(undefined4)&frame.local_58,(fbits1 = frame.local_68 + k_f_0_5, *(int *)&fbits1),
                                             param_3), iVar2 != 0)) &&
               ((fVar1 = *(float *)(frame.local_64 + 0x34) - pfVar3[-2], k_f_n0_75 < fVar1 &&
                (fVar1 < k_f_0_75)))) {
              return 2;
            }
          }
        }
      }
      frame.local_60 = frame.local_60 + 1;
      pfVar3 = pfVar3 + 0x23;
    } while (frame.local_60 < frame.local_5c);
  }
  return 0;
}


