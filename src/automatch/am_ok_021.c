#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*0042bd00*/ extern int dat_get_string();
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

extern void menu_selection_step(int param_1);


/* menu_current_item_offset */

/* "computes the item list offset of the selected entry of the current menu" */

extern undefined4 menu_current_item_offset(void);


/* menu_select_pressed_any */

/* "edge detect of menu_select on any of the 12 player inputs (blocked while a message is closing)"
    */

extern undefined4 __cdecl menu_select_pressed_any(undefined4 param_1);


/* menu_back_pressed_any */

/* "edge detect of menu_back on any player input" */

extern undefined4 menu_back_pressed_any(void);


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


/* menu_validate_player_setup */

/* "sets slot types to 0, counts the usable players and checks for duplicate colours" */

extern undefined4 menu_validate_player_setup(void);


/* message_show_now */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "opens the message box when none is active" */

extern void __cdecl message_show_now(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, undefined4 param_5,undefined4 param_6);


/* message_show */

/* "(message id, message code, ...) shows a HUD message box or queues it when one is open" */

void message_show(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)
{
  if (g_message_state == 0) {
    message_show_now(param_1,param_2,param_3,param_4,param_5,param_6);
    return;
  }
  g_message_69caf4 = param_2;
  g_message_69cafc = param_4;
  g_message_69cb00 = param_5;
  g_message_69cb04 = param_6;
  g_message_69caf8 = param_3;
  g_message_69caf0 = param_1;
  g_message_69cb08 = 1;
  return;
}


/* message_box_active_count */

/* "counts the non-zero slots of the message box table" */

extern void message_box_active_count(void);


/* menu_event_0x11_reset_page */

/* "for the tutorial event resets menu record 0 and the page id" */

extern void menu_event_0x11_reset_page(int param_1);


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

extern void __cdecl frontend_set_69d1f4(undefined4 param_1,undefined4 param_2);


/* team_names_string_build */

/* WARNING: Removing unreachable block (ram,0x004320aa) */
/* WARNING: Removing unreachable block (ram,0x004320af) */
/* "builds 'TeamA/TeamB' strings for the slots with the given state into dat string 0xA7 + n" */

extern void team_names_string_build(int param_1);


/* ui_format_number_string */

/* "formats a number below 10 with one format string, else with another" */

extern void ui_format_number_string(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);


/* ui_compare_sign */

/* "(a, b, out): out = 0 / 2 / 1 for a == b / a < b / a > b" */

extern void ui_compare_sign(int param_1,int param_2,undefined4 *param_3,int *param_4,int *param_5, float *param_6);


/* ui_get_menu_69cc48 */

/* "return g_menu_69cc48;" */

extern undefined4 ui_get_menu_69cc48(void);


/* menu_state_restore */

/* "copies the saved menu table (0x69CC50) back to 0x69D320 and restores g_menu_current" */

extern void menu_state_restore(void);


/* menu_state_save */

/* "copies the menu table to the saved copy and stores g_menu_current" */

extern void menu_state_save(void);


/* menu_table_clear_b */

/* "clears the records at g_menu_8c80a0" */

extern void menu_table_clear_b(void);


/* ui_camera_create */

/* "(width, height, ...): RwCamera with frame and raster for the frontend" */

extern RwCamera * ui_camera_create(RwInt32 width,RwInt32 height,int param_3);


/* credits_update_draw */

/* "advances g_hud_69d0fc by the frame ticks while the credits menu page is shown and calls
   credits_draw" */

extern void credits_update_draw(void);



