#include "mfl_tier2.h"
/*00429dd0*/ extern undefined __cdecl frame_rotate_around_own_origin(int p0, RwReal p1);
/*00484c40*/ extern undefined __cdecl spline_tween_b(RwMatrix *p1, int p2, float p3);
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*005dda20*/ extern float k_f_4_2949673e9;
/*00609550*/ extern undefined g_race_update_609550;
/*007ef804*/ extern undefined4 g_tick_number;
/*008c88a4*/ extern undefined4 g_game_8c88a4[0x1000];
/* course_animated_objects_update */

/* "advances path times for the animated course objects, rotates children, spawns particles every
   second" */

int course_animated_objects_update(uint param_1)

{
  RwFrame *frame;
  float fVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  bool bVar7;
  struct {
    RwV3d local_88;
    undefined4 local_7c;
    undefined4 local_78;
    undefined4 local_74;
    undefined4 local_70;
    undefined4 local_6c;
    undefined4 local_68;
    undefined4 local_64;
    undefined4 local_60;
    undefined4 local_5c;
    undefined4 local_58;
    undefined1 pad_54[4];
    undefined4 local_50;
    undefined1 pad_4c[4];
    undefined4 local_48;
    undefined1 pad_44[1];
    undefined1 local_43;
    undefined1 pad_42[2];
    RwMatrix local_40;
  } frame_obj;
  volatile float vang;
  float angle;
  
  fVar2 = (fVar1 = (float)param_1) * 3.3333333e-4f;
  piVar5 = g_game_8c88a4;
  iVar4 = g_tick_number;
  do {
    iVar3 = 0;
    if (piVar5[-0x11] != 0) {
      iVar3 = *piVar5;
      if (iVar3 != 0) {
        frame = *(RwFrame **)(piVar5[-0x11] + 4);
        vang = fVar2 + *(float *)&piVar5[-1];
        *(float *)&piVar5[-1] = vang;
        if (*(float *)(iVar3 + 0xc) < vang) {
          *(float *)&piVar5[-1] = (vang - *(float *)(iVar3 + 0xc));
        }

        spline_tween_b(&frame_obj.local_40,iVar3,*(float *)&piVar5[-1]);
        RwFrameTransform(frame,&frame_obj.local_40,rwCOMBINEREPLACE);
        angle = fVar1 * 0.05f;
        *(float *)&piVar5[-4] = frame_obj.local_40.pos.x;
        *(float *)&piVar5[-3] = frame_obj.local_40.pos.y;
        *(float *)&piVar5[-2] = frame_obj.local_40.pos.z;
        frame_rotate_around_own_origin(piVar5[-0xf],angle);
        frame_rotate_around_own_origin(piVar5[-0xe],fVar1 * -0.05f);
        iVar4 = g_tick_number;
      }
      iVar3 = piVar5[-0x11];
      if ((iVar3 != 0) && (piVar5[2] != -1)) {
        iVar6 = 0;
        do {
          if (iVar6 == 0) {
            iVar3 = iVar4 / 0x3c;
            bVar7 = iVar4 % 0x3c == 0;
LAB_00429fcb:
            if (bVar7) {
              RwV3dTransformPoint(&frame_obj.local_88,
                                  (RwV3d *)((char *)&g_race_update_609550 + (iVar6 + piVar5[2] * 3) * 0xc),
                                  (RwMatrix *)(*(char **)(piVar5[-0x11] + 4) + 0x10));
              frame_obj.local_43 = 3;
              frame_obj.local_58 = 0x42055556;
              frame_obj.local_7c = 0xffffffff;
              frame_obj.local_48 = 0x920;
              frame_obj.local_78 = 0x3f19999a;
              frame_obj.local_74 = 0x3f19999a;
              frame_obj.local_50 = 0x3f800000;
              frame_obj.local_70 = 0;
              frame_obj.local_6c = 0;
              frame_obj.local_68 = 0;
              frame_obj.local_64 = 0;
              frame_obj.local_60 = 0;
              frame_obj.local_5c = 0;
              iVar3 = fx_trail_particle_spawn_interpolated(&frame_obj.local_88,0,0,0);
              iVar4 = g_tick_number;
            }
          }
          else {
            if (iVar6 == 1) {
              iVar3 = iVar4 / 100;
              bVar7 = iVar4 % 100 == 10;
              goto LAB_00429fcb;
            }
            iVar3 = iVar6 + -2;
            if (iVar3 == 0) {
              iVar3 = iVar4 / 100;
              bVar7 = iVar4 % 100 == 0xf;
              goto LAB_00429fcb;
            }
          }
          iVar6 = iVar6 + 1;
        } while (iVar6 < 3);
      }
    }
    piVar5 = piVar5 + 0x14;
    if ((int)g_game_8c88a4 + 0x27f < (int)piVar5) {
      return iVar3;
    }
  } while( true );
}


