#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0040fdc0*/ extern int race_get_phase();
/*004146a0*/ extern int trail_get_value();
/*00414760*/ extern undefined __cdecl trail_get_triple(undefined4 p0, undefined4 p1, undefined4 p2);
/*00414f00*/ extern int trail_update_00414f00();
/*00422350*/ extern int vehicle_part_ltm();
/*004246f0*/ extern int car_set_damage();
/*00426b40*/ extern int car_kill();
/*0046fc20*/ extern int sound_bomb_countdown_voices();
/*00475730*/ extern int get_viewport_camera();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048e3b0*/ extern int rwp_object_set_user_data();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse();
/*00498f40*/ extern int fx_ring_add_point();
/*004b2b48*/ extern int _ftol();
/*004b2f90*/ extern double FUN_004b2f90();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*005dd330*/ extern float float_1_0;
/*005dd7e4*/ extern float k_f_0_03;
/*005ddc28*/ extern float k_f_0;
/*005fb0a8*/ extern float g_bomb_5fb0a8;
/*005fb0ac*/ extern float g_course_5fb0ac;
/*005fb0b0*/ extern undefined4 g_bomb_5fb0b0;
/*0062ac74*/ extern float g_car_62ac74;
/*0064d8dc*/ extern undefined4 g_bomb_64d8dc[0x1000];
/*0064d8e0*/ extern undefined4 g_bomb_64d8e0;
/*0064d8e4*/ extern undefined4 g_course_64d8e4;
/*0064d8f4*/ extern undefined4 g_course_64d8f4;
/*0064d908*/ extern undefined4 g_bomb_update_64d908;
/*0064d91c*/ extern undefined4 g_bomb_update_64d91c;
/*0064d930*/ extern undefined4 g_bomb_update_64d930;
/*0064d944*/ extern undefined4 g_bomb_update_64d944;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7c8*/ extern undefined4 g_bomb_7ef7c8;
/*007ef7cc*/ extern float g_course_7ef7cc;
/*007ef7f4*/ extern float g_tick_seconds;
/*0041a370*/ extern float ai_get_counter();
#define BOMB_MOUNT_PART (0x16) /* vehicle part the bomb is mounted on (inferred) */
#define RACE_PHASE_DETONATION (6) /* race_get_phase() value in which the bomb can go off (inferred) */
#define TRAIL_FLAG_CULL_TEST (0x2) /* user-data bit: trail object is frustum tested (inferred) */
#define BOMB_TRIGGER_TABLE_BYTES (0x78) /* 6 trigger records of 0x14 bytes (inferred) */
#define TRIGGER_HIT_VALUE_BITS (0x3fcccccd) /* float 1.6f written to a trigger on hit (inferred) */
#define BOMB_DAMAGE_BITS (0x42480000) /* float 50.0f, car_set_damage amount (inferred) */
#define BOMB_BLAST_FX_SCALE_BITS (0x43b40000) /* float 360.0f, fx_emitter_spawn_a_default scale (inferred) */
#define FX_RING_LARGE_BITS (0x40000000) /* float 2.0f */
#define FX_RING_SMALL_BITS (0x3f800000) /* float 1.0f */

/* bomb_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "Beat the Bomb: counts the bomb timer (g_race 0x7EF7CC) down, normalised in 0x5FB0A8; at 0 calls
   car_kill / car_enter_rwp_mode on the carrier and plays the explosion" */
/* Per frame. Counts the bomb timer down, blows up the carrier car when the timer runs out in the
   detonation phase, then tests the trail points of every trigger against the camera frustum and
   calls trail_update_00414f00 if any point was outside. No parameters, no return value. */

void bomb_update(void)

