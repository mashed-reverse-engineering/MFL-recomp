#include "mfl_tier2.h"
typedef int WeaponId;
/*00409eb0*/ extern double player_rank_key();
/*0040fdc0*/ extern int race_get_phase();
/*00419ff0*/ extern int __cdecl ai_bot_get_flag_20(undefined4 p0);
/*0041a000*/ extern int ai_bot_clear_flag_20();
/*00431730*/ extern int menu_event_0x11_reset_page();
/*00431790*/ extern undefined __cdecl training_powerups_setup(undefined4 p0);
/*00437f30*/ extern int ui_get_car_69d2d8();
/*0044c180*/ extern double player_distance_get();
/*0044c550*/ extern void * camera_get_8c7470();
/*004678b0*/ extern int player_get_weapon_id();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*0047b750*/ extern int car_get_speed();
/*00480060*/ extern int hud_label_add_world_text();
/*005dd814*/ extern float k_f_6;
/*005de20c*/ extern float k_f_2_5;
/*005de220*/ extern float k_f_2500;
/*005de8fc*/ extern float k_f_68;
/*005deb50*/ extern float k_f_69;
/*005deb54*/ extern float k_f_77;
/*005deb58*/ extern float k_f_74;
/*005deb5c*/ extern float k_f_83;
/*005deb60*/ extern float k_f_78;
/*005deb64*/ extern float k_f_73;
/*005deb68*/ extern float k_f_83_9;
/*005deb6c*/ extern float k_f_83_8;
/*005deb70*/ extern float k_f_n6;
/*005deb74*/ extern float k_f_73_4;
/*005deb78*/ extern float k_f_73_3;
/*0069cc34*/ extern undefined4 g_app_state_69cc34;
/*0069d0f8*/ extern undefined4 g_app_state_69d0f8;
/*0069d1d8*/ extern undefined4 g_app_state_69d1d8;
/*0069d218*/ extern undefined4 g_app_state_69d218;
/*0069d284*/ extern undefined4 g_app_state_69d284;
/*0069d2c4*/ extern undefined4 g_app_state_69d2c4;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8070*/ extern undefined4 g_car_8c8070;
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c807c*/ extern undefined4 g_car_8c807c;
/*008c86c4*/ extern undefined4 g_car_8c86c4;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/*008c8720*/ extern undefined4 g_hit_shake;
/*008c8788*/ extern undefined4 g_car_8c8788;
/* training_hint_labels_update */
#define WEAPON_NONE (-1)
#define WEAPON_MORTAR (7)
#define WEAPON_MACHINEGUN (9)
#define WEAPON_DEPTHCHARGE (10)
#define WEAPON_MISSILE (11)
#define WEAPON_MINE (12)
#define WEAPON_FLAMETHROWER (16)
#define WEAPON_SHOTGUN (17)
#define WEAPON_FLARE (18)
#define WEAPON_OIL (19)
#define HINT_TEXT_KEEP_GOING (0x318)
#define TRAINING_POWERUP_COUNT (9)

/* "shows world text hints such as 'KEEP GOING!' (text 0x318) next to the player with
   hud_label_add_world_text, depending on the distance to player 1 and the car modes" */

/*
 * Called once per frame during training. Picks the training lesson (g_training_lesson)
 * and shows the matching world-text hint (hud_label_add_world_text) above the camera
 * anchor point. Takes no parameters and returns nothing; hint state lives in g_app_state_* /
 * g_car_* globals.
 */
void training_hint_labels_update(void)

