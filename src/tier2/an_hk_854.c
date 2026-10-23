#include "mfl_tier2.h"
/*00408ea0*/ extern int player_record_get_8d8ca0();
/*0040fbf0*/ extern int race_get_focus_player();
/*0047a5b0*/ extern int car_add_velocity();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00481fd0*/ extern double random_range();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd33c*/ extern float k_f_4;
/*005dd374*/ extern float k_f_0_2;
/*005dd7f8*/ extern float k_f_110;
/*005ddc28*/ extern float k_f_0;
/*005de294*/ extern float k_f_n0_125;
/*005df7e8*/ extern float k_f_n0_02;
/*006a9728*/ extern undefined4 g_weapon_6a9728;
/*006a972c*/ extern undefined4 g_weapon_flamethrower_6a972c;
/*006a9730*/ extern undefined4 g_weapon_flamethrower_6a9730;
/*006a9734*/ extern undefined4 g_weapon_flamethrower_6a9734;
/*006a9738*/ extern undefined4 g_weapon_flamethrower_6a9738;
/*006a973c*/ extern undefined4 g_weapon_flamethrower_6a973c;
/*006a9750*/ extern undefined4 g_weapon_flamethrower_6a9750;
/*006a9758*/ extern undefined4 g_weapon_flamethrower_6a9758;

/* dwords per flame particle record in the g_weapon_flamethrower_* arrays (0xd * 4 = 0x34 bytes) */
#define FLAME_SLOT_DWORDS (0xd)
/* flame object offsets (inferred from use) */
#define FLAME_ACTIVE_OFFSET (0x1c)
#define FLAME_TIMER_OFFSET (0x8)
#define FLAME_CYCLE_OFFSET (0x14)
#define FLAME_SUBSTEP_OFFSET (0x18)
#define FLAME_ORIGIN_HOLDER_OFFSET (0x24)
#define FLAME_DIR_HOLDER_OFFSET (0x20)
/* float bits: 0.02f */
#define FLAME_TIMER_RESET_BITS (0x3ca3d70a)
/* car object offsets (inferred from use) */
#define CAR_PLAYER_ID_OFFSET (0xb0)
#define CAR_VEC_X_OFFSET (0x20)
#define CAR_VEC_Y_OFFSET (0x24)
#define CAR_VEC_Z_OFFSET (0x28)
/* float bits: random_range bounds, -pi and pi */
#define RANDOM_ANGLE_MIN_BITS (0xc0490fdb)
#define RANDOM_ANGLE_MAX_BITS (0x40490fdb)
/* float bits: random_range bounds, -0.002f and 0.002f */
#define RANDOM_JITTER_MIN_BITS (0xbb03126f)
#define RANDOM_JITTER_MAX_BITS (0x3b03126f)
/* weapon_flame_push_cars */

/*
 * Per-tick flamethrower update (inferred). While the flame is active and its timer is set,
 * it writes one particle record into the g_weapon_flamethrower_* ring: position from the
 * origin and direction frames, random angle, jittered velocity. Then it pushes the car at
 * flame+4 with car_add_velocity, skipped when that car belongs to the focus player with the
 * higher player record (inferred).
 * flame_arg: the flame weapon object. unused_2..unused_4 are not read.
 * (was: "flame boost: adds velocity to the player car and cars inside the flame (car_add_velocity)")
 */

void __cdecl weapon_flame_push_cars(int flame_arg,undefined4 unused_2,undefined4 unused_3,undefined4 unused_4)

