// cluster-entries: message_box_update
/* Installed cluster message_box_update (tools/cluster_install.py from output/cluster_pilot_x4/message_box_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) void __stdcall PostQuitMessage(int exit_code);
/*0069d1e0*/ extern int g_frontend_state;
/*0069cfd0*/ extern int g_message_state;
/*0069cfe4*/ extern int g_message_timer;
/*0069cff0*/ extern int g_message_code;
/*0069cff4*/ extern int g_message_buttons;
/*0069cff8*/ extern unsigned char g_message_box_69cff8;
/*0069cff9*/ extern unsigned char g_message_box_69cff9;
/*0069cffa*/ extern unsigned char g_message_box_69cffa;
/*0069d00c*/ extern int g_message_box_69d00c;
/*0069d010*/ extern int g_message_text_offset;
/*0069d6a8*/ extern int g_menu_select_pressed;
/*0069d6ac*/ extern int g_message_69d6ac;
/*0069d6b0*/ extern int g_menu_69d6b0;
/*0069d6b4*/ extern int g_menu_69d6b4;
/*0069cbe8*/ extern int g_ui_69cbe8[12];
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007ef7ec*/ extern float g_frame_seconds;
/*005ddb58*/ extern float k_f_1000;
/*005decfc*/ extern float k_f_n1000;
/*0069cbd4*/ extern int g_event_type;
/*0069caec*/ extern int g_menu_start_race_requested;
/*0069d1f0*/ extern int g_menu_69d1f0;
/*0069d2d0*/ extern int g_menu_career_race_active;
/*0069cc40*/ extern int g_team_mode;
/*0069d21c*/ extern int g_menu_multiplayer_kind;
/*0069cc4c*/ extern int g_menu_69cc4c;
/*0069d688*/ extern int g_career_row;
/*0069cfa0*/ extern int g_menu_selected_vehicle;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*0069d228*/ extern int g_menu_69d228;
/*0069d22c*/ extern int g_menu_69d22c;
/*0069d248*/ extern int g_message_69d248;
/*0069d24c*/ extern int g_menu_69d24c;
/*0069d254*/ extern int g_message_box_69d254;
/*0069d258*/ extern int g_message_box_69d258;
/*0069d25c*/ extern int g_message_box_69d25c;
/*0069caf0*/ extern int g_message_69caf0;
/*0069caf4*/ extern int g_message_69caf4;
/*0069caf8*/ extern int g_message_69caf8;
/*0069cafc*/ extern int g_message_69cafc;
/*0069cb00*/ extern int g_message_69cb00;
/*0069cb04*/ extern int g_message_69cb04;
/*0069cb08*/ extern int g_message_69cb08;
/*00431a80*/ extern int player_slots_count_human_with_controller(void);
/*004316a0*/ extern int message_box_active_count(void);
/*00430010*/ extern int menu_select_pressed_any(int player);
/*004300b0*/ extern int menu_back_pressed_any(void);
/*00430150*/ extern int menu_pause_pressed_any(void);
/*00444c10*/ extern void menu_page_enter(int page, int arg);
/*0040fdd0*/ extern void race_set_phase(int phase);
/*00412d60*/ extern void race_restart(void);
/*0042ec00*/ extern void best_time_load_current(void);
/*0042e9e0*/ extern void time_clear_current(void);
/*004a2970*/ extern void app_flag_76e7f8_set(void);
/*004a2980*/ extern void app_flag_76e7fc_set(void);
/*00430b50*/ extern void ui_input_mode_menu(void);
/*00431710*/ extern void ui_menu_reset(void);
/*00426650*/ extern void player_stats_clear_b(void);
/*0040cdb0*/ extern void round_stats_reset(void);
/*004169f0*/ extern int *career_event_slot_table(void);
/*0047bc80*/ extern void car_set_field_8(int car, int vehicle);
/*00431130*/ extern void message_show_now(int a, int b, int c, int d, int e, int f);
/*0040c0c0*/ extern void gamesave_cb_dispatch_a(void);
/*0040c200*/ extern void gamesave_cb_dispatch_b(void);

/* g_message_state: 0 idle, 1 just shown (becomes 2 at the end of the frame), 2 waiting for a button, 3 closing (the timer runs out, then the result is dispatched) */
#define MSG_IDLE 0
#define MSG_OPENED 1
#define MSG_WAITING 2
#define MSG_CLOSING 3
#define MAX_PLAYER_INPUTS 12
#define TIMER_MAX 255

