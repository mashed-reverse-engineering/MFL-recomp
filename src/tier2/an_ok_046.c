#include "mfl_tier2.h"
/*00408ea0*/ extern int player_record_get_8d8ca0();
/*0040fbf0*/ extern int race_get_focus_player();
/*0042a3f0*/ extern int course_get_rpworld();
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*00464060*/ extern int weapon_effects_clear_6a8af8();
/*00464330*/ extern int powerup_icon_texture();
/*00464c30*/ extern int powerup_place();
/*00464dd0*/ extern int powerup_set_respawn_time();
/*00472890*/ extern int sfx_play_voice_at();
/*00472bb0*/ extern int sfx_voice_stop();
/*0047a5b0*/ extern int car_add_velocity();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00481fd0*/ extern double random_range();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, int p1);      /* diffemu: the original takes (clump, flag), the flag is read from the stack slot */
/*00484680*/ extern int object_set_flag_4();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*0049bd90*/ extern int fx_system_destroy();
/*0049bdd0*/ extern int fx_system_render();
/*0049ff00*/ extern undefined __cdecl particle_emit_textured(float *p0, undefined4 *p1, int p2, int p3, struct RwTexture *p4, float p5, float *p6);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4e40*/ extern RwBool RwMatrixDestroy(RwMatrix *p0);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd374*/ extern float k_f_0_2;
/*005dd7f8*/ extern float k_f_110;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005e64a4*/ extern float k_f_0;
/*005de294*/ extern float k_f_n0_125;
/*005de2fc*/ extern float k_f_0_04;
/*005df7e8*/ extern float k_f_n0_02;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a9390*/ extern undefined g_powerup_glow;
/*006a93c8*/ extern undefined g_powerups_clear_6a93c8;
/*006a93d0*/ extern undefined4 g_powerup_count;
/*006a93d4*/ extern undefined4 g_powerup_icons_txd;
/*006a93e0*/ extern undefined g_particles_6a93e0;
/*006a9428*/ extern undefined4 g_weapon_6a9428;
/*006a942c*/ extern undefined4 g_weapon_6a942c;
/*006a9430*/ extern undefined4 g_weapon_6a9430;
/*006a9458*/ extern undefined4 g_weapon_6a9458;
/*006a9580*/ extern undefined4 g_particles_6a9580;
/*006a9588*/ extern char *g_weapon_6a9588;
/*006a958c*/ extern char *g_weapon_flamethrower_6a958c;
/*006a9590*/ extern char *g_weapon_flamethrower_6a9590;
/*006a9598*/ extern undefined4 g_weapon_flamethrower_6a9598;
/*006a959c*/ extern undefined g_weapon_flamethrower_6a959c;
/*006a95a0*/ extern undefined g_weapon_flamethrower_6a95a0;
/*006a95a4*/ extern undefined g_weapon_flamethrower_6a95a4;
/*006a95a8*/ extern char *g_weapon_flamethrower_6a95a8;
/*006a95ac*/ extern undefined g_weapon_flamethrower_6a95ac;
/*006a9728*/ extern undefined4 g_weapon_6a9728;
/*006a972c*/ extern undefined4 g_weapon_flamethrower_6a972c;
/*006a9730*/ extern undefined4 g_weapon_flamethrower_6a9730;
/*006a9734*/ extern undefined4 g_weapon_flamethrower_6a9734;
/*006a9738*/ extern undefined4 g_weapon_flamethrower_6a9738;
/*006a973c*/ extern undefined4 g_weapon_flamethrower_6a973c;
/*006a974c*/ extern undefined g_weapon_flamethrower_6a974c;
/*006a9750*/ extern undefined4 g_weapon_flamethrower_6a9750;
/*006a9758*/ extern undefined4 g_weapon_flamethrower_6a9758;
/*006a9d44*/ extern undefined g_weapon_fx_6a9d44;
/*006aab80*/ extern undefined g_weapon_shotgun_6aab80;
/*006aab84*/ extern undefined4 g_weapon_shotgun_6aab84;
/*006aab94*/ extern undefined4 g_weapon_shotgun_6aab94;
/*006aab98*/ extern undefined4 g_weapon_shotgun_6aab98;
/*006aab9c*/ extern char *g_weapon_shotgun_6aab9c;
/*006aaba0*/ extern undefined4 g_weapon_shotgun_6aaba0;
/*006aac18*/ extern undefined g_player_status;
/*006ab194*/ extern undefined g_weapon_fx_6ab194;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00801b14*/ extern undefined4 g_rw_801b14;
/* weapon_mines_race_start */

