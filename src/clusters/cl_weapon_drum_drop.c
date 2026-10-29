// cluster-entries: weapon_drum_drop
/* Installed cluster weapon_drum_drop (tools/cluster_install.py from output/cluster_pilot_hb1/weapon_drum_drop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005de3e4*/ extern float k_f_40;
/*005df4d0*/ extern float k_f_3_5;
/* weapon_drum_drop: the depth charge launcher releases its last remaining drum.
 *
 * `launcher` is the weapon state: +0 / +4 the pointers of the two drums (cleared as they are dropped), +8 the number of drums
 * left, +0xc the owning vehicle record (position at +0x10, direction at +0x90), +0x18 / +0x1c the frame holders the drums hang
 * from, +0x24 / +0x28 per drum state words (set to 4 when the drum with that index is dropped; meaning inferred).
 *
 * (Field meanings below are inferred from use.)  The drum is detached from the launcher frame, placed at the vehicle position and given the vehicle direction times 40 plus
 * the vehicle position times 3.5 as its velocity, then a puff of trail particles is spawned at the drum. */

typedef struct DrumRecord { /* one depth charge drum (weapon_drum_update_one) */
  char unknown_00[0x10];
  RwV3d pos;          /* 0x10 */
  RwV3d vel;          /* 0x1c: car direction * 40 + car velocity once dropped */
  int state;          /* 0x28: 0 = detached */
  int phase;          /* 0x2c: 2 = falling */
  char unknown_30[4];
  RwFrame *frame;     /* 0x34 */
} DrumRecord;

void weapon_drum_drop_n(char *launcher)

{
  char *owner;
  char *frame_ltm;
  DrumRecord *drum;
  float dir_y;
  float dir_z;
  float scaled_x; /* vehicle position * 3.5 (added to the drum velocity) */
  float scaled_y;
  float scaled_z;
  RwMatrix *ltm;
  int count;
  int owner_ptr;
  RwOpCombineType combine_op;
  struct {
    undefined4 spawn_x;
    undefined4 spawn_y;
    undefined4 spawn_z;
    undefined4 fx_colour;
    undefined4 fx_size_start;
    undefined4 fx_size_end;
    undefined4 fx_vel_x;
    undefined4 fx_vel_y;
    undefined4 fx_vel_z;
    undefined4 fx_accel_x;
    undefined4 fx_accel_y;
    undefined4 fx_accel_z;
    undefined4 fx_life_scale;
    undefined1 fx_field_34[4];
    undefined4 fx_field_38;
    undefined1 fx_field_3c[4];
    undefined4 fx_flags;
    undefined1 fx_flag_44[1];
    undefined1 fx_kind;
  } frame;   /* spawn_x .. fx_kind: the 0x48 byte particle record of fx_trail_particle_spawn_interpolated (layout as FxParticle in cl_fx_dust_particle_spawn.c) */
  
  owner = *(char **)(launcher + 0xc);
  scaled_x = *(float *)(owner + 0x10) * k_f_3_5;
  scaled_y = *(float *)(owner + 0x14) * k_f_3_5;
  count = *(int *)(launcher + 8);
  scaled_z = *(float *)(owner + 0x18) * k_f_3_5;
  if (count == 2) {
    *(undefined4 *)(launcher + 0x24) = 4;
  }
  else if (count == 1) {
    *(undefined4 *)(launcher + 0x28) = 4;
  }
  *(int *)(launcher + 8) = count + -1;
  drum = *(DrumRecord **)(launcher + (count + -1) * 4);
  drum->state = 0;
  ltm = RwFrameGetLTM(drum->frame);
  drum->pos.x = (ltm->pos).x;
  drum->pos.y = (ltm->pos).y;
  drum->pos.z = (ltm->pos).z;
  RwFrameRemoveChild(drum->frame);
  if (*(int *)(launcher + 8) == 1) {
    owner_ptr = *(int *)(launcher + 0x1c);
  }
  else {
    if (*(int *)(launcher + 8) != 0) goto LAB_0045f9a3;
    owner_ptr = *(int *)(launcher + 0x18);
  }
  combine_op = rwCOMBINEREPLACE;
  ltm = RwFrameGetLTM(*(RwFrame **)(owner_ptr + 4));
  RwFrameTransform(drum->frame,ltm,combine_op);
LAB_0045f9a3:
  drum->phase = 2;
  owner_ptr = *(int *)(launcher + 0xc);
  dir_y = *(float *)(owner_ptr + 0x94);
  dir_z = *(float *)(owner_ptr + 0x98);
  drum->vel.x = *(float *)(owner_ptr + 0x90) * k_f_40 + scaled_x;
  drum->vel.y = dir_y * k_f_40 + scaled_y;
  drum->vel.z = dir_z * k_f_40 + scaled_z;
  *(undefined4 *)(launcher + *(int *)(launcher + 8) * 4) = 0;
  frame_ltm = (char *)drum->frame;
  frame.spawn_x = *(undefined4 *)(frame_ltm + 0x40);
  frame.spawn_y = *(undefined4 *)(frame_ltm + 0x44);
  frame.spawn_z = *(undefined4 *)(frame_ltm + 0x48);
  frame.fx_kind = 5;
  frame.fx_life_scale = 0x41480000;
  frame.fx_colour = 0x40ffffff;
  frame.fx_flags = 0x1120;
  frame.fx_size_start = 0x3f99999a;
  frame.fx_size_end = 0x3f99999a;
  frame.fx_field_38 = 0x3f800000;
  frame.fx_vel_x = 0;
  frame.fx_vel_y = 0;
  frame.fx_vel_z = 0;
  frame.fx_accel_x = 0;
  frame.fx_accel_y = 0;
  frame.fx_accel_z = 0;
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