{
  bool any_outside_frustum;
  int part_matrix;
  int race_phase;
  undefined4 trail_object;
  uint user_data;
  RwCamera *camera;
  RwFrustumTestResult frustum_result;
  int *trigger;
  float *part_position;
  float10 counter_value;
  RwSphere *sphere;
  /* frame: copy of the bomb mount position (pos_x..pos_z) and a blast sphere (sphere) */
  struct {
    float pos_x;
    undefined4 pos_y;
    float pos_z;
    RwSphere sphere;
  } frame;
  
  any_outside_frustum = false;
  /* part_matrix: LTM of the bomb mount part; later reused as the trigger index in the loop below */
  part_matrix = vehicle_part_ltm(0,BOMB_MOUNT_PART);
  /* Count the timer down one tick, then normalise it to g_bomb_5fb0a8 (clamped to [0,1]).
     Below 0 plays voice 3 every frame (inferred: timeout). */
  g_course_7ef7cc = g_course_7ef7cc - g_tick_seconds;
  g_bomb_5fb0a8 = g_course_7ef7cc / g_course_5fb0ac;
  if (float_1_0 < g_bomb_5fb0a8) {
    g_bomb_5fb0a8 = 1.0;
  }
  else if (g_bomb_5fb0a8 < k_f_0) {
    g_bomb_5fb0a8 = 0.0;
    sound_bomb_countdown_voices(3,0);
  }
  /* Voice 2 gets the normalised timer as raw float bits (inferred: countdown). */
  sound_bomb_countdown_voices(2,*(int *)&g_bomb_5fb0a8);
  /* When the ai counter differs from the value stored in g_bomb_5fb0b0, clear the trigger hit
     flags and store the new counter (inferred: new race or restart). */
  counter_value = (float10)ai_get_counter(0);
  counter_value = (float10)FUN_004b2f90((double)counter_value);
  if ((float10)(int)g_bomb_5fb0b0 != counter_value) {
    g_bomb_64d8e0 = 0;
    g_course_64d8f4 = 0;
    g_bomb_update_64d908 = 0;
    g_bomb_update_64d91c = 0;
    g_bomb_update_64d930 = 0;
    g_bomb_update_64d944 = 0;
    counter_value = (float10)ai_get_counter(0x0);
    counter_value = (float10)FUN_004b2f90((double)counter_value);
    g_bomb_5fb0b0 = (int)counter_value;
  }
  /* Active flag of trigger 0 (its word [2]): counter > 0. */
  g_course_64d8e4 = (uint)(0 < (int)g_bomb_5fb0b0);
  race_phase = race_get_phase();
  /* Detonation: in the detonation phase with the timer at or below 0, blow up the carrier car
     (damage, rwp mode, impulse, fx), then car_kill(0) (inferred: ends the sequence). */
  if ((race_phase == RACE_PHASE_DETONATION) && (g_course_7ef7cc <= k_f_0)) {
    part_position = (float *)(part_matrix + 0x30);
    frame.pos_x = *part_position;
    frame.pos_y = *(undefined4 *)(part_matrix + 0x34);
    frame.pos_z = *(float *)(part_matrix + 0x38);
    frame.sphere.center.x = 50.0;
    frame.sphere.center.y = 1000.0;
    frame.sphere.center.z = 50.0;
    car_set_damage(g_bomb_7ef7c8,BOMB_DAMAGE_BITS, 0x0);
    car_set_damage(g_bomb_7ef7c8,BOMB_DAMAGE_BITS,1);
    fx_emitter_spawn_a_default(&frame.pos_x,&g_car_62ac74,BOMB_BLAST_FX_SCALE_BITS);
    car_enter_rwp_mode(g_bomb_7ef7c8);
    frame.pos_z = frame.pos_z + k_f_0_03;
    frame.pos_x = frame.pos_x + k_f_0_03;
    car_apply_rwp_impulse(g_bomb_7ef7c8,&frame.sphere,&frame.pos_x);
    fx_ring_add_point(part_position,FX_RING_LARGE_BITS);
    fx_ring_add_point(part_position,FX_RING_SMALL_BITS);
    car_kill(0x0);
    g_car_7ef7b0 = 0;
  }
  /* Trigger table g_bomb_64d8dc: 6 records of 0x14 bytes. Word offsets from the threshold
     (trigger[0]): [-1] float time bonus, [0] int rank-key threshold, [1] hit flag,
     [2] active flag, [3] float set to TRIGGER_HIT_VALUE_BITS on hit. A record fires when the
     player's rank key is in [threshold, threshold + 1.0) and it is active and not yet hit;
     the bonus is then added to the timer. */
  part_matrix = 0;
  trigger = g_bomb_64d8dc;
  do {
    if ((trigger[2] != 0) && (trigger[1] == 0)) {
      counter_value = (float10)player_rank_key(0x0);
      if (((float10)*trigger <= counter_value) && (counter_value < (float10)float_1_0 + (float10)*trigger)) {
        g_course_7ef7cc = g_course_7ef7cc + *(float *)&trigger[-1];
        trigger[1] = 1;
        trigger[3] = TRIGGER_HIT_VALUE_BITS;
        sound_bomb_countdown_voices(1, 0x0);
      }
    }
    /* Inner loop: trail slots 1 then 0 of this trigger. A slot with TRAIL_FLAG_CULL_TEST whose
       sphere (radius 0.2 at its trail point) is outside the camera frustum gets that result
       stored as its user data and sets any_outside_frustum. */
    race_phase = 2;
    do {
      race_phase = race_phase + -1;
      trail_object = trail_get_value(part_matrix, race_phase);
      user_data = rwp_object_get_user_data(trail_object);
      if ((user_data & TRAIL_FLAG_CULL_TEST) != 0) {
        trail_get_triple(part_matrix,race_phase,(undefined4)&frame.sphere);
        sphere = &frame.sphere;
        frame.sphere.radius = 0.2;
        camera = (RwCamera *)get_viewport_camera(0);
        frustum_result = RwCameraFrustumTestSphere(camera,sphere);
        if (frustum_result == rwSPHEREOUTSIDE) {
          any_outside_frustum = true;
          rwp_object_set_user_data(trail_object, frustum_result);
        }
      }
    } while (race_phase != 0);
    trigger = trigger + 5;
    part_matrix = part_matrix + 1;
  } while ((int)trigger < (int)g_bomb_64d8dc + BOMB_TRIGGER_TABLE_BYTES);
  /* Some trail point was outside the frustum: refresh trails (inferred: trail_update_00414f00). */
  if (!any_outside_frustum) {
    return;
  }
  trail_update_00414f00();
  return;
}


