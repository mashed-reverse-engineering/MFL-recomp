// cluster-entries: rocket_fly_straight
/* Installed cluster rocket_fly_straight (tools/cluster_install.py from output/cluster_pilot9/rocket_fly_straight.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*005ddea0*/ extern float k_f_20;
/*005de19c*/ extern float k_f_1_0en4;
/*005de25c*/ extern float k_f_0_005;
/*0062ac74*/ extern float g_car_62ac74;
extern double sin(double);
/* rocket_fly_straight */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "flight step of a rocket without a target (target ids both -1)" */

void rocket_fly_straight_n(int rocket)

{
  RwV3d *v;
  float fVar1;
  float fVar2;
  RwFrame *frame;
  float10 pitch_amp;
  float10 wobble;
  float10 yaw_amp;
  
  frame = *(RwFrame **)(*(char **)(rocket + 0x5c) + 4);
  wobble = (float10)sin((float10)*(float *)(rocket + 0x18) * (float10)k_f_20);
  yaw_amp = (float10)(15.0);
  fVar1 = *(float *)(rocket + 0x48);
  pitch_amp = (float10)(20.0);
  fVar2 = *(float *)(rocket + 0x4c);
  RwFrameSetIdentity(frame);
  RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,(float)(yaw_amp * wobble + (float10)fVar1),
                rwCOMBINEREPLACE);
  RwFrameRotate(frame,&(RwFrame_modelling(frame)).right,(float)(wobble * pitch_amp + (float10)fVar2),
                rwCOMBINEPOSTCONCAT);
  fVar1 = *(float *)(rocket + 0x2c) + k_f_1_0en4;
  *(float *)(rocket + 0x2c) = fVar1;
  if (k_f_0_005 < fVar1) {
    *(undefined4 *)(rocket + 0x2c) = 0x3ba3d70a;
  }
  v = (RwV3d *)(rocket + 0xc);
  fVar1 = *(float *)(rocket + 0x30) + *(float *)(rocket + 0x2c);
  *(float *)(rocket + 0x30) = fVar1;
  *(float *)(rocket + 0x1c) = fVar1 * (RwFrame_modelling(frame)).at.x;
  *(float *)(rocket + 0x20) = (RwFrame_modelling(frame)).at.y * fVar1;
  *(float *)(rocket + 0x24) = (RwFrame_modelling(frame)).at.z * fVar1;
  fVar1 = *(float *)(rocket + 0x28) + *(float *)(rocket + 0x20);
  *(float *)(rocket + 0x20) = fVar1;
  v->x = v->x + *(float *)(rocket + 0x1c);
  *(float *)(rocket + 0x10) = fVar1 + *(float *)(rocket + 0x10);
  *(float *)(rocket + 0x14) = *(float *)(rocket + 0x24) + *(float *)(rocket + 0x14);
  RwFrameTranslate(frame,v,rwCOMBINEPOSTCONCAT);
  return;
}

/* entry thunk at the original address of rocket_fly_straight: register inputs (esi) -> natural C rocket_fly_straight_n; ret 0 */
__declspec(naked) void rocket_fly_straight(void)
{
    __asm {
        push esi
        call rocket_fly_straight_n
        add esp, 4
        ret
    }
}