extern void weapon_mines_race_start(void);


/* powerups_destroy */

/* destroys the 25 clumps and the particle system */

extern void powerups_destroy(void);


/* powerup_pickup_fx */

/* spawns 10 icon particles of the powerup type at the pick-up position */

extern void powerup_pickup_fx(char *param_1);

#define POWERUP_STRIDE_FLOATS (0x14)          /* floats per struct Powerup record: 0x50 bytes */


/* powerups_reset_all */

/*
 * Sets every placed powerup back to available (state 1). For each record: spawn_position = position (the
 * powerup returns to where it was placed; the two field names may be swapped (inferred)), the 3 floats at
 * +0x44 are zeroed (inferred: velocity), respawn_timer = 0. Takes no parameters, returns nothing.
 */

/* TIER2: validated 2026-10-18 authority=diffemu */
void powerups_reset_all(void)
{
  float *spawn_cursor;
  int powerup_index;
  
  /* no placed powerups: nothing to reset (the do-while below needs at least one record) */
  if (g_powerup_count != 0) {
    spawn_cursor = g_powerups[g_powerup_count].spawn_position;
    powerup_index = g_powerup_count;
    /* walk the placed records from the last one down to index 0, writing each record's fields */
    do {
      powerup_index = powerup_index + -1;
      spawn_cursor[-POWERUP_STRIDE_FLOATS] = spawn_cursor[-0x17];
      spawn_cursor[-0x13] = spawn_cursor[-0x16];
      spawn_cursor[-0x12] = spawn_cursor[-0x15];
      spawn_cursor[-0x11] = 0.0;
      spawn_cursor[-0x10] = 0.0;
      spawn_cursor[-0xf] = 0.0;
      *(int *)&spawn_cursor[-0x1a] = 1;      /* defectlint: was the denormal float literal 1.4013e-45 (the dword 1) */
      spawn_cursor[-0x1b] = 0.0;
      spawn_cursor = spawn_cursor + -POWERUP_STRIDE_FLOATS;
    } while (powerup_index != 0);
  }
  return;
}


/* powerups_clear */

/*
 * Resets all 25 powerup slots (g_powerups[0..24]) and sets g_powerup_count to 0. Takes no parameters, returns nothing.
 * Per slot: the frame of the slot's clump (clump->object.parent) is set to identity; the clump pointer and special (+0x28)
 * are left alone; state = 0 (unused), type = 0, respawn_time and respawn_timer = 0; position, spawn_position and the
 * floats at +0x44..+0x4c = 0 (the +0x44..+0x4c ones are inferred: velocity and padding).
 * Called when a course is loaded (inferred).
 */

/* TIER2: validated 2026-10-18 authority=diffemu */
void powerups_clear(void)
{
  undefined4 *cursor;
  undefined4 *slot_spawn;

  cursor = (undefined4 *)&g_powerups_clear_6a93c8;
  /* walk the slots from the end of g_powerups down to slot 0; cursor starts one slot past the end, so cursor[-0x22] is the clump of the slot below it */
  do {
    slot_spawn = cursor + -POWERUP_STRIDE_FLOATS;
    /* reset the frame of this slot's clump: clump->object.parent (RwFrame *) */
    RwFrameSetIdentity(*(RwFrame **)(cursor[-0x22] + 4));
    /* the slot's fields, offsets from cursor: slot_spawn is spawn_position[0] (+0x38); the rest of the record follows */
    *slot_spawn = 0;
    cursor[-0x13] = 0;
    cursor[-0x17] = 0;
    cursor[-0x11] = 0;
    cursor[-0x12] = 0;
    cursor[-0x16] = 0;
    cursor[-0x10] = 0;
    cursor[-0x15] = 0;
    cursor[-0xf] = 0;
    cursor[-0x1a] = 0;
    cursor[-0x19] = 0;
    cursor[-0x1c] = 0;
    cursor[-0x1b] = 0;
    cursor = slot_spawn;
  } while ((float *)slot_spawn != g_powerups[0].spawn_position);
  g_powerup_count = 0;
  return;
}


