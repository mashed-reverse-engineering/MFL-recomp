#include "mfl_tier2.h"
/*00406d40*/ extern int __cdecl fx_object_timer_subtract_by_id(undefined4 p0, undefined4 p1);
/*00419fa0*/ extern double ai_get_float_8c9980();
/*00419fb0*/ extern int ai_bot_set_cooldown_78();
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*00461cc0*/ extern int weapon_particle_spark_emit();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00465f60*/ extern int weapon_effect_get_slot_record();
/*004726a0*/ extern int sfx_play_at();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b7c0*/ extern int car_set_field_144();
/*00481fd0*/ extern double random_range();
/*00486050*/ extern undefined __cdecl random_vector_in_cone(struct RwV3d *p0, float *p1, float p2);
/*004866f0*/ extern int fx_emitter_spawn_c_default();
/*0048e3f0*/ extern int rwp_object_get_id();
/*00492ca0*/ extern int rwp_object_activate();
/*0049d060*/ extern int fx_spawn_burst_default_scale();
/*004a1680*/ extern int player_rumble();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd328*/ extern float k_f_0_6;
/*005dd33c*/ extern float k_f_4;
/*005dd370*/ extern float k_f_0_3;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005ddaf8*/ extern float k_f_n0_3;
/*005ddb04*/ extern float k_f_0_023;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005de2fc*/ extern float k_f_0_04;
/*005de3e4*/ extern float k_f_40;
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/* weapon_machinegun_fire */


