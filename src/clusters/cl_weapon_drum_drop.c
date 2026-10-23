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

void weapon_drum_drop_n(char *launcher)

{
  char *rec_ptr;
  float fVar2;
  float fVar3;
  float scaled_x;
  float scaled_y;
  float scaled_z;
  RwMatrix *ltm;
  int count_or_frame;
  RwOpCombineType combine_op;
  struct {
    undefined4 spawn_x;
    undefined4 spawn_y;
    undefined4 spawn_z;
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
  
  rec_ptr = *(char **)(launcher + 0xc);
  scaled_x = *(float *)(rec_ptr + 0x10) * k_f_3_5;
  scaled_y = *(float *)(rec_ptr + 0x14) * k_f_3_5;
  count_or_frame = *(int *)(launcher + 8);
  scaled_z = *(float *)(rec_ptr + 0x18) * k_f_3_5;
  if (count_or_frame == 2) {
    *(undefined4 *)(launcher + 0x24) = 4;
  }
  else if (count_or_frame == 1) {
    *(undefined4 *)(launcher + 0x28) = 4;
  }
  *(int *)(launcher + 8) = count_or_frame + -1;
  rec_ptr = *(char **)(launcher + (count_or_frame + -1) * 4);
  *(undefined4 *)(rec_ptr + 0x28) = 0;
  ltm = RwFrameGetLTM(*(RwFrame **)(rec_ptr + 0x34));
  *(RwReal *)(rec_ptr + 0x10) = (ltm->pos).x;
  *(RwReal *)(rec_ptr + 0x14) = (ltm->pos).y;
  *(RwReal *)(rec_ptr + 0x18) = (ltm->pos).z;
  RwFrameRemoveChild(*(RwFrame **)(rec_ptr + 0x34));
  if (*(int *)(launcher + 8) == 1) {
    count_or_frame = *(int *)(launcher + 0x1c);
  }
  else {
    if (*(int *)(launcher + 8) != 0) goto LAB_0045f9a3;
    count_or_frame = *(int *)(launcher + 0x18);
  }
  combine_op = rwCOMBINEREPLACE;
  ltm = RwFrameGetLTM(*(RwFrame **)(count_or_frame + 4));
  RwFrameTransform(*(RwFrame **)(rec_ptr + 0x34),ltm,combine_op);
LAB_0045f9a3:
  *(undefined4 *)(rec_ptr + 0x2c) = 2;
  count_or_frame = *(int *)(launcher + 0xc);
  fVar2 = *(float *)(count_or_frame + 0x94);
  fVar3 = *(float *)(count_or_frame + 0x98);
  *(float *)(rec_ptr + 0x1c) = *(float *)(count_or_frame + 0x90) * k_f_40 + scaled_x;
  *(float *)(rec_ptr + 0x20) = fVar2 * k_f_40 + scaled_y;
  *(float *)(rec_ptr + 0x24) = fVar3 * k_f_40 + scaled_z;
  *(undefined4 *)(launcher + *(int *)(launcher + 8) * 4) = 0;
  rec_ptr = *(char **)(rec_ptr + 0x34);
  frame.spawn_x = *(undefined4 *)(rec_ptr + 0x40);
  frame.spawn_y = *(undefined4 *)(rec_ptr + 0x44);
  frame.spawn_z = *(undefined4 *)(rec_ptr + 0x48);
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
  fx_trail_particle_spawn_interpolated(&frame.spawn_x,0,0,0);
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