/* g_message_code = the menu item id that opened the box (0xffNN0000); the result is dispatched when the box has closed */
#define MSG_RESTART_RACE 0xff1e0000     /* "restart?" */
#define MSG_TO_MAIN_MENU 0xff1f0000
#define MSG_TO_FRONTEND 0xff200000
#define MSG_INFO_A 0xff210000           /* box closes when any pad releases pause (keeps the pause latches) */
#define MSG_INFO_B 0xff220000           /* box closes when any pad releases pause */
#define MSG_START_EVENT 0xff240000
#define MSG_START_EVENT_B 0xff270000
#define MSG_TEAM_EVENT 0xff280000
#define MSG_EXIT_RACE 0xff320000
#define MSG_GAMESAVE 0xff460000
#define MSG_QUIT_GAME 0xff800000

static void message_box_confirm(void)
{
  g_message_state = MSG_CLOSING;
  g_message_box_69cff8 = 1;
  g_menu_select_pressed = 1;
}

/* Pending event start of the career: the opponents take the slots 1.. in order, a team colour that equals the player's is replaced by the first free one. */
static void message_box_setup_team_event(void)
{
  int *table;
  int *row;
  int count;
  int i;
  int skip;
  int team;
  int c;

  g_team_mode = 1;
  g_menu_multiplayer_kind = 0;
  g_menu_69cc4c = 1;
  g_player_slots[1].controller = -1;
  g_player_slots[2].controller = -1;
  g_player_slots[3].controller = -1;
  g_player_slots[0].state = 0;
  g_player_slots[1].state = -1;
  g_player_slots[2].state = -1;
  g_player_slots[3].state = -1;
  table = career_event_slot_table();
  row = table + g_career_row * 12;
  count = row[0];
  skip = 0;
  for (i = 0; i < count; i++) {
    if (g_player_slots[0].controller == i + 1) {
      skip = skip + 1;
    }
    g_player_slots[i + 1].controller = skip + i + 1;
    team = row[i + 1];
    if (team == g_player_slots[0].team) {
      for (c = 0; c < 6; c++) {
        if (c != g_player_slots[0].team && c != row[1] && c != row[2] && c != row[3]) {
          team = c;
          break;
        }
      }
    }
    g_player_slots[i + 1].team = team;
    g_player_slots[i + 1].state = 1;
  }
  car_set_field_8(0, g_menu_selected_vehicle);
  menu_page_enter(0, 2);
  g_menu_start_race_requested = 1;
  race_set_phase(1);
  g_menu_69d1f0 = 1;
}

