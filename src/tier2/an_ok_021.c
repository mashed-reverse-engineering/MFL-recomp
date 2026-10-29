#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*0042bd00*/ extern void * dat_get_string();
/*00432f20*/ extern undefined __cdecl credits_draw(undefined4 p0);
/*004755b0*/ extern int aux_frame_destroy();
/*004824a0*/ extern int draw_rect();
/*004827a0*/ extern int gfx_draw_rect_2d_flat_a();
/*00482920*/ extern int gfx_draw_rect_2d_flat_b();
/*004a5b20*/ extern int __cdecl controller_present_thunk_4a5b20(int p0);
/*004b2a60*/ extern int sprintf();
/*004b2b48*/ extern int _ftol();
/*004b4ba7*/ extern int entry();
/*004c7cc0*/ extern undefined __cdecl message(undefined4 p0, undefined4 p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1ec0*/ extern struct RwCamera * RwCameraCreate();
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*005de9b4*/ extern undefined g_frontend_5de9b4;
/*005de9b8*/ extern undefined g_ui_5de9b8;
/*005de9c0*/ extern undefined g_ui_5de9c0;
/*00609c48*/ extern undefined g_menu_page_01_game_type_select;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069caf0*/ extern undefined4 g_message_69caf0;
/*0069caf4*/ extern undefined4 g_message_69caf4;
/*0069caf8*/ extern undefined4 g_message_69caf8;
/*0069cafc*/ extern undefined4 g_message_69cafc;
/*0069cb00*/ extern undefined4 g_message_69cb00;
/*0069cb04*/ extern undefined4 g_message_69cb04;
/*0069cb08*/ extern undefined4 g_message_69cb08;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cbe8*/ extern undefined4 g_ui_69cbe8;
/*0069cbec*/ extern undefined4 g_ui_69cbec;
/*0069cc2c*/ extern undefined2 g_ui_69cc2c;
/*0069cc2e*/ extern undefined2 g_ui_69cc2e;
/*0069cc38*/ extern undefined4 g_message_show_69cc38;
/*0069cc48*/ extern undefined4 g_menu_69cc48;
/*0069cc50*/ extern undefined4 g_menu_state_69cc50;
/*0069cc54*/ extern undefined4 g_menu_state_69cc54;
/*0069cc58*/ extern undefined4 g_menu_state_69cc58;
/*0069cc5c*/ extern undefined4 g_menu_state_69cc5c;
/*0069cc8c*/ extern undefined4 g_menu_state_69cc8c;
/*0069cfd0*/ extern undefined4 g_message_state;
/*0069cfd4*/ extern undefined4 g_message_text_id;
/*0069cfe4*/ extern undefined1 g_message_timer;
/*0069cff0*/ extern undefined4 g_message_code;
/*0069cff4*/ extern undefined4 g_message_buttons;
/*0069cffc*/ extern undefined4 g_message_button_text_a;
/*0069d000*/ extern undefined4 g_message_button_text_b;
/*0069d004*/ extern undefined4 g_message_show_69d004;
/*0069d008*/ extern undefined4 g_message_use_text_offset;
/*0069d028*/ extern undefined4 player_color_id;
/*0069d034*/ extern undefined4 g_menu_69d034;
/*0069d0fc*/ extern undefined4 g_credits_update_69d0fc;
/*0069d1e0*/ extern undefined4 g_frontend_state;
/*0069d1e4*/ extern undefined4 g_menu_fade_alpha;
/*0069d1ec*/ extern undefined4 g_menu_screen_id;
/*0069d1f0*/ extern undefined4 g_menu_69d1f0;
/*0069d1f4*/ extern undefined4 g_ui_69d1f4;
/*0069d2cc*/ extern undefined4 g_menu_state_69d2cc;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2dc*/ extern undefined4 g_ui_69d2dc;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01f4*/ extern undefined4 g_input_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c807c*/ extern undefined4 g_car_8c807c;
/*008c8090*/ extern undefined4 g_menu_8c8090;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/* menu_get_69d1e4 */

/* "byte g_menu_69d1e4" */

extern undefined1 menu_get_69d1e4(void);


/* menu_selection_step */

/* "(delta): moves the selected entry of the current menu, wrapping around" */

extern void menu_selection_step(char *param_1);


/* menu_current_item_offset */

/* "computes the item list offset of the selected entry of the current menu" */

extern undefined4 menu_current_item_offset(void);


/* menu_select_pressed_any */

/* "edge detect of menu_select on any of the 12 player inputs (blocked while a message is closing)"
    */

extern undefined4 __cdecl menu_select_pressed_any(char *param_1);
extern char g_hud_8c86bc[];
extern char g_message_box_69cc1c[];
extern char g_ai_editor_view_origin_x[];
extern char g_menu_update_69d0b8[];
extern char g_ai_editor_mode[];
extern char g_ui_69d0ac[];
extern char g_ui_69d0c4[];
extern char g_player_slot3_extra[];
extern char g_player_inputs_previous_8__menu_select[];

#define INPUT_MODE_MENU (0x1000)
#define CONTROLLER_NONE (-1)
#define PLAYER_INPUT_SIZE (0x4c)


/* menu_back_pressed_any */

/* "edge detect of menu_back on any player input" */

/* Returns 1 if menu_back was pressed this frame (down now, up on the previous frame) on any
   player input, else 0. Takes no parameters.
   Returns 0 while a message box is up (inferred: g_menu_8c8090 set and g_message_state != 2).
   In menu input mode it scans the previous-input table, otherwise the player slots. */
/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 menu_back_pressed_any(void)
{
  byte *prev_input_menu_back;
  int slot_type;
  PlayerSlot *slot;
  int slot_index;

  if ((g_message_state != 2) && (g_menu_8c8090 != 0)) {
    return 0;
  }
  /* menu input mode: walk g_player_inputs_previous one PlayerInput at a time until a menu_back
     that is new this frame (current entry 1, previous entry 0). The current table
     g_player_inputs sits 0x4c0 bytes before the previous table. */
  if (g_input_mode == INPUT_MODE_MENU) {
    prev_input_menu_back = &g_player_inputs_previous[0].menu_back;
    while ((prev_input_menu_back[-0x4c0] == '\0' || (*prev_input_menu_back != 0))) {
      prev_input_menu_back = prev_input_menu_back + PLAYER_INPUT_SIZE;
      if (((LONG_PTR)&g_player_inputs_previous_8__menu_select) < (LONG_PTR)prev_input_menu_back) {
        return 0;
      }
    }
    return 1;
  }
  /* otherwise walk the 4 player slots: skip slots with no controller and slot type 2
     (inferred: not a human player; type 1 is human), return 1 for the first slot whose
     controller has menu_back newly pressed */
  slot_index = 0;
  slot = g_player_slots;
  while ((((slot->controller == CONTROLLER_NONE ||
           (slot_type = race_setup_get_slot_type(slot_index), slot_type == 2)) ||
          (g_player_inputs[slot->controller].menu_back == 0)) ||
         (g_player_inputs_previous[slot->controller].menu_back != 0))) {
    slot = slot + 1;
    slot_index = slot_index + 1;
    if (((LONG_PTR)&g_player_slot3_extra + 0x3) < (LONG_PTR)slot) {
      return 0;
    }
  }
  return 1;
}


/* menu_pause_pressed_any */

/* "edge detect of pause on any player input" */

extern undefined4 menu_pause_pressed_any(void);


/* menu_input_next_pressed */

/* "edge detect of menu_next on controller 0 (any player slot)" */

extern undefined4 menu_input_next_pressed(void);


/* ui_set_ui_69cc2c */

/* "g_ui_69cc2c = param_1; return;" */

extern void ui_set_ui_69cc2c(undefined2 param_1);


/* ui_set_ui_69cc2e */

/* "g_ui_69cc2e = param_1; return;" */

extern void ui_set_ui_69cc2e(undefined2 param_1);


/* screen_width */

/* "short at 0x69CC2C (pixels)" */

extern undefined2 screen_width(void);


/* screen_height */

/* "short at 0x69CC2E (pixels)" */

extern undefined2 screen_height(void);


/* ui_state_is_0 */

/* "g_ui_69d1e0 == 0" */

extern bool ui_state_is_0(void);


/* ui_state_is_2 */

/* "g_ui_69d1e0 == 2" */

extern bool ui_state_is_2(void);


/* ui_state_is_5 */

/* "g_ui_69d1e0 == 5" */

extern bool ui_state_is_5(void);


/* ui_get_frontend_state */

/* "return g_frontend_state;" */

extern undefined4 ui_get_frontend_state(void);


/* frontend_stub_return_0_a */

/* "returns without work (xor eax, eax)" */

extern undefined4 frontend_stub_return_0_a(void);


/* frontend_stub_return_0_b */

/* "returns without work (xor eax, eax)" */

extern undefined4 frontend_stub_return_0_b(void);


/* ui_get_menu_69d1ec */

/* "return g_menu_69d1ec;" */

extern undefined4 ui_get_menu_69d1ec(void);


/* ui_set_menu_69d1ec */

/* "g_menu_69d1ec = param_1; return;" */

extern void ui_set_menu_69d1ec(undefined4 param_1);


/* ui_input_mode_menu */

/* "g_ui_7f01f4 = 0x1000: input goes to the menu" */

extern void ui_input_mode_menu(void);


/* ui_input_reset */

/* "controller of slot 0 = 0 and clears the previous input table" */

extern void ui_input_reset(void);

#define PLAYER_INPUT_COUNT (12)
#define MIN_PLAYERS (2)
#define COLOUR_ID_END (7)
#define MATCH_STATE_DONE (2)
#define INPUT_MODE_GAME_2 (2) /* inferred: a value other than 0x1000 that the doc lists as 1 */


/* menu_validate_player_setup */

/* "sets slot types to 0, counts the usable players and checks for duplicate colours" */

/* Validates the player setup before a race: clears the 4 player slot types, counts the usable
   players, rejects a setup with a duplicate colour, then gives each colour to the next slot.
   Parameters: none. Returns 1 when fewer than MIN_PLAYERS players are usable (inferred: setup
   skipped), 0 when two players share a colour, else 0x1000 once the slots are assigned
   (inferred). The variable usable_count is also reused as a slot or player index in the
   later loops. */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined4 menu_validate_player_setup(void)
{
  bool has_duplicate_colour;
  int *cursor;
  PlayerSlot *slot;
  int colour_id;
  int usable_count;
  int *colour_scan;
  int match_state;
  int slot_index;

  match_state = 0;
  usable_count = 0;
  /* reset the 4 player slot types to 0 (inferred: no player) */
  do {
    race_setup_set_slot_type(usable_count,0);
    usable_count = usable_count + 1;
  } while (usable_count < 4);
  /* start from PLAYER_INPUT_COUNT and subtract 1 for each menu table entry below 1
     (inferred: entry has no player) */
  usable_count = PLAYER_INPUT_COUNT;
  cursor = &g_menu_69d034;
  do {
    colour_id = 1;
    if (cursor[-3] < 1) {
      usable_count = usable_count + -1;
    }
    if (*cursor < 1) {
      usable_count = usable_count + -1;
    }
    if (cursor[3] < 1) {
      usable_count = usable_count + -1;
    }
    if (cursor[6] < 1) {
      usable_count = usable_count + -1;
    }
    if (cursor[9] < 1) {
      usable_count = usable_count + -1;
    }
    if (cursor[0xc] < 1) {
      usable_count = usable_count + -1;
    }
    cursor = cursor + 0x12;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_ui_69d0c4));
  /* fewer than MIN_PLAYERS usable players: stop here */
  if (usable_count < MIN_PLAYERS) {
    return 1;
  }
  /* compare each colour entry with the entries after it; a colour above 0 used twice sets
     has_duplicate_colour (usable_count counts the rows, inferred) */
  has_duplicate_colour = false;
  usable_count = 0;
  cursor = &player_color_id;
  do {
    usable_count = usable_count + 1;
    if (usable_count < PLAYER_INPUT_COUNT) {
      colour_scan = cursor + 3;
      do {
        if ((*cursor == *colour_scan) && (0 < *cursor)) {
          has_duplicate_colour = true;
        }
        colour_scan = colour_scan + 3;
      } while ((LONG_PTR)colour_scan < ((LONG_PTR)&g_menu_update_69d0b8));
    }
    cursor = cursor + 3;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_ui_69d0ac));
  if (has_duplicate_colour) {
    return 0;
  }
  /* clear the controller of every player slot */
  slot = g_player_slots;
  do {
    slot->controller = CONTROLLER_NONE;
    slot = slot + 1;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_ai_editor_mode));
  slot_index = 0;
  cursor = &g_player_slots[0].team;
  /* one pass per slot: for each colour from 1 to COLOUR_ID_END - 1, the first player with that
     colour gets the slot (controller = player index, team = colour - 1, slot type human
     (inferred)). match_state is MATCH_STATE_DONE once every colour has been tried */
  do {
    if (match_state == MATCH_STATE_DONE) {
      g_input_mode = INPUT_MODE_GAME_2;
      return 0x1000;
    }
    match_state = 0;
    do {
      usable_count = 0;
      colour_scan = &player_color_id;
      do {
        if (*colour_scan == colour_id) {
          ((PlayerSlot *)(cursor + -2))->controller = usable_count;
          ((PlayerSlot *)(cursor + -2))->team = colour_id + -1;
          race_setup_set_slot_type(slot_index,1);
          match_state = 1;
        }
        colour_scan = colour_scan + 3;
        usable_count = usable_count + 1;
      } while ((LONG_PTR)colour_scan < ((LONG_PTR)&g_menu_update_69d0b8));
      colour_id = colour_id + 1;
      if (colour_id == COLOUR_ID_END) {
        match_state = MATCH_STATE_DONE;
        break;
      }
    } while (match_state == 0);
    slot_index = slot_index + 1;
    cursor = cursor + 4;
    if (((LONG_PTR)&g_ai_editor_view_origin_x + 0x3) < (LONG_PTR)cursor) {
      g_input_mode = INPUT_MODE_GAME_2;
      return 0x1000;
    }
  } while( true );
}