/* powerup_set_enabled */

/* "(index, enable): disabled = state 3; enabling sets state 1 again" */

extern undefined4 __cdecl powerup_set_enabled(char *param_1,char *param_2);


/* powerup_place_with_respawn */

/* "powerup_place + powerup_set_respawn_time" */

extern void __cdecl powerup_place_with_respawn(char *param_1,char *param_2,char *param_3);


/* weapon_effects_destroy_6a9458 */

/* "destroys the clump and matrix of every 0x58 byte record at 0x6A9458" */

extern void weapon_effects_destroy_6a9458(void);


/* weapon_effect_slot_clear */

/* "(slot): active = 0, id = -1" */

extern void weapon_effect_slot_clear(char *param_1);


/* weapon_effect_get_slot_value_a */

/* "(player 0..3): g_weapon_6a942c[player * 0x58] or -1" */

extern undefined4 weapon_effect_get_slot_value_a(char *param_1);


/* weapon_effect_get_slot_value_b */

/* "(player 0..3): g_weapon_6a9428[player * 0x58] or -1" */

extern undefined4 weapon_effect_get_slot_value_b(char *param_1);

#define FX_GRID_PLANE_DWORDS (0x145)          /* dwords per plane of the fx table: FX_GRID_ROWS_PER_PLANE rows */
#define FX_GRID_ROW_DWORDS (0x41)             /* dwords per row: FX_GRID_CELLS_PER_ROW cells */
#define FX_GRID_CELL_DWORDS (0xd)             /* dwords per fx cell, 0x34 bytes (inferred) */
#define FX_GRID_ROWS_PER_PLANE (5)
#define FX_GRID_CELLS_PER_ROW (5)


/* weapon_fx_grid_clear */

/*
 * Clears the fx grid table at 0x6AB194 and resets the player record passed in: dword 0 (active flag) = 0,
 * dwords 5 and 6 = 0, dword 3 = 1.0f (inferred). The table is 4 planes x 5 rows x 5 cells (inferred from the
 * loop bounds, 0x1450 bytes; the old note said 5 x 5 x 5). Parameter record: the player's flame record
 * (undefined4 *), see weapon_flamethrower_equip.
 */

/* TIER2: validated 2026-10-18 authority=diffemu */
void weapon_fx_grid_clear(undefined4 *record)
{
  undefined4 *cell;
  int cells_left;
  int rows_left;
  undefined4 *row;
  undefined4 *plane;

  /* walk the table backwards, one plane (FX_GRID_PLANE_DWORDS dwords) at a time, until the start of the table */
  plane = (undefined4 *)&g_weapon_fx_6ab194;
  do {
    plane = plane + -FX_GRID_PLANE_DWORDS;
    rows_left = FX_GRID_ROWS_PER_PLANE;
    row = plane;
    /* each plane: walk its rows backwards */
    do {
      row = row + -FX_GRID_ROW_DWORDS;
      cells_left = FX_GRID_CELLS_PER_ROW;
      cell = row;
      /* each row: zero the 10 dwords the code writes per cell (cell layout inferred) */
      do {
        cells_left = cells_left + -1;
        cell[-0xc] = 0;
        cell[-0xd] = 0;
        cell[-0xe] = 0;
        cell[-9] = 0;
        cell[-10] = 0;
        cell[-0xb] = 0;
        cell[-6] = 0;
        cell[-8] = 0;
        cell[-3] = 0;
        cell[-2] = 0;
        cell = cell + -FX_GRID_CELL_DWORDS;
      } while (cells_left != 0);
      rows_left = rows_left + -1;
    } while (rows_left != 0);
  } while (plane != (undefined4 *)&g_weapon_fx_6a9d44);
  /* reset the record: active flag, dwords 5 and 6, and the 1.0f at dword 3 */
  *record = 0;
  record[5] = 0;
  record[6] = 0;
  record[3] = FLOAT_ONE_BITS;
  return;
}


