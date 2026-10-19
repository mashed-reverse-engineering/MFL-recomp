#include "mfl_tier2.h"
/*0040def0*/ extern int color_code_to_index();
/*004221c0*/ extern double vehicle_get_float_184();
/*0049fa40*/ extern undefined __cdecl fx_particle_spawn_from_style(undefined4 p0);
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*005dd330*/ extern float float_1_0;
/*005dd36c*/ extern float k_f_0_4;
/*005dd61c*/ extern float k_f_0_001;
/*005dd62c*/ extern float k_f_100;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005ddad4*/ extern float k_f_90;
/*0064d89c*/ extern undefined4 g_round_64d89c;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];

#define NO_VEHICLE (-1)
#define BRAKE_SLIP_THRESHOLD (0x40)
#define DEFAULT_WHEEL_SCALE (0.85)
#define VEL_RANDOM_MAX_BITS (0x3f666666) /* float 0.9f as raw bits (inferred: random range max for the velocity jitter) */
#define OBJ_STYLE_OFFSET (0x30)
#define OBJ_VEL_X_OFFSET (0x20)
#define OBJ_VEL_Y_OFFSET (0x24)
#define OBJ_VEL_Z_OFFSET (0x28)
/* skid_mark_spawn_from_wheel */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "if the wheel slips (speed>100, drift<90 or brake) spawns a skid/smoke effect via
   effects_update_frame_sub_49fa40" */

/* Spawns one skid/smoke particle for a wheel that is slipping or sliding on the ground.
   wheel: per-wheel record (inferred layout): [0] object pointer (velocity scales at +0x20..+0x28,
   style block at +0x30), [1] surface colour code, [2] vehicle index or NO_VEHICLE,
   [3] wheel index, [4] float[3] wheel position (scaled by 0.001), [5] speed, [6] drift,
   [7] force-spawn flag, [8] strength (drives the particle intensity), [9] player index into g_player_inputs.
   param_2, param_3, param_4 are not used by this function (inferred).
   Returns nothing; the particle is handed to fx_particle_spawn_from_style. */

void __cdecl skid_mark_spawn_from_wheel(int *wheel,char *param_2,char *param_3,char *param_4)

{
  float *pos;
  bool is_slipping;
  float10 temp;
  struct {
    float intensity;
    int vehicle_index;
    int wheel_index;
    float pos_x;
    float pos_y;
    float pos_z;
    int style_data;
    int surface_index;
    float scale;
    undefined4 has_velocity;
    float vel_x;
    float vel_y;
    float vel_z;
  } frame;

  is_slipping = false;
  /* Only while no round is running (g_round_64d89c == 0, inferred), and only if the surface has an index (non-zero). */
  if ((g_round_64d89c == 0) && (frame.surface_index = color_code_to_index(wheel[1]), frame.surface_index != 0)) {
    /* Slipping if forced, going faster than 100, or drifting under 90 while the player brakes hard. */
    if (((wheel[7] != 0) || (k_f_100 < *(float *)&wheel[5])) ||
       ((*(float *)&wheel[6] < k_f_90 && (BRAKE_SLIP_THRESHOLD < g_player_inputs[wheel[9]].brake)))) {
      is_slipping = true;
    }
    /* Not slipping and too weak a strength: nothing to spawn. */
    else if (*(float *)&wheel[8] < k_f_100) {
      return;
    }
    /* Intensity = strength * scale + base, clamped to 1.0. */
    frame.intensity = *(float *)&wheel[8] * k_f_3_3333333en4 + k_f_0_4;
    if (float_1_0 < frame.intensity) {
      frame.intensity = 1.0;
    }
    /* Wheel position, converted to the particle system's units (x 0.001). */
    pos = (float *)wheel[4];
    frame.pos_x = *pos * k_f_0_001;
    frame.pos_y = pos[1] * k_f_0_001;
    frame.pos_z = pos[2] * k_f_0_001;
    /* Particle size: twice the vehicle's float 184 value, or the default when there is no vehicle. */
    frame.scale = DEFAULT_WHEEL_SCALE;
    if (wheel[2] != NO_VEHICLE) {
      temp = (float10)vehicle_get_float_184(wheel[2]);
      frame.scale = (float)(temp + temp);
    }
    /* Early-out filter by surface: surface 6 needs slipping, others need slipping or wheel index <= 1. */
    if (frame.surface_index == 6) {
      if (!is_slipping) {
        return;
      }
    }
    else if ((!is_slipping) && (1 < wheel[3])) {
      return;
    }
    /* Style block of the object, passed along with the particle. */
    frame.style_data = *wheel + OBJ_STYLE_OFFSET;
    frame.has_velocity = 0;
    /* Slipping wheels on surfaces other than 6 and 0xd get a random negative velocity
       (inferred): each axis is -(random in [0,0.9) + 1) * object velocity scale. */
    if (((is_slipping) && (frame.surface_index != 6)) && (frame.surface_index != 0xd)) {
      temp = (float10)rp_random_float_range(0,VEL_RANDOM_MAX_BITS);
      frame.vel_x = (float)(-(temp + (float10)float_1_0) * (float10)*(float *)(*wheel + OBJ_VEL_X_OFFSET));
      temp = (float10)rp_random_float_range(0,VEL_RANDOM_MAX_BITS);
      frame.vel_y = (float)(-(temp + (float10)float_1_0) * (float10)*(float *)(*wheel + OBJ_VEL_Y_OFFSET));
      temp = (float10)rp_random_float_range(0,VEL_RANDOM_MAX_BITS);
      frame.has_velocity = 1;
      frame.vel_z = (float)(-(temp + (float10)float_1_0) * (float10)*(float *)(*wheel + OBJ_VEL_Z_OFFSET));
    }
    /* Vehicle and wheel ids, then spawn. */
    frame.vehicle_index = wheel[2];
    frame.wheel_index = wheel[3];
    fx_particle_spawn_from_style(&frame.intensity);
  }
  return;
}


