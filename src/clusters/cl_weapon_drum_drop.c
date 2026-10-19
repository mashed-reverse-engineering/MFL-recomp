// cluster-entries: weapon_drum_drop
/* Installed cluster weapon_drum_drop (tools/cluster_install.py from output/cluster_pilot_hb1/weapon_drum_drop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005de3e4*/ extern float k_f_40;
/*005df4d0*/ extern float k_f_3_5;
/* weapon_drum_drop */

/* WARNING: Unknown calling convention */
/* "detaches the depth charge drum frame from the car and starts its fall (particles at the car
   position)" */

void weapon_drum_drop_n(char *in_edi)

{
  char *iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  RwMatrix *pRVar7;
  int iVar8;
  RwOpCombineType combine;
  struct {
    undefined4 local_48;
    undefined4 local_44;
    undefined4 local_40;
    undefined4 local_3c;
    undefined4 local_38;
    undefined4 local_34;
    undefined4 local_30;
    undefined4 local_2c;
    undefined4 local_28;
    undefined4 local_24;
    undefined4 local_20;
    undefined4 local_1c;
    undefined4 local_18;
    undefined1 pad_14[4];
    undefined4 local_10;
    undefined1 pad_c[4];
    undefined4 local_8;
    undefined1 pad_4[1];
    undefined1 local_3;
  } frame;
  
  iVar1 = *(char **)(in_edi + 0xc);
  fVar4 = *(float *)(iVar1 + 0x10) * k_f_3_5;
  fVar5 = *(float *)(iVar1 + 0x14) * k_f_3_5;
  iVar8 = *(int *)(in_edi + 8);
  fVar6 = *(float *)(iVar1 + 0x18) * k_f_3_5;
  if (iVar8 == 2) {
    *(undefined4 *)(in_edi + 0x24) = 4;
  }
  else if (iVar8 == 1) {
    *(undefined4 *)(in_edi + 0x28) = 4;
  }
  *(int *)(in_edi + 8) = iVar8 + -1;
  iVar1 = *(char **)(in_edi + (iVar8 + -1) * 4);
  *(undefined4 *)(iVar1 + 0x28) = 0;
  pRVar7 = RwFrameGetLTM(*(RwFrame **)(iVar1 + 0x34));
  *(RwReal *)(iVar1 + 0x10) = (pRVar7->pos).x;
  *(RwReal *)(iVar1 + 0x14) = (pRVar7->pos).y;
  *(RwReal *)(iVar1 + 0x18) = (pRVar7->pos).z;
  RwFrameRemoveChild(*(RwFrame **)(iVar1 + 0x34));
  if (*(int *)(in_edi + 8) == 1) {
    iVar8 = *(int *)(in_edi + 0x1c);
  }
  else {
    if (*(int *)(in_edi + 8) != 0) goto LAB_0045f9a3;
    iVar8 = *(int *)(in_edi + 0x18);
  }
  combine = rwCOMBINEREPLACE;
  pRVar7 = RwFrameGetLTM(*(RwFrame **)(iVar8 + 4));
  RwFrameTransform(*(RwFrame **)(iVar1 + 0x34),pRVar7,combine);
LAB_0045f9a3:
  *(undefined4 *)(iVar1 + 0x2c) = 2;
  iVar8 = *(int *)(in_edi + 0xc);
  fVar2 = *(float *)(iVar8 + 0x94);
  fVar3 = *(float *)(iVar8 + 0x98);
  *(float *)(iVar1 + 0x1c) = *(float *)(iVar8 + 0x90) * k_f_40 + fVar4;
  *(float *)(iVar1 + 0x20) = fVar2 * k_f_40 + fVar5;
  *(float *)(iVar1 + 0x24) = fVar3 * k_f_40 + fVar6;
  *(undefined4 *)(in_edi + *(int *)(in_edi + 8) * 4) = 0;
  iVar1 = *(char **)(iVar1 + 0x34);
  frame.local_48 = *(undefined4 *)(iVar1 + 0x40);
  frame.local_44 = *(undefined4 *)(iVar1 + 0x44);
  frame.local_40 = *(undefined4 *)(iVar1 + 0x48);
  frame.local_3 = 5;
  frame.local_18 = 0x41480000;
  frame.local_3c = 0x40ffffff;
  frame.local_8 = 0x1120;
  frame.local_38 = 0x3f99999a;
  frame.local_34 = 0x3f99999a;
  frame.local_10 = 0x3f800000;
  frame.local_30 = 0;
  frame.local_2c = 0;
  frame.local_28 = 0;
  frame.local_24 = 0;
  frame.local_20 = 0;
  frame.local_1c = 0;
  fx_trail_particle_spawn_interpolated(&frame.local_48,0,0,0);
  return;
}

/* entry thunk at the original address of weapon_drum_drop: register inputs (edi) -> natural C weapon_drum_drop_n; ret 0 */
__declspec(naked) void weapon_drum_drop(void)
{
    __asm {
        push edi
        call weapon_drum_drop_n
        add esp, 4
        ret
    }
}
