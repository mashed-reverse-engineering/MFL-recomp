// cluster-entries: weapon_flame_particle_update
/* Installed cluster weapon_flame_particle_update (tools/cluster_install.py from output/cluster_pilot_hb1/weapon_flame_particle_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00472950*/ extern int sfx_set_voice_volume(int voice_kind, float *position, int emitter, float volume);
/*00481fd0*/ extern double random_range(float lo, float hi);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005df8d8*/ extern float k_f_0_18181819;
/* weapon_flame_particle_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "updates one flame record: frame transform, scale and its sound" */

void weapon_flame_particle_update_n(char *in_eax)

{
  RwFrame *frame;
  RwMatrix *pRVar1;
  float10 fVar2;
  float10 fVar3;
  struct {
    float local_1c;
    RwV3d local_18;
    RwReal local_c;
    RwReal local_8;
    RwReal local_4;
  } frame_obj;
  
  frame = *(RwFrame **)(*(int *)(in_eax + 0x24) + 4);
  frame_obj.local_18.x = 1.0;
  frame_obj.local_18.y = 1.0;
  frame_obj.local_18.z = 1.0;
  frame_obj.local_1c = 1.0;
  if (*(int *)(in_eax + 0x1c) == 1) {
    if (*(float *)(in_eax + 0xc) < float_5_0) {
      *(float *)(in_eax + 0xc) = *(float *)(in_eax + 0xc) + k_f_0_5;
    }
    frame_obj.local_1c = 1.5;
  }
  else if (float_1_0 < *(float *)(in_eax + 0xc)) {
    frame_obj.local_1c = 1.0;
    *(float *)(in_eax + 0xc) = *(float *)(in_eax + 0xc) - k_f_0_5;
  }
  pRVar1 = RwFrameGetLTM(*(RwFrame **)(*(int *)(in_eax + 0x24) + 4));
  frame_obj.local_c = (pRVar1->pos).x;
  frame_obj.local_8 = (pRVar1->pos).y;
  frame_obj.local_4 = (pRVar1->pos).z;
  sfx_set_voice_volume
            (0x16,&frame_obj.local_c,*(int *)(*(int *)(in_eax + 4) + 0xb0),
             *(float *)(in_eax + 0xc) * k_f_0_18181819);
  fVar2 = (float10)random_range(0.0,0.1);
  fVar3 = (float10)random_range(0.0,0.3);
  fVar3 = fVar3 + (float10)*(float *)(in_eax + 0xc);
  *(float *)(in_eax + 0xc) = (float)fVar3;
  frame_obj.local_18.z = (RwReal)fVar3;
  frame_obj.local_18.x = (float)(fVar2 + (float10)frame_obj.local_1c);
  frame_obj.local_18.y = (float)(fVar2 + (float10)frame_obj.local_1c);
  RwFrameTransform(frame,(RwMatrix *)(in_eax + 0x28),rwCOMBINEREPLACE);
  RwFrameScale(frame,&frame_obj.local_18,rwCOMBINEPRECONCAT);
  return;
}

/* entry thunk at the original address of weapon_flame_particle_update: register inputs (eax) -> natural C weapon_flame_particle_update_n; ret 0 */
__declspec(naked) void weapon_flame_particle_update(void)
{
    __asm {
        push eax
        call weapon_flame_particle_update_n
        add esp, 4
        ret
    }
}
