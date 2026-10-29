// cluster-entries: race_start_setup gap_411f40
/* Installed cluster race_start_setup (tools/cluster_install.py from output/cluster_pilot_uc3/race_start_setup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*00416af0*/ extern int ai_bot_skill();
/*00416d40*/ extern int ai_bots_assign_slots();
/*00416ca0*/ extern int ai_bots_randomise();
/*0044ba50*/ extern int camera_records_clear();
/*00423f40*/ extern int car_damage_events_reset();
/*0042b0b0*/ extern int course_get_split_sector_8();
/*004747b0*/ extern int course_objects_reset_transforms();
/*00405020*/ extern int course_powerups_setup();
/*004254a0*/ extern int debris_update_visible_all_vehicles();
/*00425710*/ extern int debris_update_visible_all_vehicles_b();
/*00487bc0*/ extern int effect_pool_clear();
/*0040d2e0*/ extern int effects_reset();
/*004207b0*/ extern int endpoint_panel_reset();
/*004a03b0*/ extern int fx_clump_slots_clear();
/*00408810*/ extern int fx_objects_reset();
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b14*/ extern int g_race_setup;
/*004809f0*/ extern int gfx_groups_reset();
/*0047fe10*/ extern int hud_labels_clear();
/*004208a0*/ extern int hudpanel_endpoint_659a78_clear();
/*0041b970*/ extern int hudpanel_reset_all();
/*0041fc00*/ extern int kc_panel_init();
/*0042fcb0*/ extern int menu_reset_scroll();
/*00485610*/ extern int particle_pool_counts_reset();
/*00486160*/ extern int particle_templates_init_b();
/*00408ec0*/ extern int player_records_clear_all();
/*00426630*/ extern int player_stats_clear_a();
/*00464a20*/ extern int powerups_reset_all();
/*0043a160*/ extern int race_end_check_points();
/*00412d10*/ extern int race_phase_9_update();
/*004103d0*/ extern int race_spawn_cars();
/*0040f800*/ extern int replay_save_thunk_40f800();
/*0040c710*/ extern int round_setup_limits();
/*00492e00*/ extern int rwp_objects_reset_all();
/*004807c0*/ extern int rwp_trigger_group_advance_and_fire();
/*0041e590*/ extern int score_panel_clear();
/*0041e550*/ extern int score_panel_init();
/*00487620*/ extern int sprite_slots_clear();
/*004b2ccd*/ extern int __cdecl start(int p0, int p1, int p2, int p3, int p4);
/*0041f090*/ extern int team_colour_table_init_2();
/*0041fc40*/ extern int kc_panel_origins_init();
/*0041efa0*/ extern int team_panel_reset();
/*004967f0*/ extern int triggers_assign_ids();
/*00429b60*/ extern int vehicle_slots_clear_flags();
/*004653b0*/ extern int weapon_slots_6a9424_clear();
/*00467d50*/ extern int weapons_race_start_dispatch();
/*00467860*/ extern int weapons_reset_all_dispatch();
/*00467d90*/ extern int weapons_unequip_all_a();
/* DECLS-END */
extern int race_phase_4_update(int param_1);
extern int race_phase_6_update(int param_1);
extern int race_phase_7_update(void);
extern int race_set_phase(int phase);
extern int race_set_view_player(int player);
extern int race_update_cameras(int arg);
extern int race_setup_set_slot_type(int slot, int type);
extern int vehicle_get_matrix(int slot, float *pose);   /* writes 6 floats (24 bytes): the position and three more words */
extern int car_set_bounds(int slot, float *pose);   /* reads the position (first 3 floats) */
extern int vehicle_set_active(int slot, int active);
extern int player_status_set(int slot, int status);
extern int car_deactivate(int slot);
extern int car_init_dynamics(int slot);
extern int hudpanel_slot_init_player_b(int a, int b);
extern int cup_find_entry(int course);
extern int game_set_game_659d00(int v);
extern int nop_stub(int arg);
extern int rw_stream_chunk_copy(int *stream, int *out);
extern int store_u32(int *stream, int value);
extern int course_set_split_sector_6(int v);
extern int course_set_split_sector_7(int v);
extern int FUN_005543d0(int *stream);
extern int course_hook6(int a);
extern int course_hook7(int a);
extern int course_hook8(int a);
extern int race_get_setup_index(void);
extern int get_event_type(void);
extern int is_team_mode(void);
extern int current_course_id(void);
extern int get_player_count(void);
extern int g_race_phase_arg;   /* the message stream the phase driver polls */
extern int g_race_phase, g_phase_timer, g_tick_number, g_race_clock_a, g_race_clock_b, g_time_limit_ticks, game_mode, player_count, g_cars_7ef7c0, g_race_7ef7fc, g_race_652f5c, g_camera_shake, g_intro_652df0;
extern float g_game_652e00, g_tick_seconds, k_f_2;

extern void gap_411f40(void);
static void race_phase_2_setup(void);