/* message_show_now */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "opens the message box when none is active" */

void __cdecl
message_show_now(undefined4 text_id,undefined4 code,undefined4 buttons,undefined4 button_text_a,
                undefined4 button_text_b,undefined4 extra)
{
  if (g_message_state == 0) {
    g_message_buttons = buttons;
    g_message_code = code;
    g_message_button_text_a = button_text_a;
    g_message_button_text_b = button_text_b;
    g_message_show_69d004 = extra;
    g_message_text_id = text_id;
    g_message_state = 1;
    *(int *)&g_message_timer = 0;
    g_message_use_text_offset = 0;
    if (g_message_show_69cc38 != 0) {
      g_message_use_text_offset = 1;
      g_message_show_69cc38 = 0;
    }
  }
  return;
}


/* message_show */

/* "(message id, message code, ...) shows a HUD message box or queues it when one is open" */

extern void message_show(char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6);

/* address just past the message box table (the table starts at g_ui_69cbec - 4) */
#define MESSAGE_BOX_TABLE_END (((int)&g_message_box_69cc1c))


/* message_box_active_count */

/* "counts the non-zero slots of the message box table" */

/* Counts the non-zero ints in the message box table (inferred: 12 ints, from g_ui_69cbec - 4 up to
   MESSAGE_BOX_TABLE_END). If any is non-zero, sets g_message_text_id to 0xff (inferred: "no text").
   Takes no parameters and returns nothing. */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void message_box_active_count(void)
{
  int *record;
  int active_count;

  active_count = 0;
  record = &g_ui_69cbec;
  /* each pass checks the 6 ints of one record, record[-1] to record[4]; the records are contiguous,
     so the two passes cover all 12 slots (inferred) */
  do {
    if (record[-1] != 0) {
      active_count = active_count + 1;
    }
    if (*record != 0) {
      active_count = active_count + 1;
    }
    if (record[1] != 0) {
      active_count = active_count + 1;
    }
    if (record[2] != 0) {
      active_count = active_count + 1;
    }
    if (record[3] != 0) {
      active_count = active_count + 1;
    }
    if (record[4] != 0) {
      active_count = active_count + 1;
    }
    record = record + 6;
  } while ((LONG_PTR)record < MESSAGE_BOX_TABLE_END);
  if (active_count != 0) {
    g_message_text_id = 0xff;
  }
  return;
}


