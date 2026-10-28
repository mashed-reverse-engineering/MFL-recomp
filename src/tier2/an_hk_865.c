#include "mfl_tier2.h"
/*00409f50*/ extern char *player_route_get_record_ptr();
/*00417ce0*/ extern int __cdecl ai_find_leader_b(undefined4 p0, undefined4 p1, undefined4 p2);
/*00417d50*/ extern int ai_player0_distance_is_zero();
/*0044c180*/ extern double player_distance_get();
/*00461620*/ extern int __cdecl rocket_slot_6a6000_get(undefined4 p0);
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*0047b520*/ extern int car_current_matrix();
/*0047b750*/ extern int car_get_speed();
/*00481fd0*/ extern double random_range();
/*004b2b48*/ extern int _ftol();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd620*/ extern float float_10_0;
/*005dd7f4*/ extern float k_f_80;
/*005dda24*/ extern float k_f_0_75;
/*005ddb0c*/ extern float k_f_15;
/*005ddb20*/ extern float k_f_0_9;
/*005ddb38*/ extern float k_f_25;
/*005de230*/ extern float k_f_n0_75;
/*005de238*/ extern float k_f_2000;
/*005de23c*/ extern float k_f_0_06666667;
/*005de240*/ extern float k_f_2_75;
/*007ef7f0*/ extern undefined4 g_tick_count;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9980*/ extern float g_ai_skill;

#define MUST_FIRE_HELD_MS (60000)
#define WEAPON_HELD_RESET_MS (3000) /* (inferred) held-time threshold, same 3 s as the delay below */
#define FIRE_DELAY_DEFAULT_MS (3000)
#define FIRE_DELAY_MEDIUM_MS (6000)
#define FIRE_DELAY_LONG_MS (9000)
#define FLOAT_ONE_BITS (0x3f800000) /* 1.0f as bits, passed to random_range as the upper bound */


/* ai_bot_stuck_check */

/* "per bot timers (0x8C9B40 stride 0x78) with g_tick_count and the car speed (inferred)" */

/*
 * Per-frame fire decision for one AI bot (inferred).
 * Advances the bot's held-weapon timer and fire cooldown, then picks a rule from the
 * weapon id in the bot's weapon descriptor. A rule either presses fire
 * (player_input->fire = 1) or sets the next cooldown (fire_delay) and returns.
 * bot_index_then_speed: bot index (its bit pattern is read as an int), later compared as a float
 *          against distances (inferred).
 * range_value: int compared as a float against 15 / 25 / 80 (inferred, meaning unknown).
 * unused_1..3 are unused. Always returns 0.
 */
undefined4 __cdecl ai_bot_stuck_check(undefined4 unused_1,undefined4 unused_2,undefined4 unused_3,float bot_index_then_speed,
                  int range_value)