/* Fire the machinegun of slot_arg->player: spin up barrel, consume ammo when cooldown is 0, emit sparks/shell casings, sound; branch on whether a target object is hooked (inferred). */
void weapon_machinegun_fire(WeaponSlot *slot_arg)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float aim_scale;
  WeaponSlot *slot;
  int slot_value;
  RwV3d *muzzle_pos;
  RwMatrix *gun_ltm;
  int timer_result;
  undefined4 slot_record;
  uint player;
  bool ammo_even;
  double rnd;
  /* one contiguous frame: rwp_object_activate reads the two vectors at muzzle_dir / shell_vel_x together */
  struct {
    undefined1 shell_r;
    undefined1 shell_g;
    undefined1 shell_b;
    undefined1 shell_a;
    RwV3d muzzle_dir;
    float shell_vel_x;
    float shell_vel_y;
    float shell_vel_z;
    RwV3d shell_pos;
    undefined4 zero_vec_x;
    undefined4 zero_vec_y;
    undefined4 zero_vec_z;
  } frame;
  
  slot = slot_arg;
  frame.zero_vec_x = 0;
  frame.zero_vec_y = 0;
  frame.zero_vec_z = 0;
  frame.shell_vel_x = 1.0;
  frame.shell_vel_y = 1.5;
  frame.shell_vel_z = 1.0;
  frame.shell_r = 0;
  frame.shell_g = 0;
  frame.shell_b = 0;
  frame.shell_a = 0xff;
  slot_value = weapon_effect_get_slot_value_b(slot_arg->player);
  muzzle_pos = (RwV3d *)weapon_effect_get_slot_record(slot->player);
  player = slot->player;
  if (MachinegunState_ARRAY_006a60d0[player].spin < k_f_40) {
    MachinegunState_ARRAY_006a60d0[player].spin = MachinegunState_ARRAY_006a60d0[player].spin + k_f_4;
  }
  player = slot->player;
  if ((MachinegunState_ARRAY_006a60d0[player].cooldown == k_f_0) &&
     (0 < MachinegunState_ARRAY_006a60d0[player].ammo)) {
    MachinegunState_ARRAY_006a60d0[player].cooldown = 0.06;
    MachinegunState_ARRAY_006a60d0[slot->player].ammo =
         MachinegunState_ARRAY_006a60d0[slot->player].ammo + -1;
    weapon_particle_spark_emit(muzzle_pos,0x3f19999a);
    MachinegunState_ARRAY_006a60d0[slot->player].fired = 1;
    player = MachinegunState_ARRAY_006a60d0[slot->player].ammo & 0x80000001;
    ammo_even = player == 0;
    if ((int)player < 0) {
      ammo_even = (player - 1 | 0xfffffffe) == 0xffffffff;
    }
    if (ammo_even) {
      slot_arg = (WeaponSlot *)0xff2184ff;
      gun_ltm = RwFrameGetLTM(*(RwFrame **)
                              (*(int *)&MachinegunState_ARRAY_006a60d0[slot->player].field_0x44 +
                              4));
      frame.shell_pos.x = (gun_ltm->pos).x;
      frame.shell_pos.y = (gun_ltm->pos).y;
      frame.shell_pos.z = (gun_ltm->pos).z;
      frame.muzzle_dir.x = muzzle_pos->x - (gun_ltm->pos).x;
      frame.muzzle_dir.y = muzzle_pos->y - (gun_ltm->pos).y;
      frame.muzzle_dir.z = muzzle_pos->z - (gun_ltm->pos).z;
      RwV3dNormalize(&frame.muzzle_dir,&frame.muzzle_dir);
      frame.shell_vel_x = frame.muzzle_dir.x * k_f_0_6;
      frame.shell_vel_y = frame.muzzle_dir.y * k_f_0_6;
      frame.shell_vel_z = frame.muzzle_dir.z * k_f_0_6;
      frame.shell_pos.x = frame.muzzle_dir.x + frame.muzzle_dir.x + frame.shell_pos.x;
      frame.shell_pos.y = frame.muzzle_dir.y + frame.muzzle_dir.y + frame.shell_pos.y;
      frame.shell_pos.z = frame.muzzle_dir.z + frame.muzzle_dir.z + frame.shell_pos.z;
      fx_spawn_burst_default_scale(&frame.shell_pos,1,&frame.shell_vel_x,&slot_arg,&frame.zero_vec_x,0x40a00000);
    }
    if (slot_value == -1) {
      slot_value = weapon_effect_get_slot_value_a(slot->player);
      if (slot_value != -1) {
        rnd = (double)ai_get_float_8c9980();
        timer_result = fx_object_timer_subtract_by_id
                          (slot_value,(fbits1 = (float)((double)k_f_0_9 - rnd * (double)k_f_0_04), *(int *)&fbits1));
        if (timer_result == 0) {
          frame.muzzle_dir.x = slot->aim_vector[0];
          frame.muzzle_dir.y = slot->aim_vector[1];
          frame.muzzle_dir.z = slot->aim_vector[2];
          rnd = (double)rp_random_float_range(0x3ca3d70a,0x3d75c28f);
          fx_emitter_spawn_c_default(muzzle_pos,0,0x43b40000,(fbits2 = (float)rnd, *(int *)&fbits2));
          timer_result = rwp_object_get_id(slot_value);
          if (timer_result < 150000) {
            frame.muzzle_dir.y = frame.muzzle_dir.y * float_10_0;
            aim_scale = float_10_0;
          }
          else {
            frame.muzzle_dir.y = 100.0;
            aim_scale = k_f_100;
          }
          frame.muzzle_dir.x = frame.muzzle_dir.x * aim_scale;
          frame.muzzle_dir.z = frame.muzzle_dir.z * aim_scale;
          slot_record = weapon_effect_get_slot_record(slot->player);
          rwp_object_activate(slot_value,&frame.muzzle_dir,slot_record);
        }
      }
      frame.shell_vel_x = 0.0;
      frame.shell_vel_y = 1.0;
      frame.shell_vel_z = 0.0;
      slot_value = 4;
      do {
        random_vector_in_cone(&frame.shell_pos,&frame.shell_vel_x,10.0);
        frame.shell_pos.x = frame.shell_pos.x * k_f_0_023;
        frame.shell_pos.y = frame.shell_pos.y * k_f_0_023;
        frame.shell_pos.z = frame.shell_pos.z * k_f_0_023;
        fx_spawn_burst_default_scale(muzzle_pos,1,&frame.shell_pos,&frame.shell_r,&frame.zero_vec_x,0x40000000);
        slot_value = slot_value + -1;
      } while (slot_value != 0);
    }
    else {
      rnd = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.shell_vel_x = (float)rnd;
      rnd = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.shell_vel_y = (float)rnd;
      rnd = (double)rp_random_float_range(0xbf800000,0x3f800000);
      frame.shell_vel_z = (float)rnd;
      timer_result = 4;
      do {
        random_vector_in_cone(&frame.shell_pos,&frame.shell_vel_x,10.0);
        frame.shell_pos.x = frame.shell_pos.x * k_f_0_023;
        frame.shell_pos.y = frame.shell_pos.y * k_f_0_023;
        frame.shell_pos.z = frame.shell_pos.z * k_f_0_023;
        fx_spawn_burst_default_scale(muzzle_pos,1,&frame.shell_pos,0,&frame.zero_vec_x,0x40800000);
        timer_result = timer_result + -1;
      } while (timer_result != 0);
      rnd = (double)rp_random_float_range(0x3d75c28f,0x3dcccccd);
      fx_emitter_spawn_c_default(muzzle_pos,0,0x43b40000,(fbits3 = (float)rnd, *(int *)&fbits3));
      player_rumble(slot_value,1,2,0xff);
      rnd = (double)random_range(0xc3480000,0x43480000);
      frame.muzzle_dir.x = (RwReal)rnd;
      frame.muzzle_dir.y = 0.0;
      rnd = (double)random_range(0xc3480000,0x43480000);
      frame.muzzle_dir.z = (RwReal)rnd;
      car_add_velocity(&frame.muzzle_dir,slot_value);
      frame.muzzle_dir.x = 0.0;
      rnd = (double)random_range(0xbf4ccccd,0x3f4ccccd);
      frame.muzzle_dir.y = (RwReal)rnd;
      if ((!((double)k_f_0_3 > rnd)) || (!(frame.muzzle_dir.y > k_f_0))) {
        if ((k_f_n0_3 < frame.muzzle_dir.y) && (frame.muzzle_dir.y < k_f_0)) {
          frame.muzzle_dir.y = -0.3;
        }
      }
      else {
        frame.muzzle_dir.y = 0.3;
      }
      frame.muzzle_dir.z = 0.0;
      car_set_field_144(&frame.muzzle_dir,slot_value);
      ai_bot_set_cooldown_78(slot_value);
      car_damage_at_world_point(slot_value,0x3f400000,muzzle_pos);
    }
    sfx_play_at(0x13,slot->muzzle_position);
  }
  return;
}