/* menu_event_0x11_reset_page */

/* "for the tutorial event resets menu record 0 and the page id" */

extern void menu_event_0x11_reset_page(char *param_1);


/* ui_get_message_state */

/* "return g_message_state;" */

extern undefined4 ui_get_message_state(void);


/* player_slots_count_human_with_controller */

/* "human slots (slot type 1) with a valid controller 0..11" */

extern undefined4 player_slots_count_human_with_controller(void);


/* ui_get_menu_69d1f0 */

/* "return g_menu_69d1f0;" */

extern undefined4 ui_get_menu_69d1f0(void);


/* ui_get_ui_69d1f4 */

/* "return g_ui_69d1f4;" */

extern undefined4 ui_get_ui_69d1f4(void);


/* frontend_set_69d1f4 */

/* "stores param in g_ui_69d1f4" */

extern void __cdecl frontend_set_69d1f4(char *param_1,char *param_2);

#define TEAM_NAMES_OUT_STRING_BASE (0xa7)
#define TEAM_NAME_STRING_BASE (0x7c)
#define TEAM_NAME_SEPARATOR (0x2f)
#define TEAM_NONE (-1)


/* team_names_string_build */

/* WARNING: Removing unreachable block (ram,0x004320aa) */
/* WARNING: Removing unreachable block (ram,0x004320af) */
/* "builds 'TeamA/TeamB' strings for the slots with the given state into dat string 0xA7 + n" */