static int race_phase_3_update_r(int p0);
extern void race_phase_3_update(void);
static int race_phase_5_update_r(int p0);
extern void race_phase_5_update(void);
static int race_update_common_r(int p0);
extern void race_update_common(void);
/* The setup of phase 1 (0x411f40: a block of race_start_setup that the compiler placed before it and reaches with a jump). */
void gap_411f40(void)
{
  int setup;
  int slot;
  int offset;
  float pose[6];

  if (get_event_type() == 0x12) {
    race_setup_set_slot_type(0, 2);
    race_setup_set_slot_type(1, 2);
    race_setup_set_slot_type(2, 2);
    race_setup_set_slot_type(3, 0);
  }
  g_race_652f5c = 0;
  setup = race_get_setup_index();
  course_hook7(**(int **)(g_race_setup + 4 + setup * 4));
  g_phase_timer = 0xff;
  player_stats_clear_a();
  g_tick_number = 0;
  g_race_clock_a = 0;
  g_race_clock_b = 0;
  FID_conflict__wprintf("start of race\n");
  camera_records_clear();
  vehicle_slots_clear_flags();
  weapon_slots_6a9424_clear();
  course_objects_reset_transforms();
  player_count = 0;
  slot = 0;
  offset = 0xb8;
  do {
    if (*(int *)(offset + g_race_setup) == 0) {
      vehicle_set_active(slot, 0);
      car_deactivate(slot);
      player_status_set(slot, 4);
    }
    else {
      vehicle_get_matrix(slot, pose);
      car_set_bounds(slot, pose);
      car_init_dynamics(slot);
      vehicle_set_active(slot, 1);
      player_status_set(slot, 1);
      player_count = player_count + 1;
    }
    offset = offset + 4;
    slot = slot + 1;
  } while (offset < 200);
  round_setup_limits();
  effects_reset();
  nop_stub(0);
  ai_bot_skill();
  triggers_assign_ids();
  nop_stub(1);
  effect_pool_clear();
  particle_pool_counts_reset();
  weapons_race_start_dispatch();
  particle_templates_init_b();
  sprite_slots_clear();
  hudpanel_reset_all();
  car_damage_events_reset();
  debris_update_visible_all_vehicles_b();
  switch (get_event_type()) {
  case 0:
  case 1:
  case 2:
    game_set_game_659d00(1);
    break;
  case 3:
  case 4:
  case 5:
  case 0xe:
  case 0xf:
  case 0x10:
    switch (game_mode) {
    case 5:
      break;
    default:
      if (is_team_mode() == 0) {
        score_panel_init();
        kc_panel_init();
      }
      else {
        team_panel_reset();
        endpoint_panel_reset();
      }
      break;
    case 7:
      hudpanel_slot_init_player_b(0, 1);
      cup_find_entry(current_course_id());
      break;
    case 10:
      course_powerups_setup();
    case 4:
    case 8:
    case 9:
      cup_find_entry(current_course_id());
    }
    /* fall through */
  default:
    if (is_team_mode() != 0) {
      team_panel_reset();
      endpoint_panel_reset();
    }
    else {
      score_panel_init();
      kc_panel_init();
    }
    break;
  case 6:
  case 7:
  case 8:
  case 0xb:
  case 0xc:
  case 0xd:
    if (is_team_mode() != 0 && game_mode != 2) {
      team_panel_reset();
      endpoint_panel_reset();
    }
    else {
      score_panel_init();
      kc_panel_init();
    }
    break;
  }
  powerups_reset_all();
  player_records_clear_all();
  gfx_groups_reset();
  g_phase_timer = 100000;
  race_set_phase(2);
}

/* Phase 2: the cars are placed, the event panels are cleared by event type and game mode, and the phase timer decides the next phase. */
static void race_phase_2_setup(void)
{
  int setup;

  rwp_objects_reset_all();
  rwp_trigger_group_advance_and_fire();
  menu_reset_scroll();
  if (game_mode == 2) {
    g_cars_7ef7c0 = g_cars_7ef7c0 + 1;
    if (g_cars_7ef7c0 < 0 || get_player_count() <= g_cars_7ef7c0) {
      g_cars_7ef7c0 = 0;
    }
  }
  race_spawn_cars();
  g_race_7ef7fc = g_race_clock_a > g_time_limit_ticks;
  setup = race_get_setup_index();
  course_hook6(**(int **)(g_race_setup + 4 + setup * 4));
  effects_reset();
  weapons_reset_all_dispatch();
  particle_templates_init_b();
  sprite_slots_clear();
  hudpanel_reset_all();
  car_damage_events_reset();
  debris_update_visible_all_vehicles();
  effect_pool_clear();
  switch (get_event_type()) {
  case 0:
  case 1:
  case 2:
    break;
  case 3:
  case 4:
  case 5:
  case 0xe:
  case 0xf:
  case 0x10:
    switch (game_mode) {
    case 4:
    case 5:
    case 8:
    case 9:
      break;
    default:
      if (is_team_mode() == 0) {
        score_panel_clear();
        kc_panel_origins_init();
      }
      else {
        team_colour_table_init_2();
        hudpanel_endpoint_659a78_clear();
      }
      break;
    case 7:
      hudpanel_slot_init_player_b(0, 1);
      break;
    case 10:
      course_powerups_setup();
    }
    /* fall through */
  default:
    if (is_team_mode() != 0) {
      team_colour_table_init_2();
      hudpanel_endpoint_659a78_clear();
    }
    else {
      score_panel_clear();
      kc_panel_origins_init();
    }
    break;
  case 6:
  case 7:
  case 8:
  case 0xb:
  case 0xc:
  case 0xd:
    if (is_team_mode() != 0 && game_mode != 2) {
      team_colour_table_init_2();
      hudpanel_endpoint_659a78_clear();
    }
    else {
      score_panel_clear();
      kc_panel_origins_init();
    }
    break;
  }
  ai_bots_randomise();
  fx_clump_slots_clear();
  fx_objects_reset();
  course_objects_reset_transforms();
  if (g_phase_timer == 100000) {
    race_set_phase(3);
  }
  else {
    if (get_event_type() == 0x11) {
      race_set_phase(6);
    }
    else {
      race_set_phase(4);
      g_phase_timer = 12000;
    }
    race_update_cameras(1);
  }
  if (game_mode == 6) {
    race_set_phase(4);
  }
}