{
  float random_roll;
  struct PlayerInput *player_input;
  float route_offset;
  int iBot;
  int held_ms_total;
  int tmp_a;
  float *route_record;
  int tmp_b;
  double roll_or_dist;
  struct {
    char *player0_matrix;
    char *bot_matrix;
    RwReal to_player0_length;
    RwV3d to_player0;
    RwV3d to_player0_dir;
  } frame;
  
  iBot = *(int *)&bot_index_then_speed;
  tmp_b = g_player_slots[iBot].controller;
  player_input = &g_player_inputs[tmp_b];
  car_get_speed(&bot_index_then_speed,iBot);
  tmp_a = g_tick_count;
  held_ms_total = g_ai_bots[iBot].weapon_held_ms + g_tick_count;
  g_ai_bots[iBot].weapon_held_ms = held_ms_total;
  if (MUST_FIRE_HELD_MS < held_ms_total) {
    g_ai_bots[iBot].must_fire = 1;
  }
  if (held_ms_total < WEAPON_HELD_RESET_MS) {
    g_ai_bots[iBot].fire_delay = FIRE_DELAY_DEFAULT_MS;
  }
  if ((g_ai_bots[iBot].fire_delay != 0) &&
     (tmp_a = g_ai_bots[iBot].fire_delay - tmp_a, g_ai_bots[iBot].fire_delay = tmp_a,
     tmp_a < 0)) {
    g_ai_bots[iBot].fire_delay = 0;
  }
  if ((undefined4 *)player_weapon_slots[iBot].descriptor == (undefined4 *)0x0) {
    return 0;
  }
  /* weapon id from the descriptor selects the fire rule; ids without a rule fall through to return 0 */
  switch(*(undefined4 *)player_weapon_slots[iBot].descriptor) {
  case WEAPON_MORTAR:
    if (g_ai_bots[iBot].must_fire == 0) {
      if (g_ai_bots[iBot].fire_delay != 0) {
        return 0;
      }
      tmp_a = ai_player0_distance_is_zero();
      if (tmp_a == 0) {
        return 0;
      }
      roll_or_dist = (double)player_distance_get(iBot);
      if (!(roll_or_dist > (double)k_f_2_75)) {
        return 0;
      }
      if (!(k_f_25 > (float)range_value)) {
        return 0;
      }
      random_roll = random_range(0,FLOAT_ONE_BITS);
      tmp_a = ((int)(g_ai_skill));
      if (!((random_roll) < (double)float_1_0 - ((double)float_10_0 - (double)tmp_a) * (double)k_f_0_06666667)) {
LAB_00418283:
        g_ai_bots[iBot].fire_delay = FIRE_DELAY_DEFAULT_MS;
        return 0;
      }
    }
    player_input->fire = 1;
    g_ai_bots[iBot].fire_delay = 0x2ee;
    return 0;
  case WEAPON_MACHINEGUN:
    /* burst: start a burst at random or when must_fire is set, hold fire until the weapon effect ends */
    if (g_ai_bots[iBot].burst_active == 0) {
      if (g_ai_bots[iBot].must_fire != 0) {
LAB_00418304:
        g_ai_bots[iBot].burst_active = 1;
        return 0;
      }
      if (((g_ai_bots[iBot].fire_delay == 0) &&
          (tmp_b = ai_player0_distance_is_zero(), tmp_b != 0)) &&
         (tmp_b = weapon_effect_get_slot_value_b(iBot), tmp_b == 0)) {
        roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
        if (roll_or_dist < (double)k_f_0_75) {
          g_ai_bots[iBot].burst_active = 1;
          return 0;
        }
        goto LAB_00418283;
      }
    }
    else {
      player_input->fire = 1;
      tmp_b = weapon_effect_get_slot_value_b(iBot);
      if (tmp_b != 0) {
        g_ai_bots[iBot].burst_active = 0;
        return 0;
      }
    }
    break;
  case WEAPON_DEPTHCHARGE:
    if (g_ai_bots[iBot].must_fire == 0) {
      if (g_ai_bots[iBot].fire_delay != 0) {
        return 0;
      }
      if (!(bot_index_then_speed > k_f_2000)) {
        return 0;
      }
      tmp_a = ai_find_leader_b(iBot,0x40800000,0x41200000);
      if (tmp_a == 0) {
        return 0;
      }
      if (!(k_f_15 > (float)range_value)) {
        return 0;
      }
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (!(roll_or_dist < (double)k_f_0_9)) goto LAB_00418177;
    }
    goto LAB_004181f2;
  case WEAPON_MISSILE:
    if (g_ai_bots[iBot].must_fire != 0) {
      player_input->fire = 1;
    }
    if (((g_ai_bots[iBot].fire_delay == 0) &&
        (tmp_a = ai_player0_distance_is_zero(), tmp_a != 0)) &&
       ((tmp_a = weapon_effect_get_slot_value_b(iBot), tmp_a == 0 &&
        (tmp_a = rocket_slot_6a6000_get(iBot), tmp_a != 0)))) {
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (roll_or_dist < (double)k_f_0_75) {
        player_input->fire = 1;
        return 0;
      }
LAB_00418177:
      g_ai_bots[iBot].fire_delay = FIRE_DELAY_MEDIUM_MS;
      return 0;
    }
    break;
  case WEAPON_MINE:
    if (g_ai_bots[iBot].must_fire == 0) {
      if (g_ai_bots[iBot].fire_delay != 0) {
        return 0;
      }
      if (!(bot_index_then_speed > k_f_2000)) {
        return 0;
      }
      tmp_a = ai_find_leader_b(iBot,0x40800000,0x41200000);
      if (tmp_a == 0) {
        return 0;
      }
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (!(roll_or_dist < (double)k_f_0_75)) {
LAB_0041835d:
        g_ai_bots[iBot].fire_delay = FIRE_DELAY_LONG_MS;
        return 0;
      }
    }
LAB_004181f2:
    player_input->fire = 1;
    g_ai_bots[iBot].fire_delay = 0x5dc;
    return 0;
  case WEAPON_FLAMETHROWER:
    if (g_ai_bots[iBot].burst_active != 0) {
LAB_004182e3:
      player_input->fire = 1;
      return 0;
    }
    if (g_ai_bots[iBot].must_fire != 0) goto LAB_00418304;
    if (((g_ai_bots[iBot].fire_delay == 0) && (k_f_2000 < bot_index_then_speed)) &&
       (tmp_b = ai_find_leader_b(iBot,0x40400000,0x41200000), tmp_b != 0)) {
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (roll_or_dist < (double)k_f_0_5) {
LAB_0041834a:
        g_ai_bots[iBot].burst_active = 1;
        return 0;
      }
      goto LAB_00418283;
    }
    break;
  case WEAPON_SHOTGUN:
    /* shotgun: fire when the bot is roughly level with the route direction (route_offset in (-0.75, 0.75)) */
    if (g_ai_bots[iBot].must_fire != 0) {
LAB_004180e3:
      player_input->fire = 1;
      g_ai_bots[iBot].fire_delay = 500;
      return 0;
    }
    if ((g_ai_bots[iBot].fire_delay == 0) &&
       (tmp_a = ai_find_leader_b(iBot,0x3f000000,0x40400000), tmp_a != 0)) {
      car_current_matrix(&frame.player0_matrix,0);
      car_current_matrix(&frame.bot_matrix,iBot);
      frame.to_player0.x = *(float *)(frame.player0_matrix + 0x30) - *(float *)(frame.bot_matrix + 0x30);
      frame.to_player0.y = 0.0;
      frame.to_player0.z = *(float *)(frame.player0_matrix + 0x38) - *(float *)(frame.bot_matrix + 0x38);
      frame.to_player0_length = RwV3dLength(&frame.to_player0);
      RwV3dNormalize(&frame.to_player0_dir,&frame.to_player0);
      route_record = (float *)player_route_get_record_ptr(iBot);
      route_offset = -((route_record[2] * frame.to_player0_dir.z + frame.to_player0_dir.x * *route_record + route_record[1] * frame.to_player0_dir.y) * frame.to_player0_length);
      if ((k_f_n0_75 < route_offset) && (route_offset < k_f_0_75)) {
        roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
        if (!(roll_or_dist < (double)k_f_0_9)) goto LAB_00418283;
        goto LAB_004180e3;
      }
    }
    break;
  case WEAPON_FLARE:
    if (g_ai_bots[iBot].must_fire != 0) {
      player_input->fire = 1;
    }
    if ((g_ai_bots[iBot].fire_delay == 0) && (k_f_80 < (float)range_value)) {
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (!(roll_or_dist < (double)k_f_0_5)) goto LAB_0041835d;
      goto LAB_004182e3;
    }
    break;
  case WEAPON_OIL:
    if (g_ai_bots[iBot].burst_active != 0) goto LAB_004182e3;
    if (g_ai_bots[iBot].must_fire != 0) goto LAB_00418304;
    if ((g_ai_bots[iBot].fire_delay == 0) && (k_f_80 < (float)range_value)) {
      roll_or_dist = (double)random_range(0,FLOAT_ONE_BITS);
      if (roll_or_dist < (double)k_f_0_5) goto LAB_0041834a;
      goto LAB_0041835d;
    }
  }
  return 0;
}