/* weapon_flamethrower_free_assets */

extern void weapon_flamethrower_free_assets(void);

#define FLAME_CLUMP_STRIDE_DWORDS (0x1a)      /* dwords per player's clump slot: 0x68 bytes */



extern undefined4 * weapon_flamethrower_equip(WeaponSlot *slot);


/* weapon_flamethrower_ammo_fraction */

extern double weapon_flamethrower_ammo_fraction(WeaponSlot *param_1);


/* weapon_flame_push_cars */

/* "flame boost: adds velocity to the player car and cars inside the flame (car_add_velocity)" */

extern void __cdecl weapon_flame_push_cars(char *param_1,char *param_2,char *param_3,char *param_4);


/* weapon_flamethrower_clear_effects */

/* TIER2: validated 2026-10-11 authority=diffemu */

/*
 * Draws the flamethrower effects with depth writes off. Takes no parameters, returns nothing. Walks the 4 per-player
 * records: the first dword of each non-zero record is an fx system handle, drawn with fx_system_render. Then, for each
 * record whose active flag is set (flag 0x24 bytes before the record, inferred), calls the slot-0x48 method of the
 * record's object (inferred: draw/clear the effect).
 */
/* TIER2: validated 2026-10-22 authority=diffemu */
void weapon_flamethrower_clear_effects(void)
{
  int *active_flag;
  undefined4 *cursor;
  int *record;

  /* z-write off for the fx systems */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  /* walk the records from the end of the table down to g_weapon_flamethrower_6a9598, render each non-zero fx system */
  cursor = &g_weapon_flamethrower_6a9738;
  do {
    record = cursor + -FLAME_CLUMP_STRIDE_DWORDS;
    cursor = cursor + -FLAME_CLUMP_STRIDE_DWORDS;
    if (*record != 0) {
      fx_system_render(*record);
    }
  } while (cursor != &g_weapon_flamethrower_6a9598);
  /* z-write back on */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  /* walk the same records from g_weapon_flamethrower_6a974c down to g_weapon_flamethrower_6a95ac, calling the method of the active ones */
  record = (int *)&g_weapon_flamethrower_6a974c;
  do {
    active_flag = record + -0x23;
    record = record + -FLAME_CLUMP_STRIDE_DWORDS;
    if (*active_flag != 0) {
      (**(code **)(*record + 0x48))(*record);
    }
  } while (record != (int *)&g_weapon_flamethrower_6a95ac);
  return;
}


/* weapon_flamethrower_reset_all */

extern void weapon_flamethrower_reset_all(void);


/* weapon_flamethrower_course_33_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void weapon_flamethrower_course_33_update(void);
extern char g_weapon_shotgun_6aac14[];
extern char g_weapon_shotgun_6aab8c[];
extern char g_weapon_6aac28[];


/* weapon_shotgun_equip */

/* type 0x11; state 0x6AAB80 stride 0x24 */

#define SHOTGUN_RECORD_STRIDE_BYTES (0x24)    /* bytes per shotgun effect record; the 4 records start at 0x6AAB80 */
#define SHOTGUN_RECORD_STRIDE_DWORDS (9)      /* the same stride in dwords (state field to state field) */
#define SHOTGUN_STATE_SCAN_END (((LONG_PTR)&g_weapon_6aac28))     /* scan limit: one past the state field of the 4th record (inferred) */