/* Builds the team name list for the slots whose state equals slot_state and stores it in dat
   string TEAM_NAMES_OUT_STRING_BASE + slot_state (inferred from the string id).
   Up to three matching slots are used, in slot order (inferred): first, second and third team.
   Their names are joined with '/'. Dat strings are length-prefixed: element 0 holds the length
   and the characters start at element 1. Returns nothing. */
/* TIER2: unvalidated 2026-10-16 authority=diffemu */
void team_names_string_build(int slot_state)
{
  short third_len;
  short *out_string;
  short *src;
  int first_team;
  int second_team;
  int second_team_tmp;
  short *dst;
  short total_len;
  int first_team_tmp;
  char matched_count;
  int third_team;

  /* collect the team of each matching slot into first/second/third_team; the shuffles below
     keep them in slot order (inferred) */
  matched_count = g_player_slots[0].state == slot_state;
  third_team = TEAM_NONE;
  second_team = TEAM_NONE;
  if ((bool)matched_count) {
    second_team = g_player_slots[0].team;
  }
  first_team = TEAM_NONE;
  if (g_player_slots[1].state == slot_state) {
    first_team = g_player_slots[1].team;
    if (!(bool)matched_count) {
      first_team = TEAM_NONE;
      second_team = g_player_slots[1].team;
    }
    matched_count = matched_count + '\x01';
  }
  second_team_tmp = first_team;
  first_team_tmp = second_team;
  if (g_player_slots[2].state == slot_state) {
    first_team_tmp = g_player_slots[2].team;
    if (((matched_count != '\0') && (second_team_tmp = g_player_slots[2].team, first_team_tmp = second_team, matched_count != '\x01')) &&
       (second_team_tmp = first_team, matched_count == '\x02')) {
      third_team = g_player_slots[2].team;
    }
    matched_count = matched_count + '\x01';
  }
  second_team = second_team_tmp;
  first_team = first_team_tmp;
  if (((g_player_slots[3].state == slot_state) && (first_team = g_player_slots[3].team, matched_count != '\0')) &&
     ((second_team = g_player_slots[3].team, first_team = first_team_tmp, matched_count != '\x01' &&
      (second_team = second_team_tmp, matched_count == '\x02')))) {
    third_team = g_player_slots[3].team;
  }
  /* look up the output string and the name of the first team */
  out_string = (short *)dat_get_string(slot_state + TEAM_NAMES_OUT_STRING_BASE);
  src = (short *)dat_get_string(first_team + TEAM_NAME_STRING_BASE);
  total_len = *src;
  dst = out_string;
  /* copy the first name's characters (first_team is reused as the loop counter) */
  for (first_team = (int)total_len; dst = dst + 1, first_team != 0; first_team = first_team + -1) {
    src = src + 1;
    *dst = *src;
  }
  /* only one team matched: store the length and stop */
  if (second_team == TEAM_NONE) {
    *out_string = total_len;
    return;
  }
  /* append '/' and the second team's name (second_team is reused as the loop counter) */
  *dst = TEAM_NAME_SEPARATOR;
  src = (short *)dat_get_string(second_team + TEAM_NAME_STRING_BASE);
  total_len = total_len + 1 + *src;
  for (second_team = (int)*src; dst = dst + 1, second_team != 0; second_team = second_team + -1) {
    src = src + 1;
    *dst = *src;
  }
  /* only two teams matched: store the length and stop */
  if (third_team == TEAM_NONE) {
    *out_string = total_len;
    return;
  }
  /* append '/' and the third team's name, then store the total length in element 0 */
  *dst = TEAM_NAME_SEPARATOR;
  src = (short *)dat_get_string(third_team + TEAM_NAME_STRING_BASE);
  third_len = *src;
  for (second_team = (int)third_len; second_team != 0; second_team = second_team + -1) {
    dst = dst + 1;
    src = src + 1;
    *dst = *src;
  }
  *out_string = total_len + 1 + third_len;
  return;
}


