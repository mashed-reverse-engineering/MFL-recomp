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

/* "updates one flame record: frame transform, scale and its sound" */

void weapon_flame_particle_update_n(char *flame)

{
  RwFrame *frame;
  RwMatrix *frame_ltm;
  float10 size_jitter;
  float10 pulse_scale;
  struct {
    float base_scale;
    RwV3d scale_vec;
    RwReal voice_pos_x;
    RwReal voice_pos_y;
    RwReal voice_pos_z;
  } frame_obj;
  
  frame = *(RwFrame **)(*(int *)(flame + 0x24) + 4);
  frame_obj.scale_vec.x = 1.0;
  frame_obj.scale_vec.y = 1.0;
  frame_obj.scale_vec.z = 1.0;
  frame_obj.base_scale = 1.0;
  /* state +0x1c == 1: grow flame intensity (+0xc, capped near 5.0) and base scale 1.5; otherwise shrink it toward 1.0 (inferred) */
  if (*(int *)(flame + 0x1c) == 1) {
    if (*(float *)(flame + 0xc) < float_5_0) {
      *(float *)(flame + 0xc) = *(float *)(flame + 0xc) + k_f_0_5;
    }
    frame_obj.base_scale = 1.5;
  }
  else if (float_1_0 < *(float *)(flame + 0xc)) {
    frame_obj.base_scale = 1.0;
    *(float *)(flame + 0xc) = *(float *)(flame + 0xc) - k_f_0_5;
  }
  /* flame sound at the frame's world position, volume = intensity * 0.1818 (inferred) */
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(*(int *)(flame + 0x24) + 4));
  frame_obj.voice_pos_x = (frame_ltm->pos).x;
  frame_obj.voice_pos_y = (frame_ltm->pos).y;
  frame_obj.voice_pos_z = (frame_ltm->pos).z;
  sfx_set_voice_volume
            (0x16,&frame_obj.voice_pos_x,*(int *)(*(int *)(flame + 4) + 0xb0),
             *(float *)(flame + 0xc) * k_f_0_18181819);
  /* random jitter added to the scale each update (inferred) */
  size_jitter = (float10)random_range(0.0,0.1);
  pulse_scale = (float10)random_range(0.0,0.3);
  pulse_scale = pulse_scale + (float10)*(float *)(flame + 0xc);
  *(float *)(flame + 0xc) = (float)pulse_scale;
  frame_obj.scale_vec.z = (RwReal)pulse_scale;
  frame_obj.scale_vec.x = (float)(size_jitter + (float10)frame_obj.base_scale);
  frame_obj.scale_vec.y = (float)(size_jitter + (float10)frame_obj.base_scale);
  /* apply the stored matrix (flame+0x28), then scale by scale_vec */
  RwFrameTransform(frame,(RwMatrix *)(flame + 0x28),rwCOMBINEREPLACE);
  RwFrameScale(frame,&frame_obj.scale_vec,rwCOMBINEPRECONCAT);
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
