// cluster-entries: course_helper_45bad0_sub_45b7a0
/* Installed cluster course_helper_45bad0_sub_45b7a0 (tools/cluster_install.py from output/cluster_pilot_kb/course_helper_45bad0_sub_45b7a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd634*/ extern float k_f_2;
/*005ddacc*/ extern float k_f_360;
/*005ddc28*/ extern float k_f_0;
/*005de1f8*/ extern float k_f_3_1415927;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
/* course_helper_45bad0_sub_45b7a0 */

/* single caller course_helper_45bad0 */

void course_helper_45bad0_sub_45b7a0(int param_1)

{
  RwFrame *frame;
  float fVar1;
  int *piVar2;
  float *pfVar3;
  double fVar4;
  double fVar5;
  double sin_twice;
  float half;
  int iStack_24;
  RwV3d RStack_18;
  RwV3d RStack_c;
  
  if (k_f_0 < *(float *)(param_1 + 4)) {
    fVar4 = *(float *)(param_1 + 4) * k_f_0_5;      /* x87: stays extended for the sine, a rounded float copy goes to the rotation */
    half = (float)fVar4;
    iStack_24 = 0;
    RStack_c.x = 0.5;
    fVar5 = sin(fVar4 * k_f_3_1415927);
    sin_twice = fVar5 + fVar5; /* spilled as a double */
    RStack_c.y = 0.5;
    RStack_c.z = 0.5;
    pfVar3 = (float *)(param_1 + 0x18);
    piVar2 = (int *)(param_1 + 0xc);
    do {
      fVar1 = k_f_2 - *(float *)(param_1 + 4);
      frame = *(RwFrame **)(*piVar2 + 4);
      RStack_18.x = fVar1 * *pfVar3 + pfVar3[9];
      RStack_18.z = pfVar3[2] * fVar1 + pfVar3[0xb];
      RStack_18.y = pfVar3[10] + sin_twice;
      RwFrameTranslate(frame,&RStack_18,rwCOMBINEREPLACE);
      RwFrameRotate(frame,&RStack_c,((float)iStack_24 + k_f_4) * half * k_f_360,
                    rwCOMBINEPRECONCAT);
      iStack_24 = iStack_24 + 1;
      piVar2 = piVar2 + 1;
      pfVar3 = pfVar3 + 3;
    } while (iStack_24 < 3);
    *(float *)(param_1 + 4) = *(float *)(param_1 + 4) - g_tick_seconds;
  }
  return;
}