extern void ui_format_number_string(char *param_1,char *param_2,char *param_3,char *param_4);


/* ui_get_menu_69cc48 */

/* "return g_menu_69cc48;" */

extern undefined4 ui_get_menu_69cc48(void);


/* menu_state_restore */

/* "copies the saved menu table (0x69CC50) back to 0x69D320 and restores g_menu_current" */

extern void menu_state_restore(void);


/* menu_state_save */

/* "copies the menu table to the saved copy and stores g_menu_current" */

extern void menu_state_save(void);

/* end of g_menu_item_anims: &g_menu_item_anims[30].state (the state field one past the last element) */
#define MENU_ITEM_ANIM_END (((LONG_PTR)&g_hud_8c86bc))


/* menu_table_clear_b */

/* "clears the records at g_menu_item_anims" */

/* Zeroes the item id, state, gap at +0x8, colour, anim counter, scale, x, y, param, ref_a and ref_b of
   every g_menu_item_anims slot (30 slots, 52 bytes each). The gaps at +0x20 and +0x30 are left as they
   are. Takes no parameters and returns nothing. */
/* TIER2: validated 2026-10-18 authority=diffemu */
void menu_table_clear_b(void)
{
  int *anim_state;

  /* walk the slots: anim_state points at the state field of the current slot */
  anim_state = &g_menu_item_anims[0].state;
  do {
    ((MenuItemAnim *)(anim_state + -1))->item_id = 0;
    ((MenuItemAnim *)(anim_state + -1))->state = 0;
    anim_state[1] = 0;
    /* colour (RGBA), cleared byte by byte */
    *(undefined1 *)(anim_state + 2) = 0;
    *(undefined1 *)((char *)anim_state + 9) = 0;
    *(undefined1 *)((char *)anim_state + 10) = 0;
    *(undefined1 *)((char *)anim_state + 0xb) = 0;
    ((MenuItemAnim *)(anim_state + -1))->anim_counter = 0;
    ((MenuItemAnim *)(anim_state + -1))->scale = 0;
    ((MenuItemAnim *)(anim_state + -1))->x = 0;
    ((MenuItemAnim *)(anim_state + -1))->y = 0;
    ((MenuItemAnim *)(anim_state + -1))->param = 0;
    ((MenuItemAnim *)(anim_state + -1))->ref_a = 0;
    ((MenuItemAnim *)(anim_state + -1))->ref_b = 0;
    anim_state = anim_state + 0xd;
  } while ((LONG_PTR)anim_state < MENU_ITEM_ANIM_END);
  return;
}


