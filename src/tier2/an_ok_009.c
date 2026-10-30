#include "mfl_tier2.h"
extern unsigned char g_ai_cells[];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff70*/ extern int race_get_last_active_player();
/*004131d0*/ extern int game_mode_end_check();
/*004162f0*/ extern int trail_set_param();
/*004166d0*/ extern float __cdecl noise_interp_cos(float p0, float p1, float p2);      /* diffemu: float arguments (were undefined4: the compiler converted them with _ftol), result in ST0 */
/*00416700*/ extern float __cdecl noise_sample_row(undefined4 p0, float p1);      /* diffemu: was `()`: the float argument was pushed as a double */
/*00416e20*/ extern int point_project_onto_line_2d(undefined4 p0);
/*00417b70*/ extern int __cdecl ai_find_leader_a(undefined4 p0);
/*0041ba10*/ extern int hudpanel_slot_reset(undefined4 p0);
/*0041be10*/ extern int hudpanel_slots_clear();
/*0041be30*/ extern int hudpanel_values_init();
/*0041bf20*/ extern int marker_particles_destroy();
/*0041c0d0*/ extern int hudpanel_counters_clear(undefined4 p0);
/*0041c510*/ extern int hudpanel_markers_render_all();
/*0041d9e0*/ extern int btb_panel_render_all();
/*004220f0*/ extern int vehicle_get_position_158();
/*00422150*/ extern int vehicle_get_flags_10();
/*00422190*/ extern int vehicle_get_flag_400();
/*00422280*/ extern int vehicle_part_frame();
/*004222a0*/ extern int vehicle_part_matrix();
/*00427d20*/ extern int game_get_game_8c8ae0();
/*00434bb0*/ extern int get_event_type();
/*004353e0*/ extern int menu_markers_enabled_for_event();
/*0044c140*/ extern int __cdecl player_distance_warning(undefined4 p0);
/*0044c180*/ extern double player_distance_get();
/*0044d1f0*/ extern undefined ai_grid_cell_lookup_0044d1f0(undefined4 p0);
/*0044d2a0*/ extern int ai_nearest_route_point();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047b480*/ extern double car_probe_ground_distance();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*00481fd0*/ extern double random_range();
/*00482070*/ extern int dot_product_3d_clamped();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004955c0*/ extern int ghost_draw_markers();
/*004967d0*/ extern int triggers_get_array();
/*0049cb60*/ extern int sphere_vs_box_test(undefined4 p0, float *p1, float *p2);
/*004b2b48*/ extern int _ftol();
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00544e00*/ extern undefined __cdecl RpRandomSeed(undefined4 p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd36c*/ extern float k_f_0_4;
/*005dd370*/ extern float k_f_0_3;
/*005dd620*/ extern float float_10_0;
/*005dd628*/ extern float k_f_0_25;
/*005dd62c*/ extern float k_f_100;
/*005dd634*/ extern float k_f_2;
/*005dda24*/ extern float k_f_0_75;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb48*/ extern float k_f_16;
/*005ddb50*/ extern float k_f_8;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b0*/ extern float k_f_n0_05;
/*005de1b4*/ extern float k_f_0_0625;
/*005de1b8*/ extern float k_f_31;
/*005de218*/ extern float k_f_0_866;
/*005de230*/ extern float k_f_n0_75;
/*005de234*/ extern float k_f_0_875;
/*0060392c*/ extern float g_hud_player_60392c;
/*00603950*/ extern undefined g_ai_bots_603950;
/*00603954*/ extern undefined g_ai_bots_603954;
/*00603a84*/ extern undefined g_ai_bots_603a84;
/*006089b0*/ extern undefined4 g_ai_bots_6089b0;
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652fa0*/ extern undefined4 g_ghost_render_652fa0;
/*00652fb0*/ extern undefined g_trail_652fb0;
/*00652fb4*/ extern undefined g_trail_652fb4;
/*00652fb8*/ extern undefined g_trail_652fb8;
/*0065314c*/ extern undefined4 g_car_65314c;
/*0065315c*/ extern undefined4 g_trail_65315c;
/*00653160*/ extern undefined4 g_hud_653160;
/*00653164*/ extern undefined4 g_hud_player_653164;
/*00653258*/ extern undefined g_trail_653258;
/*00653298*/ extern undefined4 g_noise_table_653298;
/*00658300*/ extern undefined4 g_load_658300;
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658490*/ extern undefined4 g_particles_658490;
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538;
/*00658b28*/ extern undefined4 g_load_658b28;
/*00658b2c*/ extern undefined4 g_load_658b2c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef804*/ extern undefined4 g_tick_number;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0284*/ extern struct AiData g_ai_data;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
/*008c9980*/ extern float g_ai_skill;
/*008c9984*/ extern undefined4 g_ai_last_active_player;
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c998c*/ extern undefined4 g_car_8c998c;
/*008c9990*/ extern float g_car_8c9990;
/*008c9998*/ extern undefined4 g_ai_counter_9998;
/*008c999c*/ extern float g_ai_skill_bias;
/*008c99a0*/ extern float g_ai_skill_score;
/*008c99a4*/ extern undefined4 g_ai_career_skill_table;
/*008c9a40*/ extern undefined4 g_ai_career_counter_table;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9ec0*/ extern undefined4 g_ai_player_counters;
/*008c9ee0*/ extern float g_noise_table_8c9ee0;
/*008c9ee4*/ extern float g_noise_table_8c9ee4;
/*008c9ee8*/ extern float g_noise_table_8c9ee8;
/*008c9eec*/ extern float g_noise_table_8c9eec;
/*008c9ef0*/ extern undefined4 g_noise_table_8c9ef0;
/*008c9f00*/ extern undefined g_hud_player_8c9f00;
/*008cb500*/ extern undefined g_hud_8cb500;
/*008ccb02*/ extern undefined2 g_trail_indices_8ccb02;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
/* ghost_render */

/* "event types 0..2: renders the ghost car with additive blending" */

extern void ghost_render(void);


/* ghost_update */

/* "event types 0..2: updates the ghost from the best replay (inferred)" */

extern void ghost_update(void);


/* trail_get_triple */

/* "(a, b, out xyz): three floats of the record at 0x652FB0 (stride 0xC)" */

extern void __cdecl trail_get_triple(char *param_1,char *param_2,undefined4 *param_3);


/* trail_indices_init */

/* "fills the short index table at 0x8CCB00 (4 vertices per quad)" */

extern void trail_indices_init(void);


/* hud_player_marker_position_calc_with_facing */

/* "as hud_player_marker_position_calc but also computes the facing dot product" */

extern void __cdecl hud_player_marker_position_calc_with_facing (float *param_1,char *param_2,char *param_3,char *param_4,float param_5,float param_6, undefined1 *param_7,int *param_8,char *param_9,char *param_10, char *param_11);


/* hud_player_marker_position_calc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "world position between two cars scaled by param_5, distance*10, fills one marker entry (limit
   0x7f)" */

extern void __cdecl hud_player_marker_position_calc (float *param_1,char *param_2,char *param_3,char *param_4,float param_5,float param_6, undefined1 *param_7,int *param_8,char *param_9,char *param_10, char *param_11);


/* hud_player_markers_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per player distance to the camera and flashing (tick & 0x1F) used for the markers above the cars
   (inferred)" */

extern void hud_player_markers_update(char *param_1);


/* trail_particles_create */

/* "particles_create(0x653258, 0x37, 0x14, texture) with blend modes 5 / 6" */

extern void trail_particles_create(void);


/* trail_particles_destroy */

/* "particles_destroy(0x653258)" */

extern void trail_particles_destroy(void);


/* noise_table_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RpRandomSeed(0xD7A33) then fills 4096 random floats at 0x653298" */

extern void noise_table_init(void);


/* noise_value */

/* "2D value noise from the table with cosine interpolation"
   row: table row to sample (inferred from noise_sample_row); x: position along the row.
   Samples the two integer cells around x and returns their cosine-interpolated value.
   Note: x is overwritten with its fractional part (0..1) before the interpolation. */

/* TIER2: validated 2026-10-18 authority=diffemu */
float noise_value(undefined4 row,float x)
{
  int x_cell;
  float value_left;
  float value_right;

  x_cell = (int)x;      /* diffemu: was `_ftol()` without its ST0 argument */
  x = x - (float)x_cell;
  value_right = noise_sample_row(row,(float)(x_cell + 1));
  value_left = noise_sample_row(row,(float)x_cell);
  return noise_interp_cos(value_left,value_right,x);
}


/* ai_bots_reset */

/* "clears the bot_state of g_ai_data and the per bot counters at 0x8C9B10" */

extern void ai_bots_reset(void);


/* ai_bot_state_reset */

/* "(bot or -1 for all): sets the bot state bytes to defaults (0x3E8 ...)" */

extern void __cdecl ai_bot_state_reset(char *param_1);


/* ai_bots_randomise */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "random start values for the bots from the table at 0x603950" */

extern void ai_bots_randomise(void);


/* ai_bots_assign_slots */

/* "assigns bots to player slots for the active race setup (inferred)" */

extern void ai_bots_assign_slots(void);


/* ai_bot_weapon_logic */

/* "(bot): decides about picking / using a weapon unless the race phase is 10 or 0xB (inferred)" */

/* (inferred) returns 1 and copies the target xz to out_target_xz when the bot has no weapon (descriptor 0), is not in training or race phase 10 / 0xb and the target lies within about 30 degrees (dot > 0.866) of its forward direction */
undefined4 __cdecl
ai_bot_weapon_logic(int bot,float *bot_pos_xz,char *unused_3,float *target_pos_xz,char *unused_5,
                   float *out_target_xz)
{
  int phase_or_event;
  RwV3d to_target;
  RwV3d forward;
  
  phase_or_event = race_get_phase();
  if (phase_or_event != 0xb) {
    phase_or_event = race_get_phase();
    if (phase_or_event != 10) {
      if (player_weapon_slots[bot].descriptor == 0) {
        phase_or_event = get_event_type();
        if (phase_or_event != EVENT_TRAINING) {
          car_get_forward(&forward,bot);
          to_target.x = *target_pos_xz - *bot_pos_xz;
          forward.y = 0.0;
          to_target.y = 0.0;
          to_target.z = target_pos_xz[1] - bot_pos_xz[1];
          RwV3dNormalize(&forward,&forward);
          RwV3dNormalize(&to_target,&to_target);
          if (k_f_0_866 < forward.z * to_target.z + to_target.y * forward.y + forward.x * to_target.x)
          {
            *out_target_xz = *target_pos_xz;
            out_target_xz[1] = target_pos_xz[1];
            return 1;
          }
        }
      }
      return 0;
    }
  }
  return 0;
}


/* ai_trigger_zone_target_for_car */

/* "scans the trigger array for type 7 boxes the car is in and projects a target onto the line" */

extern undefined4 __cdecl ai_trigger_zone_target_for_car (char *param_1,char *param_2,char *param_3,char *param_4);


/* ai_cell_is_value2 */

/* "danger data cell value == 2 at a world position" */

extern undefined4 __cdecl ai_cell_is_value2(float *param_1,char *param_2);


/* ai_find_leader_b */

/* "variant of ai_find_leader_a" */

extern undefined4 __cdecl ai_find_leader_b(char *param_1,float param_2,float param_3);


/* ai_player0_distance_is_zero */

/* "player_distance_get(0) == 0" */

extern undefined4 ai_player0_distance_is_zero(void);


/* ai_bot_obstacle_ahead */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: tests the point 2 forward vectors ahead of the car" */

extern undefined4 __cdecl ai_bot_obstacle_ahead(char *param_1);


/* ai_target_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: validates a candidate target (position, target)" */

extern undefined4 __cdecl ai_target_check(float *param_1,float *param_2);


/* ai_route_target_for_car */

/* "(bot, out, car): ai_nearest_route_point from the car position with the per bot hint" */

extern void ai_route_target_for_car(char *param_1,char *param_2,char *param_3);
#undef local_c
#undef local_8
#undef local_4


/* ai_get_float_8c9980 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "float at 0x8C9980" */

extern double ai_get_float_8c9980(void);

/* ai_erase_cell grid: 8 world units per cell, 128 x 128 cells, world origin at -512 */
#define AI_GRID_DIM (0x80)
#define AI_WORLD_HALF (0x200)
#define AI_CELL_SHIFT (3)
#define AI_CELL_MASK (7)
#define AI_SIGN_SHIFT (0x1f)
/* tile: 8 x 8 bytes, 0xFF = empty */
#define AI_TILE_BYTES (0x40)
#define AI_EMPTY (-1)
#define AI_EMPTY_BYTE (0xff)


/* ai_erase_cell */

/* "(x, z): clears a cell and frees the tile when it became empty" */

/* Erases the danger byte of the 8 x 8 unit cell at (world_x, world_z) in the AI grid.
   The byte is set to AI_EMPTY_BYTE (0xFF). If no byte of the cell's tile is left set,
   the grid entry is reset to AI_EMPTY so the tile can be reused. Returns 1 when the
   position is inside the grid (also when it has no tile), 0 when it is outside. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined4 __cdecl ai_erase_cell(uint world_x,uint world_z)
{
  short tile_index;
  bool tile_has_data;
  int cell_z;
  int cell_x;
  char *tile_offset;

  /* world position -> cell column: (x + 512) / 8, rounded toward zero */
  cell_x = (int)(world_x + AI_WORLD_HALF + ((int)(world_x + AI_WORLD_HALF) >> AI_SIGN_SHIFT & 7U)) >> AI_CELL_SHIFT;
  /* positions outside the 128 x 128 grid are rejected */
  if ((-1 < cell_x) && (cell_x < AI_GRID_DIM)) {
    /* same conversion for the z axis (cell row) */
    cell_z = (int)(world_z + AI_WORLD_HALF + ((int)(world_z + AI_WORLD_HALF) >> AI_SIGN_SHIFT & 7U)) >> AI_CELL_SHIFT;
    if ((-1 < cell_z) && (cell_z < AI_GRID_DIM)) {
      /* grid index = column * 128 + row; cell_z is reused for it from here on */
      cell_z = cell_x * AI_GRID_DIM + cell_z;
      tile_index = g_ai_data.grid[cell_z];
      /* tile index 0 or less (inferred: 0 is reserved) means the cell has no tile, nothing to erase */
      if (tile_index < 1) {
        return 1;
      }
      /* clear this position's byte: inside the tile, byte (z & 7) * 8 + (x & 7) */
      *(undefined1 *)((world_x & AI_CELL_MASK) + (int)g_ai_cells + ((world_z & AI_CELL_MASK) + tile_index * 8) * 8) = AI_EMPTY_BYTE;
      /* scan the whole tile, 8 rows of 8 bytes: any byte that is not empty keeps the tile */
      tile_has_data = false;
      cell_x = 0;
      tile_offset = tile_index * AI_TILE_BYTES;
      do {
        if (*(char *)(tile_offset + (int)g_ai_cells + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 1 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 2 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 3 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 4 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 5 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 6 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        if (*(char *)(tile_offset + (int)g_ai_cells + 7 + cell_x) != AI_EMPTY) {
          tile_has_data = true;
        }
        cell_x = cell_x + 8;
      } while (cell_x < AI_TILE_BYTES);
      /* tile fully empty: free the grid entry */
      if (!tile_has_data) {
        g_ai_data.grid[cell_z] = AI_EMPTY;
      }
      return 1;
    }
  }
  return 0;
}


/* ai_get_counter */

/* "float at 0x8C9EC0 + index * 4 (the per player countdown used by game_mode_end_check)" */

extern double ai_get_counter(char *param_1);


/* ai_ftol_noop_wrapper */

/* "just calls _ftol" */

extern void ai_ftol_noop_wrapper(void);


/* race_ai_slot_assign_or_adjust */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "for non-career events adjusts g_ai_8c999c by +-0.5 or calls ai_bots_assign_slots" */

extern void __fastcall race_ai_slot_assign_or_adjust(char *param_1);


/* hudpanel_reset_all */

/* "resets every 0x6C byte player marker slot, the panel slots and the value tables" */

extern void hudpanel_reset_all(void);


/* marker_particles_update_b */

/* "race phase 6: FUN_0041c510 when markers enabled" */

extern void marker_particles_update_b(void);


/* hudpanel_clumps_destroy */

/* "RpClumpDestroy for the marker clumps from 0x658490 (0x54 byte stride) and the 0x658300 slots" */

extern void hudpanel_clumps_destroy(void);


