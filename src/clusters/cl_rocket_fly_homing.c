// cluster-entries: rocket_fly_homing
/* Installed cluster rocket_fly_homing (tools/cluster_install.py from output/cluster_pilot10/rocket_fly_homing.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408a80*/ extern int fx_object_get_matrix_ptr();
/*0047b520*/ extern int car_current_matrix();
/*0047b750*/ extern int car_get_speed();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd374*/ extern float k_f_0_2;
/*005dd61c*/ extern float k_f_0_001;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de25c*/ extern float k_f_0_005;
/*005de490*/ extern float k_f_0_02;
/*005debe8*/ extern float k_f_0_075;
/*005df774*/ extern float k_f_4_8780486en5;
/*005df7b8*/ extern float k_f_n0_2;
/*0062ac74*/ extern float g_car_62ac74;
/* rocket_fly_homing */

/* WARNING: Unknown calling convention */
/* "flight step of a rocket that has a target car (homing)" */

void rocket_fly_homing_n(int in_esi)

{
  RwV3d *v;
  RwFrame *frame;
  float fVar1;
  struct {
    int local_6c;
    float local_68;
    RwV3d local_64;
    RwV3d local_58;
    RwV3d local_4c;
    RwMatrix local_40;
  } frame_obj;
  
  frame_obj.local_68 = 0.0;
  frame = *(RwFrame **)(*(char **)(in_esi + 0x5c) + 4);
  RwMatrixInvert(&frame_obj.local_40,&RwFrame_modelling(frame));
  if (*(int *)(in_esi + 0x54) == -1) {
    fx_object_get_matrix_ptr(&frame_obj.local_6c,*(undefined4 *)(in_esi + 0x58));
  }
  else {
    car_current_matrix(&frame_obj.local_6c,*(int *)(in_esi + 0x54));
  }
  frame_obj.local_64.x = *(RwReal *)(frame_obj.local_6c + 0x30);
  frame_obj.local_64.z = *(RwReal *)(frame_obj.local_6c + 0x38);
  frame_obj.local_64.y = *(float *)(frame_obj.local_6c + 0x34) + k_f_0_2;
  RwV3dTransformPoints(&frame_obj.local_4c,&frame_obj.local_64,1,&frame_obj.local_40);
  RwV3dNormalize(&frame_obj.local_58,&frame_obj.local_4c);
  if (frame_obj.local_58.y <= k_f_0_2) {
    if (k_f_n0_2 <= frame_obj.local_58.y) {
      *(undefined4 *)(in_esi + 0x40) = 0;
    }
    else {
      *(undefined4 *)(in_esi + 0x40) = 0x41a00000;
    }
  }
  else {
    *(undefined4 *)(in_esi + 0x40) = 0xc1a00000;
  }
  if (frame_obj.local_58.x <= k_f_0_2) {
    if (k_f_n0_2 <= frame_obj.local_58.x) {
      *(undefined4 *)(in_esi + 0x44) = 0;
    }
    else {
      *(undefined4 *)(in_esi + 0x44) = 0xc0000000;
    }
  }
  else {
    *(undefined4 *)(in_esi + 0x44) = 0x40000000;
  }
  RwV3dLength(&frame_obj.local_4c);
  *(float *)(in_esi + 0x48) = *(float *)(in_esi + 0x44) + *(float *)(in_esi + 0x48);
  *(float *)(in_esi + 0x4c) = *(float *)(in_esi + 0x4c) + *(float *)(in_esi + 0x40);
  RwFrameSetIdentity(frame);
  RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,*(RwReal *)(in_esi + 0x48),rwCOMBINEREPLACE);
  RwFrameRotate(frame,&(RwFrame_modelling(frame)).right,*(RwReal *)(in_esi + 0x4c),rwCOMBINEPOSTCONCAT);
  car_get_speed(&frame_obj.local_68,*(undefined4 *)(in_esi + 0x54));
  fVar1 = k_f_4_8780486en5 * frame_obj.local_68;
  *(float *)(in_esi + 0x34) = fVar1;
  fVar1 = fVar1 - *(float *)(in_esi + 0x30);
  *(float *)(in_esi + 0x38) = fVar1;
  if (k_f_0_02 <= fVar1) {
    fVar1 = *(float *)(in_esi + 0x2c) - k_f_0_001;
  }
  else {
    fVar1 = *(float *)(in_esi + 0x2c) + k_f_1_0en4;
  }
  *(float *)(in_esi + 0x2c) = fVar1;
  if (*(float *)(in_esi + 0x2c) < k_f_0) {
    *(undefined4 *)(in_esi + 0x2c) = 0;
  }
  if (k_f_0_005 < *(float *)(in_esi + 0x2c)) {
    *(undefined4 *)(in_esi + 0x2c) = 0x3ba3d70a;
  }
  fVar1 = *(float *)(in_esi + 0x2c) + *(float *)(in_esi + 0x30);
  *(float *)(in_esi + 0x30) = fVar1;
  if (fVar1 < k_f_0_075) {
    *(undefined4 *)(in_esi + 0x30) = 0x3d99999a;
  }
  v = (RwV3d *)(in_esi + 0xc);
  *(float *)(in_esi + 0x1c) = *(float *)(in_esi + 0x30) * (RwFrame_modelling(frame)).at.x;
  *(float *)(in_esi + 0x20) = (RwFrame_modelling(frame)).at.y * *(float *)(in_esi + 0x30);
  *(float *)(in_esi + 0x24) = (RwFrame_modelling(frame)).at.z * *(float *)(in_esi + 0x30);
  fVar1 = *(float *)(in_esi + 0x28) + *(float *)(in_esi + 0x20);
  *(float *)(in_esi + 0x20) = fVar1;
  v->x = v->x + *(float *)(in_esi + 0x1c);
  *(float *)(in_esi + 0x10) = fVar1 + *(float *)(in_esi + 0x10);
  *(float *)(in_esi + 0x14) = *(float *)(in_esi + 0x24) + *(float *)(in_esi + 0x14);
  RwFrameTranslate(frame,v,rwCOMBINEPOSTCONCAT);
  return;
}

/* entry thunk at the original address of rocket_fly_homing: register inputs (esi) -> natural C rocket_fly_homing_n; ret 0 */
__declspec(naked) void rocket_fly_homing(void)
{
    __asm {
        push esi
        call rocket_fly_homing_n
        add esp, 4
        ret
    }
}