/* Race phase driver (0x4136d0), called once per tick: `param_1` is handed to the per-phase update.  Phase 1 sets up the race, phase 2 spawns the cars, 3..7 run the
   countdown / race / finish updates, 8/9 the end of race, 10 replays, 0xb waits for a key press. */
void race_start_setup(int param_1)
{
  int value;
  int msg;
  int setup;
  int ctrl;
  PlayerSlot *slot;
  double timer;

  g_tick_number = g_tick_number + 1;
  if (course_get_split_sector_8() == 0) {
    return;
  }
  msg = FUN_005543d0(&g_race_phase_arg);
  if (msg == 5) {
    rw_stream_chunk_copy(&g_race_phase_arg, &value);
    store_u32(&g_race_phase_arg, 0);
    course_set_split_sector_6(value);
  }
  else if (msg == 6) {
    rw_stream_chunk_copy(&g_race_phase_arg, &value);
    store_u32(&g_race_phase_arg, 0);
    course_set_split_sector_7(value);
  }
  switch (g_race_phase) {
  case 1:
    g_camera_shake = 0;
    gap_411f40();
    return;
  case 2:
    g_camera_shake = 0;
    race_phase_2_setup();
    return;
  case 3:
    g_camera_shake = 0;
    race_phase_3_update_r(param_1);
    race_update_common_r(param_1);
    return;
  case 4:
    g_camera_shake = 0;
    race_phase_4_update(param_1);
    race_update_common_r(param_1);
    return;
  case 5:
    race_phase_5_update_r(param_1);
    race_update_common_r(param_1);
    return;
  case 6:
    race_phase_6_update(param_1);
    race_update_common_r(param_1);
    return;
  case 7:
    race_phase_7_update();
    race_update_common_r(param_1);
    weapons_unequip_all_a();
    return;
  case 8:
    if (get_event_type() == 0x11) {
      race_end_check_points();
    }
    ai_bots_assign_slots();
    setup = race_get_setup_index();
    course_hook8(**(int **)(g_race_setup + 4 + setup * 4));
    weapons_unequip_all_a();
    hud_labels_clear();
    race_set_phase(2);
    return;
  case 9:
    if (get_event_type() == 0x11) {
      round_setup_limits();
    }
    race_phase_9_update();
    race_set_phase(0xa);
    return;
  case 10:
    replay_save_thunk_40f800();
    race_update_common_r(param_1);
    return;
  case 0xb:
    race_update_common_r(param_1);
    timer = g_game_652e00 + g_tick_seconds;
    g_game_652e00 = (float)timer;
    if (!(timer < k_f_2)) {
      slot = g_player_slots;
      do {
        ctrl = slot->controller;
        if (ctrl != -1) {
          if ((g_player_inputs[ctrl].menu_select != 0 && g_player_inputs_previous[ctrl].menu_select == 0) ||
              (g_player_inputs[ctrl].pause != 0 && g_player_inputs_previous[ctrl].pause == 0)) {
            race_set_phase(9);
            race_set_view_player(1);
            g_intro_652df0 = 0;
          }
        }
        slot = slot + 1;
      } while (slot < &g_player_slots[4]);
    }
    return;
  default:
    return;
  }
}

/* race_phase_3_update_r: C -> original race_phase_3_update (registers ebx) */
__declspec(naked) int race_phase_3_update_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call race_phase_3_update
        pop ebx
        ret
    }
}

/* race_phase_5_update_r: C -> original race_phase_5_update (registers eax) */
__declspec(naked) int race_phase_5_update_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call race_phase_5_update
        ret
    }
}

/* race_update_common_r: C -> original race_update_common (registers esi) */
__declspec(naked) int race_update_common_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call race_update_common
        pop esi
        ret
    }
}