{
  float rank_key;
  bool in_hint_band;
  undefined4 *camera_vec;
  WeaponId held_weapon;
  int status;
  float10 scratch;
  undefined4 label_id;
  undefined1 *label_colour;
  undefined1 *label_colour_alt;
  struct {
    undefined1 colour_red_r;
    undefined1 colour_red_g;
    undefined1 colour_red_b;
    undefined1 colour_red_a;
    undefined1 colour_yellow_r;
    undefined1 colour_yellow_g;
    undefined1 colour_yellow_b;
    undefined1 colour_yellow_a;
    undefined1 colour_green_r;
    undefined1 colour_green_g;
    undefined1 colour_green_b;
    undefined1 colour_green_a;
    int car_mode;
    int car_mode_flag;
    char *car_matrix;
    float car_speed;
    undefined4 label_pos_x;
    undefined4 label_pos_y;
    undefined4 label_pos_z;
    undefined4 powerup_weapons [TRAINING_POWERUP_COUNT];
  } frame;

  frame.colour_red_r = 0xff;
  frame.colour_red_g = 0;
  frame.colour_red_b = 0;
  frame.colour_red_a = 0xff;
  frame.colour_yellow_r = 0xff;
  frame.colour_yellow_g = 0xff;
  frame.colour_yellow_b = 0;
  frame.colour_yellow_a = 0xff;
  frame.colour_green_r = 0;
  frame.colour_green_g = 0xff;
  frame.colour_green_b = 0;
  frame.colour_green_a = 0xff;
  frame.powerup_weapons[0] = WEAPON_MACHINEGUN;
  frame.powerup_weapons[1] = WEAPON_MISSILE;
  frame.powerup_weapons[2] = WEAPON_MORTAR;
  frame.powerup_weapons[3] = WEAPON_FLAMETHROWER;
  frame.powerup_weapons[4] = WEAPON_OIL;
  frame.powerup_weapons[5] = WEAPON_MINE;
  frame.powerup_weapons[6] = WEAPON_DEPTHCHARGE;
  frame.powerup_weapons[7] = WEAPON_SHOTGUN;
  frame.powerup_weapons[8] = WEAPON_FLARE;
  g_car_8c8788 = 0;
  camera_vec = (undefined4 *)camera_get_8c7470();
  frame.label_pos_x = *camera_vec;
  frame.label_pos_y = camera_vec[1];
  frame.label_pos_z = camera_vec[2];
  /* lesson 0 (inferred): show KEEP GOING once player 1 has moved away from the start;
     g_app_state_69d0f8 is the latch, re-armed when player 1 is close again */
  if (g_training_lesson == 0) {
    scratch = (float10)player_distance_get(1);
    if (scratch < (float10)k_f_2_5) {
      g_app_state_69d0f8 = 0;
    }
    scratch = (float10)player_distance_get(1);
    if (((float10)k_f_6 < scratch) && (g_app_state_69d0f8 == 0)) {
      hud_label_add_world_text(HINT_TEXT_KEEP_GOING,&frame.label_pos_x,&frame.colour_red_r,&frame.colour_yellow_r,0x42000000,0x40000000);
      g_app_state_69d0f8 = 1;
    }
  }
  /* lesson 2 (inferred): car 1 in a mode 2..100 with its flag set, checked on a cooldown
     (g_app_state_69d1d8); shows 0x31a / 0x31b alternating, toggled by g_app_state_69d284 */
  if (((g_training_lesson == 2) &&
      ((((car_get_mode(1,&frame.car_mode_flag,&frame.car_mode), g_app_state_69d1d8 == 0 ||
         (g_app_state_69d1d8 = g_app_state_69d1d8 + -1, g_app_state_69d1d8 == 0)) && (frame.car_mode_flag != 0)
        ) && ((1 < frame.car_mode && (frame.car_mode < 0x65)))))) &&
     (car_get_mode(0,&frame.car_mode_flag,&frame.car_mode), frame.car_mode == 0)) {
    g_app_state_69d284 = g_app_state_69d284 + 1;
    if ((g_app_state_69d284 & 1) == 0) {
      label_id = 0x31b;
    }
    else {
      label_id = 0x31a;
    }
    hud_label_add_world_text(label_id,&frame.label_pos_x,&frame.colour_red_r,&frame.colour_yellow_r,0x42000000,0x40000000);
    g_app_state_69d1d8 = 0x32;
  }
  /* lesson 3 (inferred): weapon training. Once per run, hand out the powerup for the
     current slot; then hint on the weapon player 1 holds and set g_car_8c8788 to a
     weapon category (1 fire-type, 2 machinegun/missile/mortar, 3 mine/depthcharge, 4 shotgun) */
  if (g_training_lesson != 3) goto LAB_0043a5ab;
  if (g_car_8c86c4 == 0) {
    if ((int)g_car_8c807c < TRAINING_POWERUP_COUNT) {
      training_powerups_setup(frame.powerup_weapons[g_car_8c807c]);
    }
    g_car_8c86c4 = 1;
    g_app_state_69d218 = 0;
    g_hit_shake = 0;
  }
  if (g_car_8c86c8 == 100) {
    g_app_state_69d218 = 0;
  }
  held_weapon = player_get_weapon_id(0x0);
  if (((held_weapon != WEAPON_NONE) && (g_app_state_69d218 == 0)) && (g_car_69d2d8 == 0)) {
    menu_event_0x11_reset_page(g_car_8c807c * 2 + 1);
    g_app_state_69d218 = 1;
  }
  held_weapon = player_get_weapon_id(0x0);
  if (held_weapon == WEAPON_MACHINEGUN) {
    status = ai_bot_get_flag_20(1);
    if (status != 0) {
      ai_bot_clear_flag_20(1);
      g_hit_shake = g_hit_shake + 1;
      if ((g_hit_shake == 1) || (g_hit_shake == 10)) {
        hud_label_add_world_text(0x317,&frame.label_pos_x,&frame.colour_red_r,&frame.colour_yellow_r,0x42000000,0x3f000000);
      }
    }
LAB_0043a57e:
    g_car_8c8788 = 2;
  }
  else {
    if (((held_weapon == WEAPON_FLAMETHROWER) || (held_weapon == WEAPON_OIL)) ||
       (held_weapon == WEAPON_FLARE)) {
      g_car_8c8788 = 1;
    }
    if ((held_weapon == WEAPON_MISSILE) || (held_weapon == WEAPON_MORTAR)) goto LAB_0043a57e;
  }
  if ((held_weapon == WEAPON_MINE) || (held_weapon == WEAPON_DEPTHCHARGE)) {
    g_car_8c8788 = 3;
  }
  if (held_weapon == WEAPON_SHOTGUN) {
    g_car_8c8788 = 4;
  }
LAB_0043a5ab:
  /* lesson 4 (inferred): speed and rank-band hints for player 1. Returns early unless
     g_car_8c86c8 is 1 or 4 (inferred: active stage) */
  if (g_training_lesson != 4) {
    return;
  }
  if ((g_car_8c86c8 != 1) && (g_car_8c86c8 != 4)) {
    return;
  }
  status = race_get_phase();
  if (status == 5) {
    g_car_8c8070 = 0;
    g_app_state_69cc34 = 0;
    g_app_state_69d2c4 = 0;
  }
  car_current_matrix(&frame.car_matrix,0);
  scratch = (float10)player_rank_key(0);
  rank_key = (float)scratch;
  car_get_speed(&frame.car_speed,0);
  if (g_app_state_69d1d8 != 0) {
    g_app_state_69d1d8 = g_app_state_69d1d8 + -1;
  }
  in_hint_band = false;
  if (((k_f_73_3 < rank_key) && (rank_key < k_f_73_4)) && (k_f_n6 < *(float *)(frame.car_matrix + 0x30))) {
    in_hint_band = true;
  }
  if (((k_f_83_8 < rank_key) && (rank_key < k_f_83_9)) && (*(float *)(frame.car_matrix + 0x30) < k_f_n6)) {
    in_hint_band = true;
  }
  if (((k_f_2500 < frame.car_speed) && (in_hint_band)) && (g_app_state_69d1d8 == 0)) {
    hud_label_add_world_text(0x319,&frame.label_pos_x,&frame.colour_red_r,&frame.colour_yellow_r,0x42000000,0x3f000000);
    g_app_state_69d1d8 = 100;
  }
  if (((((k_f_73 < rank_key) && (rank_key < k_f_78)) || (k_f_83 < rank_key)) &&
      ((g_player_inputs[g_player_slots[0].controller].brake != 0 &&
       (g_player_inputs[g_player_slots[0].controller].accelerate != 0)))) &&
     (g_player_inputs[g_player_slots[0].controller].left != 0)) {
    g_app_state_69d2c4 = g_app_state_69d2c4 + 1;
  }
  if (((k_f_74 < rank_key) && (rank_key < k_f_77)) && (g_app_state_69cc34 == 0)) {
    g_app_state_69cc34 = 1;
  }
  if (!(rank_key > k_f_68)) {
    return;
  }
  if (!(rank_key < k_f_69)) {
    return;
  }
  if (g_app_state_69cc34 == 1) {
    g_app_state_69cc34 = 0;
    if ((int)g_app_state_69d2c4 < 0xb) {
      status = ui_get_car_69d2d8();
      if (status != 0) {
        g_app_state_69d2c4 = 0;
        return;
      }
      label_colour_alt = &frame.colour_red_r;
      label_colour = &frame.colour_red_r;
      label_id = 0x31d;
    }
    else {
      g_car_8c8070 = g_car_8c8070 + 1;
      label_colour_alt = &frame.colour_green_r;
      label_colour = &frame.colour_green_r;
      label_id = 0x31c;
    }
    hud_label_add_world_text(label_id,&frame.label_pos_x,label_colour,label_colour_alt,0x42000000,0x3f000000);
    g_app_state_69d2c4 = 0;
    return;
  }
  return;
}