/* ui_camera_create */

/* "(width, height, ...): RwCamera with frame and raster for the frontend" */

extern RwCamera * ui_camera_create(RwInt32 width,RwInt32 height,char *param_3);

#define RECT_WIDTH_640 (0x44200000)
#define RECT_COLOUR_DIM (0xa0000000)
#define CREDITS_SCROLL_DELAY (15000)


/* credits_update_draw */

/* "advances g_credits_update_69d0fc by the frame ticks while the credits menu page is shown and calls
   credits_draw" */

/* Per frame: while the current menu's item list is the credits page (inferred: the same item list as
   g_menu_page_01_game_type_select), advances g_credits_update_69d0fc by g_frame_ticks and calls
   credits_draw with the timer minus CREDITS_SCROLL_DELAY (inferred: the scroll starts after that
   delay). Otherwise resets the timer to 0. Draws the frontend background bars. Takes no parameters
   and returns nothing. */
/* TIER2: validated 2026-10-18 authority=diffemu */
void credits_update_draw(void)
{
  /* credits page shown: advance the timer and draw the credits text */
  if ((undefined *)g_menu_records[g_menu_current].item_list == &g_menu_page_01_game_type_select) {
    g_credits_update_69d0fc = g_credits_update_69d0fc + g_frame_ticks;
    credits_draw(g_credits_update_69d0fc + -CREDITS_SCROLL_DELAY);
  }
  else {
    g_credits_update_69d0fc = 0;
  }
  /* two full-width background panels (inferred), then two white top/bottom bars (inferred) */
  gfx_draw_rect_2d_flat_a(0,0,RECT_WIDTH_640,0x42800000,RECT_COLOUR_DIM);
  gfx_draw_rect_2d_flat_b(0,0x43d00000,RECT_WIDTH_640,0x42800000,RECT_COLOUR_DIM);
  draw_rect(0,0x43d00000,RECT_WIDTH_640,0x3f800000,COLOUR_WHITE_ARGB);
  draw_rect(0,0x42800000,RECT_WIDTH_640,0x3f800000,COLOUR_WHITE_ARGB);
  /* career row past 11 (inferred: later career progress): four more coloured bars */
  if (0xb < (int)g_career_row) {      /* diffemu: undefined4 global, the original compares signed (jle) */
    draw_rect(0,0x42800000,RECT_WIDTH_640,0x40400000,0x80f7941d);
    draw_rect(0,0x42860000,RECT_WIDTH_640,0x40400000,0x40f7941d);
    draw_rect(0,0x43ce8000,RECT_WIDTH_640,0x40400000,0x80f7941d);
    draw_rect(0,0x43cd0000,RECT_WIDTH_640,0x40400000,0x40f7941d);
  }
  return;
}