/*
 * Equips the shotgun for the weapon slot: takes the first free effect record (state 0 = free), marks it active
 * (state 1), stores the slot at record + 0x14 and sets record + 0x10 = 3 and record + 0x20 = 0 (meanings inferred).
 * Adds the record's clump (record + 0x1c) to the course world. Parameter: slot (WeaponSlot *). Returns the record
 * (undefined *). If all 4 records are busy the scan stops at index 4 and the writes land past the table (inferred
 * from the loop bound).
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined * weapon_shotgun_equip(WeaponSlot *slot)
{
  int *record_state;
  RpWorld *world;
  int record_index;
  RpClump *clump;

  /* find the first record whose state is 0 (free); record_index ends on that record */
  record_index = 0;
  record_state = &g_weapon_shotgun_6aab98;
  do {
    if (*record_state == 0) break;
    record_state = record_state + SHOTGUN_RECORD_STRIDE_DWORDS;
    record_index = record_index + 1;
  } while ((LONG_PTR)record_state < SHOTGUN_STATE_SCAN_END);
  /* set up the record: clump, type 3 (inferred), active state, slot and cleared +0x20 */
  clump = (RpClump *)(&g_weapon_shotgun_6aab9c)[record_index * SHOTGUN_RECORD_STRIDE_DWORDS];
  *(undefined4 *)(record_index * SHOTGUN_RECORD_STRIDE_BYTES + ((LONG_PTR)&g_weapon_shotgun_6aab8c + 0x4)) = 3;
  (&g_weapon_shotgun_6aab98)[record_index * SHOTGUN_RECORD_STRIDE_DWORDS] = 1;
  (&g_weapon_shotgun_6aaba0)[record_index * SHOTGUN_RECORD_STRIDE_DWORDS] = 0;
  (&g_weapon_shotgun_6aab94)[record_index * SHOTGUN_RECORD_STRIDE_DWORDS] = slot;
  world = (RpWorld *)course_get_rpworld_00467e90();
  RpWorldAddClump(world,clump);
  return &g_weapon_shotgun_6aab80 + record_index * SHOTGUN_RECORD_STRIDE_BYTES;
}


/* weapon_shotgun_ammo_fraction */

extern double weapon_shotgun_ammo_fraction(WeaponSlot *param_1);

#define SHOTGUN_RECORDS_END (((LONG_PTR)&g_weapon_shotgun_6aac14))     /* address one past the last shotgun effect record (4 records of 0x24 bytes from 0x6AAB84) */


/* weapon_shotgun_clear_effects */

/* TIER2: validated 2026-10-11 authority=diffemu */

/*
 * Draws the shotgun effects with depth writes off: for every active shotgun record (state 1 with its
 * second object set) calls the slot-0x48 method of both of its objects (inferred: draw/clear the effect),
 * then turns depth writes back on. Takes no parameters, returns nothing.
 */
/* TIER2: validated 2026-10-22 authority=diffemu */
void weapon_shotgun_clear_effects(void)
{
  int *record;

  /* turn z-write off for the effects */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  /* walk the 4 shotgun records, 9 dwords apart; draw the active ones */
  record = &g_weapon_shotgun_6aab84;
  do {
    if ((record[5] == 1) && (record[2] != 0)) {
      (**(code **)(*record + 0x48))(*record);
      (**(code **)(record[1] + 0x48))(record[1]);
    }
    record = record + 9;
  } while ((LONG_PTR)record < SHOTGUN_RECORDS_END);
  /* restore z-write */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


/* player_status_get */

/* "(player): g_player_status[player]" */

extern undefined4 __cdecl player_status_get(char *param_1);


/* course_get_rpworld_00467e90 */

/* duplicate name: was course_get_rpworld (first address 0042a3f0) */

extern uint course_get_rpworld_00467e90(void);


/* weapon_get_rw_801b14 */

/* "return g_rw_801b14;" */

extern undefined4 weapon_get_rw_801b14(void);


