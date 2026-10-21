// cluster-entries: hudpanel_marker_place
/* Installed cluster hudpanel_marker_place (tools/cluster_install.py from output/cluster_pilot_uc3/hudpanel_marker_place.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, undefined4 p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005ddad4*/ extern float k_f_90;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005de26c*/ extern float k_f_1_2;
/*005de284*/ extern float k_f_1_1111112;
/*00603ea4*/ extern undefined4 g_marker_particles_603ea4;
/* hudpanel_marker_place */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "builds the matrices of a player marker (translate / scale / rotate) and applies them to its
   frame" */

static float marker_x_f32;
void hudpanel_marker_place_n(int in_eax)

{
  int iVar1;
  float fVar2;
  uint uVar3;
  uint uVar4;
  float *pfVar5;
  float *px;
  int iVar6;
  int *piVar7;
  struct {
    float local_74;
    RwV3d local_70;
    RwV3d local_64;
    RwV3d local_58;
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  iVar6 = *(int *)(in_eax + 0x50);
  px = &frame.local_70.x;
  frame.local_70.x = 0.0;
  frame.local_70.y = 0.0;
  frame.local_70.z = 1.0;
  frame.local_58.x = 0.0;
  frame.local_58.y = 0.0;
  frame.local_58.z = 1.0;
  frame.local_64.x = 1.0;
  frame.local_64.y = 1.0;
  frame.local_64.z = 1.0;
  if (iVar6 == 0) {
    return;
  }
  *(undefined4 *)(in_eax + 0x4c) = 0;
  if (*(int *)(iVar6 + 0x34) == 2) {
    *(undefined4 *)(in_eax + 0x4c) = 1;
  }
  if (*(int *)(iVar6 + 0x60) == 0) {
    if (*(int *)(iVar6 + 0x58) == 0) goto LAB_0041c171;
    uVar3 = *(uint *)(in_eax + 0x4c) | 2;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x4c) | 4;
  }
  *(uint *)(in_eax + 0x4c) = uVar3;
LAB_0041c171:
  uVar3 = *(uint *)(in_eax + 0x4c);
  iVar6 = 0;
  do {
    iVar1 = *(int *)(in_eax + 0x20 + iVar6 * 4);
    *(undefined4 *)(in_eax + iVar6 * 4) = 0;
    if ((iVar1 != 0) && ((uVar3 & 1) != 0)) {
      switch(iVar6) {
      case 0:
        uVar4 = 1;
        break;
      case 1:
        uVar4 = uVar3 & 4;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
        uVar4 = ~(uVar3 >> 2) & 1;
        break;
      default:
        uVar4 = 0;
      }
      *(uint *)(in_eax + iVar6 * 4) = uVar4;
      FUN_004c5380(iVar1,*(int *)(in_eax + 0x50) + 0x10);
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 8);
  pfVar5 = *(float **)(in_eax + 0x50);
  frame.local_74 = pfVar5[0x17] * k_f_1_1111112;
  if ((float_1_0 <= frame.local_74) || (k_f_0 <= frame.local_74)) {
    if (float_1_0 <= frame.local_74) {
      frame.local_74 = 1.0;
    }
  }
  else {
    frame.local_74 = 0.0;
  }
  frame.local_40.flags = frame.local_40.flags | 0x20003;
  frame.local_64.y = 1.0;
  frame.local_64.x = 1.0;
  frame.local_40.at.z = 1.0;
  marker_x_f32 = (((float_1_0 - *pfVar5) + (float_1_0 - *pfVar5)) - float_1_0) * k_f_0_5;
  frame.local_40.up.y = 1.0;
  frame.local_40.right.x = 1.0;
  frame.local_40.up.x = 0.0;
  frame.local_40.right.z = 0.0;
  frame.local_40.right.y = 0.0;
  frame.local_70.x = marker_x_f32 * k_f_1_2;
  frame.local_70.y = (((float_1_0 - pfVar5[1]) + (float_1_0 - pfVar5[1])) - float_1_0) * k_f_0_5 *
               k_f_0_9;
  frame.local_40.at.y = 0.0;
  frame.local_40.at.x = 0.0;
  frame.local_40.up.z = 0.0;
  frame.local_40.pos.z = 0.0;
  frame.local_40.pos.y = 0.0;
  frame.local_40.pos.x = 0.0;
  RwMatrixTranslate(&frame.local_40,&frame.local_70,rwCOMBINEREPLACE);
  RwMatrixScale(&frame.local_40,&frame.local_64,rwCOMBINEPRECONCAT);
  RwFrameTransform(*(RwFrame **)(in_eax + 0x44),&frame.local_40,rwCOMBINEREPLACE);
  frame.local_40.flags = frame.local_40.flags | 0x20003;
  frame.local_40.at.z = 1.0;
  frame.local_40.up.y = 1.0;
  frame.local_40.right.x = 1.0;
  frame.local_40.up.x = 0.0;
  frame.local_40.right.z = 0.0;
  frame.local_40.right.y = 0.0;
  frame.local_40.at.y = 0.0;
  frame.local_40.at.x = 0.0;
  frame.local_40.up.z = 0.0;
  frame.local_40.pos.z = 0.0;
  frame.local_40.pos.y = 0.0;
  frame.local_40.pos.x = 0.0;
  RwMatrixRotate(&frame.local_40,&frame.local_58,frame.local_74 * k_f_90,rwCOMBINEPRECONCAT);
  piVar7 = (int *)(in_eax + 0x28);
  RwFrameTransform(*(RwFrame **)(*(int *)(*(int *)(in_eax + 0x28) + 4) + 4),&frame.local_40,
                   rwCOMBINEREPLACE);
  pfVar5 = (float *)&g_marker_particles_603ea4;
  fVar2 = (float_1_0 - frame.local_74) * k_f_0_025;
  do {
    frame.local_4c.x = pfVar5[-1] * fVar2;
    frame.local_4c.y = fVar2 * *pfVar5;
    frame.local_4c.z = pfVar5[1] * fVar2;
    RwFrameTranslate(*(RwFrame **)(*piVar7 + 4),&frame.local_4c,rwCOMBINEREPLACE);
    pfVar5 = pfVar5 + 3;
    piVar7 = piVar7 + 1;
  } while ((int)pfVar5 < (int)&g_marker_particles_603ea4 + 0x25);
  return;
}

/* entry thunk at the original address of hudpanel_marker_place: register inputs (eax) -> natural C hudpanel_marker_place_n; ret 0 */
__declspec(naked) void hudpanel_marker_place(void)
{
    __asm {
        push eax
        call hudpanel_marker_place_n
        add esp, 4
        ret
    }
}