{
  int flame;
  RwMatrix *frame_ltm;
  int slot;
  int record_b;
  int tmp;
  double rand_a;
  double rand_b;
  struct {
    int focus_player_b;
    float pos_x;
    float pos_y;
    float pos_z;
    float dir_x;
    float dir_y;
    float dir_z;
    undefined1 car_vel_out [12];
  } vec;

  flame = flame_arg;
  if ((*(int *)(flame_arg + FLAME_ACTIVE_OFFSET) != 0) &&
     (*(float *)(flame_arg + FLAME_TIMER_OFFSET) < k_f_0 != (*(float *)(flame_arg + FLAME_TIMER_OFFSET) == k_f_0))) {
    /* particle origin: position of the origin frame */
    frame_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(flame_arg + FLAME_ORIGIN_HOLDER_OFFSET) + 4));
    vec.pos_x = (frame_ltm->pos).x;
    vec.pos_y = (frame_ltm->pos).y;
    vec.pos_z = (frame_ltm->pos).z;
    /* direction frame: at vector, scaled into the position offset (dir_* is reused as push vector below) */
    frame_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(flame + FLAME_DIR_HOLDER_OFFSET) + 4));
    vec.dir_x = (frame_ltm->at).x;
    vec.dir_y = (frame_ltm->at).y;
    tmp = *(int *)(*(char **)(flame + 4) + CAR_PLAYER_ID_OFFSET);
    vec.dir_z = (frame_ltm->at).z * k_f_n0_125;
    /* ring slot: derived from owner player id, cycle and substep counters */
    slot = *(int *)(flame + FLAME_SUBSTEP_OFFSET) + (tmp + *(int *)(flame + FLAME_CYCLE_OFFSET) + tmp * 4) * 5;
    vec.pos_x = vec.pos_x + vec.dir_x * k_f_n0_125;
    tmp = *(int *)(flame + FLAME_SUBSTEP_OFFSET) + 1;
    vec.pos_y = vec.pos_y + vec.dir_y * k_f_n0_125;
    *(int *)(flame + FLAME_SUBSTEP_OFFSET) = tmp;
    vec.pos_z = vec.pos_z + vec.dir_z;
    /* after 5 substeps (0..4): advance the cycle, clear the active flag and substep (inferred) */
    if (4 < tmp) {
      *(int *)(flame + FLAME_CYCLE_OFFSET) = *(int *)(flame + FLAME_CYCLE_OFFSET) + 1;
      *(undefined4 *)(flame + FLAME_ACTIVE_OFFSET) = 0;
      *(undefined4 *)(flame + FLAME_SUBSTEP_OFFSET) = 0;
    }
    /* store the particle record at slot: flag, angle, position (rand_a/rand_b are reused as samples and scales) */
    (&g_weapon_flamethrower_6a9758)[slot * FLAME_SLOT_DWORDS] = 1;
    *(undefined4 *)(flame + FLAME_TIMER_OFFSET) = FLAME_TIMER_RESET_BITS;
    rand_a = (double)random_range(RANDOM_ANGLE_MIN_BITS,RANDOM_ANGLE_MAX_BITS);
    *(float *)&(&g_weapon_flamethrower_6a9750)[slot * FLAME_SLOT_DWORDS] = (float)rand_a;
    *(float *)&(&g_weapon_6a9728)[slot * FLAME_SLOT_DWORDS] = vec.pos_x;
    *(float *)&(&g_weapon_flamethrower_6a972c)[slot * FLAME_SLOT_DWORDS] = vec.pos_y;
    *(float *)&(&g_weapon_flamethrower_6a9730)[slot * FLAME_SLOT_DWORDS] = vec.pos_z;
    /* particle velocity: owner car's vector times k_f_n0_02, plus jitter (pos_* hold velocity from here) */
    car_get_velocity_any_mode(vec.car_vel_out,*(undefined4 *)(*(char **)(flame + 4) + CAR_PLAYER_ID_OFFSET));
    tmp = *(int *)(flame + 4);
    vec.pos_x = *(float *)(tmp + CAR_VEC_X_OFFSET) * k_f_n0_02;
    vec.pos_y = *(float *)(tmp + CAR_VEC_Y_OFFSET) * k_f_n0_02;
    vec.pos_z = *(float *)(tmp + CAR_VEC_Z_OFFSET) * k_f_n0_02;
    rand_a = (double)random_range(RANDOM_JITTER_MIN_BITS,RANDOM_JITTER_MAX_BITS);
    vec.pos_x = (float)(rand_a * (double)k_f_4 + (double)vec.pos_x);
    rand_b = (double)random_range(RANDOM_JITTER_MIN_BITS,RANDOM_JITTER_MAX_BITS);
    rand_a = (double)k_f_4;
    *(float *)&(&g_weapon_flamethrower_6a9734)[slot * FLAME_SLOT_DWORDS] = vec.pos_x;
    *(float *)&(&g_weapon_flamethrower_6a9738)[slot * FLAME_SLOT_DWORDS] = vec.pos_y;
    vec.pos_z = (float)(rand_b * rand_a + (double)vec.pos_z);
    *(float *)&(&g_weapon_flamethrower_6a973c)[slot * FLAME_SLOT_DWORDS] = vec.pos_z;
    /* flame_arg slot reused as out-param: focus player A; vec.focus_player_b is focus player B */
    flame_arg = 0;
    vec.focus_player_b = 0;
    race_get_focus_player(&flame_arg,&vec.focus_player_b);
    /* slot reused: player record of focus player A */
    slot = player_record_get_8d8ca0(flame_arg);
    record_b = player_record_get_8d8ca0(vec.focus_player_b);
    /* chosen player: the focus player with the higher record (inferred) */
    tmp = flame_arg;
    if (slot <= record_b) {
      tmp = vec.focus_player_b;
    }
    /* push the owner car unless it belongs to the chosen player; push = owner vector * substep * k_f_0_2 * k_f_110 */
    slot = *(int *)(flame + 4);
    if (*(int *)(slot + CAR_PLAYER_ID_OFFSET) != tmp) {
      rand_b = (double)*(int *)(flame + FLAME_SUBSTEP_OFFSET) * (double)k_f_0_2 * (double)k_f_110;
      vec.dir_x = (float)(rand_b * (double)*(float *)(slot + CAR_VEC_X_OFFSET));
      vec.dir_y = (float)((double)*(float *)(slot + CAR_VEC_Y_OFFSET) * rand_b);
      vec.dir_z = (float)((double)*(float *)(slot + CAR_VEC_Z_OFFSET) * rand_b);
      car_add_velocity(&vec.dir_x,*(int *)(slot + CAR_PLAYER_ID_OFFSET));
    }
  }
  return;
}


