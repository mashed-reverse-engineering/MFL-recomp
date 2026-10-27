// cluster-entries: training_mode_update
/* Installed cluster training_mode_update (tools/cluster_install.py from output/cluster_pilot_j1/training_mode_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040af20*/ extern int autosave_request();
/*00410020*/ extern int race_begin_countdown();
/*00412d60*/ extern int race_restart();
/*0042e9e0*/ extern int time_clear_current();
/*00430010*/ extern int __cdecl menu_select_pressed_any(undefined4 p0);
/*004300b0*/ extern int menu_back_pressed_any();
/*00431710*/ extern int ui_menu_reset();
/*0043a2c0*/ extern int training_hint_labels_update();
/*00444c10*/ extern int menu_page_enter();
/*0047a5b0*/ extern int car_add_velocity();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00491d70*/ extern int car_reset_physics();
/*004a2970*/ extern int app_flag_76e7f8_set();
/*004a29a0*/ extern int app_enter_frontend();
/*004b2b48*/ extern int _ftol();
/*005de270*/ extern float k_f_n0_1;
/*005deb3c*/ extern float k_f_n2000;
/*005de238*/ extern float k_f_2000;
/*007ef7ec*/ extern float g_frame_seconds;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d1e0*/ extern undefined4 g_frontend_state;
/*0069d1ec*/ extern undefined4 g_menu_screen_id;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2dc*/ extern undefined4 g_ui_69d2dc;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*0069d6a8*/ extern undefined4 g_menu_select_pressed;
/*0069d6ac*/ extern undefined4 g_message_69d6ac;
/*0069d6b0*/ extern undefined4 g_menu_69d6b0;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/* training_mode_update */

/* "Training mode: builds the per lesson text id tables (texts 0x2D6.. 0x312: brake, steering,
   battle mode, handbrake, weapon lessons) and runs the lesson state machine" */

/* Slot names txt_XXX are the original stack offsets (hex) of the text id table entries. */
/* Training mode, run on the per frame event (g_event_type 0x11): `frame` holds, per lesson step, a zero terminated list of text ids (0x2D6..0x312); lesson_texts points at the list for (g_training_lesson, g_car_8c86c8 = step). Then clears all player inputs, damps car velocities (x0.1), and drives the hint dialog state g_car_69d2d8 (1 idle/shown, 2 fading out, 3 next, 4 previous, 5 finished; inferred) with rec->entry_enabled[0..2] as fade alphas (0..255). */
void training_mode_update(void)