/* Message box state machine, once per frame: fade the timer in, poll the buttons, close the box and dispatch the result. */
void message_box_update(void)
{
  int state;
  int i;
  int code;

  state = g_message_state;
  if (g_frontend_state == 0) {
    g_menu_select_pressed = 0;
    g_message_69d6ac = 0;
    g_menu_69d6b0 = 0;
  }
  if (state < MSG_CLOSING && state != MSG_IDLE) {
    g_message_timer = g_message_timer + (int)(g_frame_seconds * k_f_1000);
    if (g_message_timer > TIMER_MAX) {
      g_message_timer = TIMER_MAX;
    }
  }
  if (state == MSG_WAITING) {
    g_message_box_69cffa = 0;
    g_message_box_69cff8 = 0;
    g_message_box_69cff9 = 0;
    if (g_message_code == (int)MSG_INFO_A) {
      player_slots_count_human_with_controller();
      for (i = 0; i < MAX_PLAYER_INPUTS; i++) {
        if (g_ui_69cbe8[i] != 0 && g_player_inputs[i].pause != 0 && g_player_inputs_previous[i].pause == 0) {
          g_ui_69cbe8[i] = 0;
          if (i < 6) {
            g_ui_69cbe8[i + 6] = 0;
          }
        }
      }
      message_box_active_count();
      for (i = 0; i < MAX_PLAYER_INPUTS; i++) {
        if (g_ui_69cbe8[i] != 0) {
          break;
        }
      }
      if (i == MAX_PLAYER_INPUTS) {
        g_message_state = MSG_CLOSING;
        g_message_box_69cffa = 1;
      }
    } else if (g_message_code == (int)MSG_INFO_B) {
      for (i = 0; i < MAX_PLAYER_INPUTS; i++) {
        if (g_player_inputs[i].pause != 0 && g_player_inputs_previous[i].pause == 0) {
          g_message_state = MSG_CLOSING;
          g_message_box_69cffa = 1;
        }
      }
    }
    switch (g_message_buttons) {
    case 1:
      if (menu_select_pressed_any(0) != 0 || menu_back_pressed_any() != 0) {
        message_box_confirm();
      }
      break;
    case 11:
      if (menu_select_pressed_any(0) != 0) {
        message_box_confirm();
      }
      break;
    case 2:
    case 12:
      if (menu_select_pressed_any(0) != 0) {
        message_box_confirm();
      }
      if (menu_back_pressed_any() != 0) {
        g_message_state = MSG_CLOSING;
        g_message_box_69cff9 = 1;
        g_message_69d6ac = 1;
      }
      break;
    case 7:
      if (menu_pause_pressed_any() != 0) {
        g_message_state = MSG_CLOSING;
        g_message_box_69cffa = 1;
        g_menu_select_pressed = 1;
      }
      break;
    case 9:
      if (menu_select_pressed_any(0) != 0) {
        g_message_state = MSG_CLOSING;
        g_menu_select_pressed = 1;
      }
      if (menu_back_pressed_any() != 0) {
        g_message_text_offset = g_message_text_offset + 1;
        if (g_message_text_offset == g_message_box_69d00c) {
          g_message_text_offset = 0;
        }
        g_message_69d6ac = 1;
      }
      break;
    }
  }
  if (g_message_state == MSG_CLOSING) {
    g_message_timer = g_message_timer + (int)(g_frame_seconds * k_f_n1000);
    if (g_message_timer >= 0) {
      return;
    }
    code = g_message_code;
    g_message_timer = 0;
    g_message_state = MSG_IDLE;
    g_message_code = 0;
    switch (code) {
    case MSG_RESTART_RACE:
      if (g_message_box_69cff8 != 0) {
        menu_page_enter(0, 2);
        if (g_event_type == 0x11) {
          ui_menu_reset();
        }
        race_restart();
        g_menu_start_race_requested = 1;
        time_clear_current();
      }
      break;
    case MSG_TO_MAIN_MENU:
      if (g_message_box_69cff8 != 0) {
        app_flag_76e7f8_set();
        g_menu_69d6b4 = 0;
        menu_page_enter(7, 0);
        race_restart();
        if (g_menu_career_race_active != 0) {
          g_event_type = 3;
        }
      }
      break;
    case MSG_TO_FRONTEND:
      if (g_message_box_69cff8 != 0) {
        app_flag_76e7fc_set();
        ui_input_mode_menu();
        menu_page_enter(1, 0);
        race_restart();
      }
      break;
    case MSG_INFO_A:
    case MSG_INFO_B:
      if (g_menu_69d228 > 0) {
        if (g_message_69d248 != -1) {
          message_show_now(g_menu_69d22c, g_message_69d248, g_menu_69d24c, g_message_box_69d254, g_message_box_69d258, g_message_box_69d25c);
        }
        g_message_69d248 = 0;
        g_menu_69d228 = 0;
      }
      break;
    case MSG_START_EVENT:
    case MSG_START_EVENT_B:
      if (g_message_box_69cff8 != 0) {
        menu_page_enter(0, 2);
        g_menu_start_race_requested = 1;
        race_set_phase(1);
        g_menu_69d1f0 = 4;
        best_time_load_current();
        time_clear_current();
      }
      if (g_message_box_69cff9 != 0) {
        g_menu_69d6b4 = 0;
        menu_page_enter(7, 0);
        if (g_menu_career_race_active == 2) {
          g_event_type = 3;
        }
      }
      break;
    case MSG_TEAM_EVENT:
      if (g_message_box_69cff8 != 0) {
        message_box_setup_team_event();
      }
      break;
    case MSG_EXIT_RACE:
      if (g_message_box_69cff8 != 0) {
        menu_page_enter(7, 0);
        player_stats_clear_b();
        round_stats_reset();
      }
      if (g_message_box_69cff9 != 0) {
        g_menu_69d1f0 = 0;
        ui_input_mode_menu();
        menu_page_enter(1, 0);
        player_stats_clear_b();
        round_stats_reset();
      }
      break;
    case MSG_GAMESAVE:
      if (g_message_box_69cff8 != 0) {
        gamesave_cb_dispatch_a();
      }
      if (g_message_box_69cff9 != 0) {
        gamesave_cb_dispatch_b();
      }
      break;
    case MSG_QUIT_GAME:
      if (g_message_box_69cff8 != 0) {
        PostQuitMessage(0);
      }
      break;
    }
    if (g_message_69cb08 != 0) {
      message_show_now(g_message_69caf0, g_message_69caf4, g_message_69caf8, g_message_69cafc, g_message_69cb00, g_message_69cb04);
      g_message_69cb08 = 0;
    }
  }
  if (g_message_state == MSG_OPENED) {
    g_message_state = MSG_WAITING;
  }
}