{
  struct MenuRecord *rec;
  byte *input_byte;
  int dialog_state;
  undefined4 *******lesson_texts;
  int scratch;
  struct {
    float vel_x;
    float vel_y;
    float vel_z;
    undefined4 ******txt_1c4;
    undefined4 txt_1c0;
    undefined4 *****txt_1bc;
    undefined4 txt_1b8;
    undefined4 *****txt_1b4;
    undefined4 txt_1b0;
    undefined4 *****txt_1ac;
    undefined4 txt_1a8;
    undefined4 *****txt_1a4;
    undefined4 txt_1a0;
    undefined4 *****txt_19c;
    undefined4 txt_198;
    undefined4 *****txt_194;
    undefined4 txt_190;
    undefined4 *****txt_18c;
    undefined4 txt_188;
    undefined4 *****txt_184;
    undefined4 txt_180;
    undefined4 *****txt_17c;
    undefined4 txt_178;
    undefined4 *****txt_174;
    undefined4 txt_170;
    undefined4 *****txt_16c;
    undefined4 txt_168;
    undefined4 *****txt_164;
    undefined4 txt_160;
    undefined4 *****txt_15c;
    undefined4 txt_158;
    undefined4 *****txt_154;
    undefined4 txt_150;
    undefined4 *****txt_14c;
    undefined4 txt_148;
    undefined4 *****txt_144;
    undefined4 txt_140;
    undefined4 *****txt_13c;
    undefined4 txt_138;
    undefined4 txt_134;
    undefined4 txt_130;
    undefined4 txt_12c;
    undefined4 *****txt_128;
    undefined4 txt_124;
    undefined4 txt_120;
    undefined4 txt_11c;
    undefined4 txt_118;
    undefined4 *****txt_114;
    undefined4 txt_110;
    undefined4 txt_10c;
    undefined4 *****txt_108;
    undefined4 txt_104;
    undefined4 txt_100;
    undefined4 *****txt_fc;
    undefined4 txt_f8;
    undefined4 txt_f4;
    undefined4 *****txt_f0;
    undefined4 txt_ec;
    undefined4 txt_e8;
    undefined4 *****txt_e4;
    undefined4 txt_e0;
    undefined4 txt_dc;
    undefined4 *****txt_d8;
    undefined4 txt_d4;
    undefined4 txt_d0;
    undefined4 *****txt_cc;
    undefined4 txt_c8;
    undefined4 txt_c4;
    undefined4 *****txt_c0;
    undefined4 txt_bc;
    undefined4 txt_b8;
    undefined4 *****txt_b4;
    undefined4 txt_b0;
    undefined4 txt_ac;
    undefined4 *****txt_a8;
    undefined4 txt_a4;
    undefined4 txt_a0;
    undefined4 *****txt_9c;
    undefined4 txt_98;
    undefined4 txt_94;
    undefined4 txt_90;
    undefined4 *****txt_8c;
    undefined4 txt_88;
    undefined4 txt_84;
    undefined4 txt_80;
    undefined4 *****txt_7c;
    undefined4 txt_78;
    undefined4 txt_74;
    undefined4 txt_70;
    undefined4 txt_6c;
    undefined4 *****txt_68;
    undefined4 txt_64;
    undefined4 txt_60;
    undefined4 txt_5c;
    undefined4 txt_58;
    undefined4 *****txt_54;
    undefined4 txt_50;
    undefined4 txt_4c;
    undefined4 txt_48;
    undefined4 txt_44;
    undefined4 txt_40;
    undefined4 *****txt_3c;
    undefined4 txt_38;
    undefined4 txt_34;
    undefined4 txt_30;
    undefined4 txt_2c;
    undefined4 txt_28;
    undefined4 *****txt_24;
    undefined4 txt_20;
    undefined4 txt_1c;
    undefined4 txt_18;
    undefined4 txt_14;
    undefined4 txt_10;
    undefined4 txt_c;
  } frame;
  
  rec = &g_menu_records[0];
  frame.txt_128 = (undefined4 *****)((undefined4 ******)0x2e7);
  frame.txt_13c = (undefined4 *****)((undefined4 ******)0x2e7);
  frame.txt_9c = (undefined4 *****)((undefined4 ******)0x2df);
  frame.txt_98 = 0x2e0;
  frame.txt_94 = 0x2e1;
  frame.txt_124 = 0x2df;
  frame.txt_120 = 0x2e0;
  frame.txt_11c = 0x2e1;
  frame.txt_44 = 0x2ec;
  frame.txt_5c = 0x2ec;
  frame.txt_24 = (undefined4 *****)((undefined4 ******)0x2d6);
  frame.txt_20 = 0x2d7;
  frame.txt_1c = 0x2d8;
  frame.txt_18 = 0x2d9;
  frame.txt_14 = 0x2da;
  frame.txt_10 = 0x2db;
  frame.txt_c = 0;
  frame.txt_144 = (undefined4 *****)((undefined4 ******)0x2dd);
  frame.txt_140 = 0;
  frame.txt_15c = (undefined4 *****)((undefined4 ******)0x2de);
  frame.txt_158 = 0;
  frame.txt_1ac = (undefined4 *****)((undefined4 ******)0x2dc);
  frame.txt_1a8 = 0;
  frame.txt_90 = 0;
  frame.txt_1c4 = (undefined4 ******)((undefined4 *******)0x2e6);
  frame.txt_1c0 = 0;
  frame.txt_118 = 0;
  frame.txt_138 = 0x2e3;
  frame.txt_134 = 0x2e4;
  frame.txt_130 = 0x2e5;
  frame.txt_12c = 0;
  frame.txt_54 = (undefined4 *****)((undefined4 ******)0x2e8);
  frame.txt_50 = 0x2e9;
  frame.txt_4c = 0x2ea;
  frame.txt_48 = 0x2eb;
  frame.txt_40 = 0;
  frame.txt_16c = (undefined4 *****)((undefined4 ******)0x2ed);
  frame.txt_168 = 0;
  frame.txt_68 = (undefined4 *****)((undefined4 ******)0x2ee);
  frame.txt_64 = 0x2ea;
  frame.txt_60 = 0x2eb;
  frame.txt_58 = 0;
  frame.txt_19c = (undefined4 *****)((undefined4 ******)0x2ef);
  frame.txt_198 = 0;
  frame.txt_14c = (undefined4 *****)((undefined4 ******)0x2f0);
  frame.txt_148 = 0;
  frame.txt_d8 = (undefined4 *****)((undefined4 ******)0x312);
  frame.txt_d4 = 0x2f0;
  frame.txt_d0 = 0;
  frame.txt_114 = (undefined4 *****)((undefined4 ******)0x2f1);
  frame.txt_110 = 0x2f2;
  frame.txt_10c = 0;
  frame.txt_18c = (undefined4 *****)((undefined4 ******)0x2f3);
  frame.txt_188 = 0;
  frame.txt_a8 = (undefined4 *****)((undefined4 ******)0x2f4);
  frame.txt_a4 = 0x2f5;
  frame.txt_a0 = 0;
  frame.txt_17c = (undefined4 *****)((undefined4 ******)0x2f6);
  frame.txt_178 = 0;
  frame.txt_fc = (undefined4 *****)((undefined4 ******)0x2f7);
  frame.txt_f8 = 0x2f8;
  frame.txt_f4 = 0;
  frame.txt_1bc = (undefined4 *****)((undefined4 ******)0x2f9);
  frame.txt_1b8 = 0;
  frame.txt_c0 = (undefined4 *****)((undefined4 ******)0x2fa);
  frame.txt_bc = 0x2fb;
  frame.txt_b8 = 0;
  frame.txt_70 = 0x30f;
  frame.txt_2c = 0x30f;
  frame.txt_84 = 0x2e5;
  frame.txt_174 = (undefined4 *****)((undefined4 ******)0x2fc);
  frame.txt_170 = 0;
  frame.txt_cc = (undefined4 *****)((undefined4 ******)0x2fd);
  frame.txt_c8 = 0x2fe;
  frame.txt_c4 = 0;
  frame.txt_1b4 = (undefined4 *****)((undefined4 ******)0x2ff);
  frame.txt_1b0 = 0;
  frame.txt_e4 = (undefined4 *****)((undefined4 ******)0x300);
  frame.txt_e0 = 0x301;
  frame.txt_dc = 0;
  frame.txt_1a4 = (undefined4 *****)((undefined4 ******)0x302);
  frame.txt_1a0 = 0;
  frame.txt_b4 = (undefined4 *****)((undefined4 ******)0x303);
  frame.txt_b0 = 0x304;
  frame.txt_ac = 0;
  frame.txt_194 = (undefined4 *****)((undefined4 ******)0x305);
  frame.txt_190 = 0;
  frame.txt_108 = (undefined4 *****)((undefined4 ******)0x306);
  frame.txt_104 = 0x307;
  frame.txt_100 = 0;
  frame.txt_184 = (undefined4 *****)((undefined4 ******)0x308);
  frame.txt_180 = 0;
  frame.txt_f0 = (undefined4 *****)((undefined4 ******)0x309);
  frame.txt_ec = 0x30a;
  frame.txt_e8 = 0;
  frame.txt_164 = (undefined4 *****)((undefined4 ******)0x30b);
  frame.txt_160 = 0;
  frame.txt_8c = (undefined4 *****)((undefined4 ******)0x2e3);
  frame.txt_88 = 0x2e4;
  frame.txt_80 = 0;
  frame.txt_7c = (undefined4 *****)((undefined4 ******)0x30c);
  frame.txt_78 = 0x30d;
  frame.txt_74 = 0x30e;
  frame.txt_6c = 0;
  frame.txt_3c = (undefined4 *****)((undefined4 ******)0x311);
  frame.txt_38 = 0x30c;
  frame.txt_34 = 0x30d;
  frame.txt_30 = 0x30e;
  frame.txt_28 = 0;
  frame.txt_154 = (undefined4 *****)((undefined4 ******)0x310);
  frame.txt_150 = 0;
  if (g_event_type != EVENT_TRAINING) {
    return;
  }
  training_hint_labels_update();
  if (g_car_69d2d8 == 0) {
    return;
  }
  if (g_training_lesson == 0) {
    if (g_car_8c86c8 == 1) {
      lesson_texts = (undefined4 *******)&frame.txt_144;
    }
    else if (g_car_8c86c8 == 2) {
      lesson_texts = (undefined4 *******)&frame.txt_15c;
    }
    else {
      lesson_texts = (undefined4 *******)&frame.txt_24;
      if (g_car_8c86c8 == 3) {
        lesson_texts = (undefined4 *******)&frame.txt_1ac;
      }
    }
  }
  else if (g_training_lesson == 1) {
    if (g_car_8c86c8 == 1) {
      g_car_8c86c8 = 2;
      lesson_texts = &frame.txt_1c4;
    }
    else if (g_car_8c86c8 == 2) {
      lesson_texts = &frame.txt_1c4;
    }
    else if (g_car_8c86c8 == 3) {
      lesson_texts = (undefined4 *******)&frame.txt_128;
    }
    else {
      lesson_texts = (undefined4 *******)&frame.txt_9c;
      if (g_car_8c86c8 == 4) {
        lesson_texts = (undefined4 *******)&frame.txt_13c;
      }
    }
  }
  else if (g_training_lesson == 2) {
    if (g_car_8c86c8 == 1) {
      lesson_texts = (undefined4 *******)&frame.txt_16c;
    }
    else if (g_car_8c86c8 == 2) {
      lesson_texts = (undefined4 *******)&frame.txt_19c;
    }
    else {
      lesson_texts = (undefined4 *******)&frame.txt_54;
      if (g_car_8c86c8 == 3) {
        lesson_texts = (undefined4 *******)&frame.txt_68;
      }
    }
  }
  else if (g_training_lesson == 3) {
    if (g_car_8c86c8 == 1) {
      lesson_texts = (undefined4 *******)&frame.txt_114;
    }
    else if (g_car_8c86c8 == 2) {
      lesson_texts = (undefined4 *******)&frame.txt_18c;
    }
    else if (g_car_8c86c8 == 3) {
      lesson_texts = (undefined4 *******)&frame.txt_a8;
    }
    else if (g_car_8c86c8 == 4) {
      lesson_texts = (undefined4 *******)&frame.txt_17c;
    }
    else if (g_car_8c86c8 == 5) {
      lesson_texts = (undefined4 *******)&frame.txt_fc;
    }
    else if (g_car_8c86c8 == 6) {
      lesson_texts = (undefined4 *******)&frame.txt_1bc;
    }
    else if (g_car_8c86c8 == 7) {
      lesson_texts = (undefined4 *******)&frame.txt_c0;
    }
    else if (g_car_8c86c8 == 8) {
      lesson_texts = (undefined4 *******)&frame.txt_174;
    }
    else if (g_car_8c86c8 == 9) {
      lesson_texts = (undefined4 *******)&frame.txt_cc;
    }
    else if (g_car_8c86c8 == 10) {
      lesson_texts = (undefined4 *******)&frame.txt_1b4;
    }
    else if (g_car_8c86c8 == 0xb) {
      lesson_texts = (undefined4 *******)&frame.txt_e4;
    }
    else if (g_car_8c86c8 == 0xc) {
      lesson_texts = (undefined4 *******)&frame.txt_1a4;
    }
    else if (g_car_8c86c8 == 0xd) {
      lesson_texts = (undefined4 *******)&frame.txt_b4;
    }
    else if (g_car_8c86c8 == 0xe) {
      lesson_texts = (undefined4 *******)&frame.txt_194;
    }
    else if (g_car_8c86c8 == 0xf) {
      lesson_texts = (undefined4 *******)&frame.txt_108;
    }
    else if (g_car_8c86c8 == 0x10) {
      lesson_texts = (undefined4 *******)&frame.txt_184;
    }
    else if (g_car_8c86c8 == 0x11) {
      lesson_texts = (undefined4 *******)&frame.txt_f0;
    }
    else if (g_car_8c86c8 == 0x12) {
      lesson_texts = (undefined4 *******)&frame.txt_164;
    }
    else {
      lesson_texts = (undefined4 *******)&frame.txt_14c;
      if (g_car_8c86c8 == 100) {
        lesson_texts = (undefined4 *******)&frame.txt_d8;
      }
    }
  }
  else {
    lesson_texts = (undefined4 *******)frame.txt_1c4;
    if (g_training_lesson == 4) {
      if (g_car_8c86c8 == 0) {
        g_car_8c86c8 = 1;
        lesson_texts = (undefined4 *******)&frame.txt_8c;
      }
      else if (g_car_8c86c8 == 1) {
        lesson_texts = (undefined4 *******)&frame.txt_8c;
      }
      else if (g_car_8c86c8 == 2) {
        lesson_texts = &frame.txt_1c4;
      }
      else if (g_car_8c86c8 == 3) {
        lesson_texts = (undefined4 *******)&frame.txt_128;
      }
      else if (g_car_8c86c8 == 4) {
        lesson_texts = (undefined4 *******)&frame.txt_13c;
      }
    }
    else if (g_training_lesson == 5) {
      if (g_car_8c86c8 == 1) {
        g_car_8c86c8 = 2;
        lesson_texts = (undefined4 *******)&frame.txt_154;
      }
      else if (g_car_8c86c8 == 2) {
        lesson_texts = (undefined4 *******)&frame.txt_154;
      }
      else {
        lesson_texts = (undefined4 *******)&frame.txt_7c;
        if (g_car_8c86c8 == 3) {
          lesson_texts = (undefined4 *******)&frame.txt_3c;
        }
      }
    }
  }
  /* clear every digital input of the 12 player input slots (stride 0x4c) */
  input_byte = &g_player_inputs[0].right;
  do {
    ((PlayerInput *)(input_byte + -1))->left = 0;
    *input_byte = 0;
    input_byte[1] = 0;
    input_byte[2] = 0;
    input_byte[3] = 0;
    input_byte[4] = 0;
    input_byte[5] = 0;
    input_byte[6] = 0;
    input_byte[7] = 0;
    input_byte = input_byte + 0x4c;
  } while ((int)input_byte < (int)&g_player_inputs + 0x391);
  /* per car (4 cars): reset physics and scale the velocity by k_f_n0_1 (-0.1 inferred from the constant name) */
  scratch = 0;
  do {
    car_reset_physics(scratch);
    car_get_velocity_any_mode(&frame.vel_x,scratch);
    frame.vel_x = frame.vel_x * k_f_n0_1;
    frame.vel_y = frame.vel_y * k_f_n0_1;
    frame.vel_z = frame.vel_z * k_f_n0_1;
    car_add_velocity(&frame.vel_x,scratch);
    dialog_state = g_car_69d2d8;
    scratch = scratch + 1;
  } while (scratch < 4);
  if (g_frontend_state == 0) {
    g_menu_select_pressed = 0;
    g_message_69d6ac = 0;
    g_menu_69d6b0 = 0;
  }
  if (((int)g_car_69d2d8 < 3) && (g_car_69d2d8 != 0)) {
    scratch = (int)(g_frame_seconds * k_f_n2000);
    rec->entry_enabled[0] = rec->entry_enabled[0] - scratch;
    rec->entry_enabled[1] = rec->entry_enabled[1] - scratch;
  }
  /* alpha cross fade: state 3 fades entry 1 in / entry 2 out, state 4 the reverse (inferred) */
  if (dialog_state == 3) {
    scratch = (int)(g_frame_seconds * (rec->entry_enabled[1] == 0 ? k_f_2000 : k_f_n2000));
    if (rec->entry_enabled[1] == 0) rec->entry_enabled[2] = rec->entry_enabled[2] + scratch;
    else rec->entry_enabled[1] = rec->entry_enabled[1] + scratch;
  }
  else if (dialog_state == 4) {
    scratch = (int)(g_frame_seconds * (rec->entry_enabled[2] == 0 ? k_f_2000 : k_f_n2000));
    if (rec->entry_enabled[2] == 0) rec->entry_enabled[1] = rec->entry_enabled[1] + scratch;
    else rec->entry_enabled[2] = rec->entry_enabled[2] + scratch;
  }
  /* select pressed: advance to the next text page (state 3, or 5 when there is no further list) */
  scratch = menu_select_pressed_any(0);
  if (scratch != 0) {
    g_car_69d2d8 = 3;
    (*(byte *)((char *)&rec->entry_enabled[5] + 0)) = 1;
    g_menu_select_pressed = 1;
    if (rec->item_list == 0) {
      g_car_69d2d8 = 5;
    }
  }
  /* back pressed with a page before this one: go to the previous page (state 4); otherwise run the state 5 / state 1 handling */
  dialog_state = menu_back_pressed_any(0);
  scratch = g_car_69d2d8;
  if ((dialog_state == 0) || (rec->selected == 0)) {
    if (g_car_69d2d8 == 5) {
      dialog_state = (int)(g_frame_seconds * k_f_2000);
      rec->entry_enabled[0] = rec->entry_enabled[0] - dialog_state;
      rec->entry_enabled[1] = rec->entry_enabled[1] - dialog_state;
      if (-1 < rec->entry_enabled[0]) goto LAB_00445af1;
      scratch = 2;
      if (g_training_lesson == 3) {
        scratch = 0x12;
      }
      /* last step of the lesson (2, or 0x12 for lesson 3) reached: leave to the frontend, mark the career row done; else start the countdown for the next step */
      if (g_car_8c86c8 == scratch) {
        menu_page_enter(MENU_PAGE_PAUSE,2);
        if (g_event_type == EVENT_TRAINING) {
          ui_menu_reset();
        }
        race_restart();
        time_clear_current();
        app_flag_76e7f8_set();
        app_enter_frontend(6);
        if (g_game_state.progress[g_career_row * 0x12 + 0x11] != 2) {
          autosave_request();
        }
        scratch = g_career_row;
        g_game_state.progress[g_career_row * 0x12 + 0x11] = 2;
        if (g_game_state.progress[scratch * 0x12 + 0x23] == 0) {
          g_game_state.progress[scratch * 0x12 + 0x23] = 1;
        }
        g_menu_screen_id = 0;
      }
      else {
        g_car_69d2d8 = 0;
        race_begin_countdown();
      }
    }
    scratch = g_car_69d2d8;
    if (g_car_69d2d8 == 1) {
      scratch = 2;
      g_car_69d2d8 = 2;
    }
  }
  else {
    rec->selected = rec->selected + -1;
    g_car_69d2d8 = 4;
    (*(byte *)((char *)&rec->entry_enabled[5] + 0)) = 1;
    g_message_69d6ac = 1;
    rec->entry_enabled[1] = 0;
    rec->entry_enabled[2] = 0xff;
    scratch = 4;
  }
/* clamp the three fade alphas to 0..255 */
LAB_00445af1:
  if (0xff < rec->entry_enabled[0]) {
    rec->entry_enabled[0] = 0xff;
  }
  if (0xff < rec->entry_enabled[1]) {
    rec->entry_enabled[1] = 0xff;
  }
  if (0xff < rec->entry_enabled[2]) {
    rec->entry_enabled[2] = 0xff;
  }
  if (rec->entry_enabled[0] < 0) {
    rec->entry_enabled[0] = 0;
  }
  if (rec->entry_enabled[1] < 0) {
    rec->entry_enabled[1] = 0;
  }
  if (rec->entry_enabled[2] < 0) {
    rec->entry_enabled[2] = 0;
  }
  if (scratch == 3) {
    if ((rec->entry_enabled[1] != 0) || (rec->entry_enabled[2] != 0xff))
    goto LAB_00445b92;
    rec->selected = rec->selected + 1;
  }
  else if (((scratch != 4) || (rec->entry_enabled[2] != 0)) ||
          (rec->entry_enabled[1] != 0xff)) goto LAB_00445b92;
  g_car_69d2d8 = 1;
  rec->entry_enabled[1] = 0xff;
  rec->entry_enabled[2] = 0;
LAB_00445b92:
  rec->item_list = (char *)((uint)lesson_texts[rec->selected + 1]);
  g_ui_69d2dc = lesson_texts[rec->selected];
  return;
}

