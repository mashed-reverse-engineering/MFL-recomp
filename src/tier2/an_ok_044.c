#include "mfl_tier2.h"
/*004089e0*/ extern int fx_object_get_rwp_id();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00458170*/ extern int __cdecl course_point_in_water_box_test(struct RwV3d *p0);
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*0045f880*/ extern undefined weapon_fx_explosion_slot_clear(undefined4 p0);
/*0045f8f0*/ extern undefined weapon_drum_drop(undefined4 p0);
/*0045fb00*/ extern int weapon_drum_update_one();
/*004600f0*/ extern undefined weapon_fx_explosion_pool_clear(undefined4 p0);
/*00460500*/ extern undefined weapon_fx_clump_remove_a(undefined4 p0);
/*00460530*/ extern int flashunit_state_clear(undefined4 p0);
/*00460540*/ extern undefined weapon_flashbang_start_effect(undefined4 p0);
/*004605a0*/ extern undefined weapon_flashbang_state_step(undefined4 p0);
/*00460610*/ extern undefined weapon_flashbang_flare_particles(undefined4 p0);
/*00460970*/ extern int weapon_effects_clear_6a5dd0();
/*00462b10*/ extern undefined weapon_oil_pick_slot(undefined4 p0);
/*00462d70*/ extern int weapon_oil_slicks_update_render();
/*004637e0*/ extern undefined weapon_fx_slot_clear_b(undefined4 p0);
/*00463850*/ extern undefined weapon_mine_drop(undefined4 p0);
/*00463c60*/ extern undefined weapon_fx_pool_clear_b(undefined4 p0);
/*00463cc0*/ extern int weapon_mine_update_one();
/*00465390*/ extern int weapon_effect_slot_clear();
/*00465470*/ extern int weapon_find_targets_in_cone();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00465f60*/ extern int weapon_effect_get_slot_record();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*004726a0*/ extern int sfx_play_at();
/*0047a4b0*/ extern int car_set_field_set();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*0047b7c0*/ extern int car_set_field_144();
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, int p1);      /* diffemu: the original takes (clump, flag), the flag is read from the stack slot */
/*00484680*/ extern int object_set_flag_4();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*00486710*/ extern int particles_destroy();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8(float p0);      /* defectlint: was `()`: the float was pushed as a double */
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00486ef0*/ extern int particles_render();
/*0049c0f0*/ extern int fx_system_render_no_zwrite();
/*0049c120*/ extern int fx_buffers_free();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd378*/ extern float k_f_0_1;
/*005dd420*/ extern float k_f_n3;
/*005ddacc*/ extern float k_f_360;
/*005ddb34*/ extern float k_f_0_95;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de20c*/ extern float k_f_2_5;
/*005de270*/ extern float k_f_n0_1;
/*005df470*/ extern float k_f_n2_5;
/*005df5c0*/ extern float k_f_49;
/*005df7d4*/ extern float k_f_n1_0en4;
/*005df7d8*/ extern float k_f_0_025000006;
/*006a5a40*/ extern undefined g_weapon_6a5a40;
/*006a5c60*/ extern undefined g_weapon_6a5c60;
/*006a5c70*/ extern undefined4 g_weapon_drum_6a5c70;
/*006a5c78*/ extern undefined4 g_weapon_drum_6a5c78;
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern undefined g_weapon_6a5d78;
/*006a5d84*/ extern undefined4 g_weapon_flashbang_6a5d84;
/*006a5d88*/ extern char *g_weapon_6a5d88;
/*006a5dc8*/ extern undefined4 g_weapon_6a5dc8;
/*006a5e2c*/ extern undefined4 g_weapon_6a5e2c;
/*006a5e34*/ extern undefined g_weapon_rocket_6a5e34;
/*006a5e38*/ extern undefined4 g_weapon_rocket_6a5e38;
/*006a5ff4*/ extern undefined g_weapon_rocket_6a5ff4;
/*006a5ff8*/ extern undefined g_weapon_6a5ff8;
/*006a6008*/ extern undefined4 g_weapon_6a6008;
/*006a6010*/ extern undefined g_weapon_rocket_6a6010;
/*006a6048*/ extern undefined g_weapon_rocket_6a6048;
/*006a6050*/ extern undefined g_weapon_rocket_6a6050;
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/*006a61f0*/ extern undefined g_weapon_6a61f0;
/*006a6204*/ extern undefined g_weapon_machinegun_6a6204;
/*006a6230*/ extern undefined4 g_weapon_6a6230;
/*006a6234*/ extern undefined g_weapon_machinegun_6a6234;
/*006a6280*/ extern undefined4 g_weapon_oil_6a6280;
/*006a7c38*/ extern undefined g_weapon_6a7c38;
/*006a7c78*/ extern char *g_weapon_6a7c78;
/*006a7c7c*/ extern undefined4 g_weapon_6a7c7c;
/*006a7c80*/ extern undefined4 g_weapon_oil_6a7c80;
/*006a7c84*/ extern char *g_weapon_6a7c84;
/*006a7ce8*/ extern undefined g_weapon_6a7ce8;
/*006a7d28*/ extern undefined1 g_weapon_6a7d28;
/*006a7d38*/ extern undefined g_weapon_6a7d38;
/*006a7d78*/ extern undefined4 g_weapon_mines_6a7d78;
/*006a7d90*/ extern undefined4 g_weapon_6a7d90;
/*006a7d98*/ extern undefined4 g_weapon_mines_6a7d98;
/*006a8ab8*/ extern undefined g_weapon_6a8ab8;
/*006a8af8*/ extern undefined g_weapon_6a8af8;
/*006a8b04*/ extern undefined4 g_weapon_mines_6a8b04;
/*006a8b08*/ extern undefined4 g_weapon_mines_6a8b08;
/*006a8b0c*/ extern char *g_weapon_6a8b0c;
/*006a8b58*/ extern undefined4 g_weapon_6a8b58;
/*006a8b5c*/ extern undefined4 g_weapon_6a8b5c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7f4*/ extern float g_tick_seconds;
/*008bf000*/ extern undefined g_weapon_8bf000;
/*006a8b18*/ extern char g_weapon_mines_6a8b18[];
/*006a5dd4*/ extern char g_weapon_flashbang_6a5dd4[];
/*006a8b68*/ extern char g_weapon_6a8b68[];
/*006a8b10*/ extern char g_weapon_mines_6a8b10[];
/*006a5d28*/ extern char g_weapon_drum_6a5d28[];

#define DRUM_RECORD_DWORDS (0xb)        /* record stride: 0x2C bytes */
#define DRUM_RECORDS_END (((int)&g_weapon_drum_6a5d28))     /* end address of the drum record array */
#define VTABLE_RELEASE_OFFSET (0x48)    /* vtable slot called on each owned object (inferred) */
#define MACHINEGUN_STATE_DWORDS (0x12)  /* sizeof(struct MachinegunState) = 0x48 bytes */
#define MINE_RECORD_DWORDS (0x1b)       /* mine record stride: 0x6C bytes */
#define MINE_RECORDS_END (((int)&g_weapon_mines_6a8b10))     /* end address of the mine record array */
#define SPARK_GREY_RANGE (0x80)         /* random_int_range upper bound; grey = -1 - r gives 127..255 (inclusive range, inferred) */
#define SPARK_MAX_COUNT (0x80)          /* cap on the spark counter g_weapon_6a6230 (inferred) */
#define SPARK_ALPHA (0xff)              /* alpha byte of the colour word */
#define SPARK_LERP_FRACTION (0x3dcccccd) /* float 0.1: how far the spark moves toward the aux camera (inferred) */
#define SPARK_ANGLE_RANGE (0x43b40000)  /* float 360.0: upper bound of the random angle (inferred) */
#define MINE_EQUIP_RECORD_BYTES (0x18)  /* stride of the equip record array at 0x6A8AF8 */
#define MINE_EQUIP_RECORD_DWORDS (6)    /* the same stride in dwords */
#define MINE_EQUIP_RECORDS_END (((LONG_PTR)&g_weapon_6a8b68)) /* end address of the equip record array */
#define MINE_EQUIP_COUNT_OFFSET (((LONG_PTR)&g_weapon_6a8af8 + 0x8)) /* attached-count dword: record+0x8 of record 0 (inferred) */
#define MINE_EQUIP_MAX_ATTACHED (2)     /* mine objects attached to one equip record (inferred) */
#define FLASHBANG_RECORD_DWORDS (5)     /* flashbang record stride in dwords */
#define FLASHBANG_RECORD_BYTES (0x14)   /* the same stride in bytes */
#define FLASHBANG_RECORDS_END (((LONG_PTR)&g_weapon_flashbang_6a5dd4)) /* end address of the flashbang record array */
#define ROCKET_RECORD_DWORDS (0x1b)     /* rocket record stride: 0x6C bytes (inferred from the loop step) */
#define ROCKET_ACTIVE_DWORD (-0x1e)     /* in-use flag, dword index from record_end (record base + 0x1b - 0x1e) (inferred) */
#define ROCKET_POS_X_DWORD (-0x2f)      /* stored position x, dword index from record_end (inferred) */
#define ROCKET_POS_Y_DWORD (-0x2e)      /* stored position y, dword index from record_end (inferred) */
#define ROCKET_POS_Z_DWORD (-0x2d)      /* stored position z, dword index from record_end (inferred) */
#define OIL_RECORD_DWORDS (4)           /* oil equip record stride in dwords (state at 0x6A7C78) */
#define OIL_RECORD_BYTES (0x10)         /* the same stride in bytes */
#define OIL_FULL_AMMO_BITS (0x3f800000) /* float 1.0 bits written to the record on equip: full ammo (inferred) */
#define SLOT_FLAG_BIT_MASK (7)          /* bit index within one flag byte of g_weapon_6a7d28 (8 slots per byte) */
#define SLOT_FLAG_BYTE_SHIFT (3)        /* slot index >> 3 selects the flag byte */
#define OIL_WATER_BOX_END (((LONG_PTR)&g_weapon_oil_6a7c80))    /* end address of the oil water box array (start of g_weapon_oil_6a7c80) (inferred) */
/* weapon_drum_clear_effects */

/* TIER2: validated 2026-10-11 authority=diffemu */
/* Clears the drum weapon's effect records (no parameters, returns nothing).
 * Each record is 0x2C bytes; an active record (state 1) releases its two
 * objects through the vtable slot when that object's flag is set (inferred:
 * the flag at record+0x18 covers object_a at record+0x8, the one at
 * record+0x14 covers object_b at record+0xc). */
/* TIER2: validated 2026-10-18 authority=diffemu */
void weapon_drum_clear_effects(void)
{
  int *record;

  /* walk the record array from the first record up to DRUM_RECORDS_END */
  record = &g_weapon_drum_6a5c78;
  do {
    /* only active records release their objects */
    if (record[-2] == 1) {
      if (record[4] != 0) {
        (**(code **)(*record + VTABLE_RELEASE_OFFSET))(*record);
      }
      if (record[3] != 0) {
        (**(code **)(record[1] + VTABLE_RELEASE_OFFSET))(record[1]);
      }
    }
    record = record + DRUM_RECORD_DWORDS;
  } while ((LONG_PTR)record < DRUM_RECORDS_END);
  return;
}


/* weapon_drum_fire */

extern void weapon_drum_fire(WeaponSlot *param_1,char *param_2);


/* weapon_drum_update */

extern void weapon_drum_update(void);


/* weapon_effects_clear_6a5c60 */

/* "clears every 0x2C byte effect record from 0x6A5C60 to 0x6A5D10" */

extern void weapon_effects_clear_6a5c60(void);


/* weapon_drum_race_start */

extern void weapon_drum_race_start(void);


/* weapon_flashbang_equip */

/* type 0x12; state 0x6A5D78 stride 0x14 */

/* Equips a flashbang for a weapon slot: takes the first free record of the
 * flashbang record array (4 records, stride 0x14 bytes, the array starts at
 * 0x6A5D78), marks it in use, stores the owning slot in it, adds the record's
 * clump to the course world and returns the record (undefined *).
 * Parameter slot: the WeaponSlot that receives the flashbang.
 * No free record: the search runs past the array and the write goes out of
 * bounds (inferred, no guard). */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined * weapon_flashbang_equip(WeaponSlot *slot)
{
  int *record;
  RpWorld *world;
  int record_index;
  RpClump *clump;

  /* find the first record whose in-use flag is clear */
  record_index = 0;
  record = &g_weapon_flashbang_6a5d84;
  do {
    if (*record == 0) break;
    record = record + FLASHBANG_RECORD_DWORDS;
    record_index = record_index + 1;
  } while ((LONG_PTR)record < FLASHBANG_RECORDS_END);
  /* mark it in use, store the owning slot and attach its clump to the world */
  clump = (RpClump *)(&g_weapon_6a5d88)[record_index * FLASHBANG_RECORD_DWORDS];
  (&g_weapon_flashbang_6a5d84)[record_index * FLASHBANG_RECORD_DWORDS] = 1;
  *(WeaponSlot **)(record_index * FLASHBANG_RECORD_BYTES + ((LONG_PTR)&g_weapon_6a5d78 + 0x8)) = slot;
  world = (RpWorld *)course_get_rpworld_00467e90();
  RpWorldAddClump(world,clump);
  return &g_weapon_6a5d78 + record_index * FLASHBANG_RECORD_BYTES;
}


/* weapon_flashbang_free_assets */

extern void weapon_flashbang_free_assets(void);


/* weapon_flashbang_fire */

extern void weapon_flashbang_fire(WeaponSlot *param_1,char *param_2);


/* weapon_flashbang_update */

extern void weapon_flashbang_update(void);


/* weapon_flashbang_race_start */

extern void weapon_flashbang_race_start(void);


/* weapon_rocket_unequip */

extern void weapon_rocket_unequip(WeaponSlot *param_1);


/* weapon_rocket_ammo_fraction */

extern float10 weapon_rocket_ammo_fraction(WeaponSlot *param_1);


/* weapon_rocket_free_assets */

extern void weapon_rocket_free_assets(void);


/* weapon_rocket_course_33_update */

/* Refreshes the stored position of every rocket record (no parameters, returns nothing).
 * A record's object pointer at +0x0 leads to an RwFrame (its parent at +0x4). The frame's
 * modelling matrix (+0x10) is kept inside the course zone by the callee (inferred from its
 * name; the matrix is passed as both input and output), then the matrix position
 * (+0x40..+0x48) is copied into the record's three stored position dwords (inferred).
 * Only records whose in-use flag is set are updated (inferred). */
/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { int a, b, c; } V3y;
void weapon_rocket_course_33_update(void)
{
  char *frame;
  int *p;
  int active;

  p = (int *)&g_weapon_rocket_6a6048;
  do {
    active = p[-0x1e];
    p = p + -ROCKET_RECORD_DWORDS;
    if (active != 0) {
      frame = *(char **)(*p + 4) + 0x10;
      course_zone_contains_matrix_in_zone_6a2888(frame,frame);
      *(V3y *)(p - 0x14) = *(V3y *)(frame + 0x30);
    }
  } while (p != &g_weapon_6a5e2c);
  return;
}


/* weapon_rocket_clear_effects */

extern void weapon_rocket_clear_effects(void);


/* weapon_rocket_reset_all */

extern void weapon_rocket_reset_all(void);


/* weapon_particle_spark_emit */

/* "(pos, size): random grey spark particle moving toward the aux camera" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Emits one random grey spark particle at pos, moving toward the aux camera (inferred).
 * pos is the spark's world position; size is written to both floats of the size pair
 * (width and height). Does nothing once the spark counter g_weapon_6a6230 has reached
 * SPARK_MAX_COUNT. Returns nothing. */
/* TIER2: unvalidated 2026-10-16 authority=diffemu */
void weapon_particle_spark_emit(undefined4 pos,float size)
{
  char grey;
  float10 angle;
  float size_pair [2];
  undefined1 lerped_pos [12];
  RwRGBA color;

  size_pair[0] = size;
  size_pair[1] = size;
  grey = random_int_range(0,SPARK_GREY_RANGE);
  if ((int)g_weapon_6a6230 < SPARK_MAX_COUNT) {
    grey = -1 - grey;
    color.red = grey;
    color.green = grey;
    color.blue = grey;
    color.alpha = SPARK_ALPHA;
    particles_set_color(&color);
    particles_set_size(size_pair);
    point_lerp_to_aux_camera(pos,lerped_pos,SPARK_LERP_FRACTION);
    particles_set_position(lerped_pos);
    angle = (float10)random_range(0,SPARK_ANGLE_RANGE);
    gfx_set_gfx_6c50c8((float)angle);
    particles_add(&g_weapon_6a61f0);
    g_weapon_6a6230 = g_weapon_6a6230 + 1;
  }
  return;
}


/* weapon_machinegun_unequip */

extern void weapon_machinegun_unequip(WeaponSlot *param_1);


/* weapon_machinegun_free_assets */

extern void weapon_machinegun_free_assets(void);


/* weapon_machinegun_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void weapon_machinegun_update(void);


/* weapon_machinegun_clear_effects */

/* TIER2: validated 2026-10-11 authority=diffemu */
/* Clears the machinegun's fire state (no parameters, returns nothing).
 * Draws the shared machinegun particles, releases the object of every
 * MachinegunState record whose `fired` flag is set (inferred: the object
 * lives at record+0x2c and is released through its vtable), clears that
 * flag, then resets the spark counter g_weapon_6a6230 (inferred). */
/* TIER2: validated 2026-10-18 authority=diffemu */
void weapon_machinegun_clear_effects(void)
{
  undefined4 *cursor;
  undefined4 *fired_flag;

  particles_render(&g_weapon_6a61f0);
  /* walk the records from last to first; the cursor starts one record past the end */
  cursor = (undefined4 *)&g_weapon_machinegun_6a6204;
  do {
    fired_flag = cursor + -MACHINEGUN_STATE_DWORDS;
    if (cursor[-MACHINEGUN_STATE_DWORDS] != 0) {
      /* record+0x2c is a gap in the dossier struct, so it stays a raw dword offset */
      (**(code **)(cursor[-0xc] + VTABLE_RELEASE_OFFSET))(cursor[-0xc]);
      *fired_flag = 0;
    }
    cursor = fired_flag;
  } while (fired_flag != &MachinegunState_ARRAY_006a60d0[0].fired);
  g_weapon_6a6230 = 0;
  return;
}


/* weapon_oil_free_assets */

extern void weapon_oil_free_assets(void);


/* weapon_oil_equip */

/* type 0x13; state 0x6A7C78 stride 0x10 */

/* Equips the oil weapon for the player that owns weapon slot (returns the player's oil record, undefined4 *).
 * The record is indexed by slot->player (stride OIL_RECORD_BYTES): the slot pointer is stored in it,
 * its first dword is set to full ammo (1.0f, inferred), its clump is added to the course world, the
 * clump's atomics get flag 1, and the record's active dword is set (inferred). */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined4 * weapon_oil_equip(WeaponSlot *slot)
{
  char *rec;

  rec = (char *)&g_weapon_6a7c78 + slot->player * OIL_RECORD_BYTES;
  *(WeaponSlot **)(rec + 8) = slot;
  *(undefined4 *)rec = OIL_FULL_AMMO_BITS;
  RpWorldAddClump((RpWorld *)course_get_rpworld_00467e90(),*(RpClump **)(rec + 0xc));
  clump_set_flag_on_atomics(*(RpClump **)(rec + 0xc),1);
  *(undefined4 *)(rec + 4) = 1;
  return (undefined4 *)rec;
}


/* weapon_oil_is_finished */

extern undefined4 weapon_oil_is_finished(WeaponSlot *param_1);


/* weapon_oil_ammo_fraction */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern float10 weapon_oil_ammo_fraction(WeaponSlot *param_1);




/* weapon_oil_course_2_update */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Per-course update for the oil weapon (no parameters, returns nothing).
 * Walks the oil water boxes in order, one per slot (slot index = bit index across the
 * flag bytes at g_weapon_6a7d28). A slot whose flag bit is set has its box tested with
 * course_point_in_water_box_test; when that test returns non-zero the bit is cleared
 * (inferred meaning of the test result). */
/* TIER2: validated 2026-10-18 authority=diffemu */
void weapon_oil_course_2_update(void)
{
  byte *flag_byte;
  int in_water_box;
  byte bit_in_byte;
  int slot_index;
  RwV3d *water_box;

  slot_index = 0;
  water_box = (RwV3d *)&g_weapon_oil_6a6280;
  /* one pass per oil slot; the box cursor advances by one box stride each time */
  do {
    bit_in_byte = (byte)slot_index & SLOT_FLAG_BIT_MASK;
    flag_byte = &g_weapon_6a7d28 + (slot_index >> SLOT_FLAG_BYTE_SHIFT);
    if ((*flag_byte & (byte)(1 << bit_in_byte)) != 0) {
      in_water_box = course_point_in_water_box_test(water_box);
      if (in_water_box != 0) {
        *flag_byte = *flag_byte & ~('\x01' << bit_in_byte);
      }
    }
    water_box = (RwV3d *)&water_box[8].z;
    slot_index = slot_index + 1;
  } while ((LONG_PTR)water_box < OIL_WATER_BOX_END);
  return;
}


/* weapon_effects_any_active */

/* "1 when any slot of g_weapon_6a7d28 is set" */

extern undefined4 weapon_effects_any_active(void);


/* weapon_oil_update */

extern void weapon_oil_update(void);


/* weapon_oil_pick_slot_forward */

/* "calls weapon_oil_pick_slot" */

extern void weapon_oil_pick_slot_forward(char *param_1);


/* weapon_mines_equip */

/* type 0x0C; state 0x6A8AF8 stride 0x18 */

/* TIER2: validated 2026-10-11 authority=diffemu */
/* Equips one mine for weapon_slot (returns the claimed equip record, inferred).
 * Claims the first unused equip record, stores weapon_slot in it, attaches up to
 * MINE_EQUIP_MAX_ATTACHED unused mine objects to it (inferred), then adds the
 * record's clump to the course world. */
/* TIER2: validated 2026-10-18 authority=diffemu */
undefined * weapon_mines_equip(WeaponSlot *weapon_slot)
{
  int record_offset;
  undefined *record;
  int *flag_ptr;
  RpWorld *world;
  int record_index;
  int attached_count;
  RpClump *clump;
  
  /* find the first unused equip record; record_index is its index (inferred) */
  record_index = 0;
  flag_ptr = &g_weapon_mines_6a8b08;
  do {
    if (*flag_ptr == 0) break;
    flag_ptr = flag_ptr + MINE_EQUIP_RECORD_DWORDS;
    record_index = record_index + 1;
  } while ((LONG_PTR)flag_ptr < MINE_EQUIP_RECORDS_END);
  record_offset = record_index * MINE_EQUIP_RECORD_BYTES;
  record = &g_weapon_6a8af8 + record_offset;
  /* attach unused mine objects to the record until MINE_EQUIP_MAX_ATTACHED is reached (inferred) */
  flag_ptr = &g_weapon_mines_6a7d98;
  do {
    if (*flag_ptr == 0) {
      *(int **)(record + *(int *)(record_offset + MINE_EQUIP_COUNT_OFFSET) * 4) = flag_ptr + -0x18;
      attached_count = *(int *)(record_offset + MINE_EQUIP_COUNT_OFFSET) + 1;
      *flag_ptr = 1;
      flag_ptr[-1] = (int)record;
      *(int *)(record_offset + MINE_EQUIP_COUNT_OFFSET) = attached_count;
      if (attached_count == MINE_EQUIP_MAX_ATTACHED) break;
    }
    flag_ptr = flag_ptr + MINE_RECORD_DWORDS;
  } while ((LONG_PTR)flag_ptr < ((LONG_PTR)&g_weapon_mines_6a8b18));
  clump = (RpClump *)(&g_weapon_6a8b0c)[record_index * MINE_EQUIP_RECORD_DWORDS];
  (&g_weapon_mines_6a8b08)[record_index * MINE_EQUIP_RECORD_DWORDS] = 1;
  (&g_weapon_mines_6a8b04)[record_index * MINE_EQUIP_RECORD_DWORDS] = weapon_slot;
  world = (RpWorld *)course_get_rpworld_00467e90();
  RpWorldAddClump(world,clump);
  return record;
}


/* weapon_mines_free_assets */

extern void weapon_mines_free_assets(void);


/* weapon_mines_clear_effects */

extern void weapon_mines_clear_effects(void);


/* weapon_mines_course_33_update */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Refreshes the stored world position of every mine record (no parameters, returns nothing).
 * A record's object pointer at +0x18 leads to an RwFrame whose modelling matrix is
 * re-zoned and transformed; the resulting position is written to the record's
 * first three floats (inferred). Only records whose state is 2..6 are updated (inferred). */
/* TIER2: validated 2026-10-18 authority=diffemu */
void weapon_mines_course_33_update(void)
{
  int *mine_record;
  RwMatrix new_matrix;

  mine_record = &g_weapon_6a7d90;
  /* walk the mine records from the first record up to MINE_RECORDS_END */
  do {
    /* only mines in an active state (2..6, inferred) get their position refreshed */
    if ((1 < mine_record[2]) && (mine_record[2] < 7)) {
      course_zone_contains_matrix_in_zone_6a2888(*(int *)(*mine_record + 4) + 0x10,&new_matrix);
      RwFrameTransform(*(RwFrame **)(*mine_record + 4),&new_matrix,rwCOMBINEREPLACE);
      *(float *)&mine_record[-6] = new_matrix.pos.x;      /* diffemu: float stores into an int array became _ftol conversions */
      *(float *)&mine_record[-5] = new_matrix.pos.y;
      *(float *)&mine_record[-4] = new_matrix.pos.z;
    }
    mine_record = mine_record + MINE_RECORD_DWORDS;
  } while ((LONG_PTR)mine_record < MINE_RECORDS_END);
  return;
}


/* weapon_mines_course_2_update */

extern void weapon_mines_course_2_update(void);


/* weapon_mines_fire */

extern void weapon_mines_fire(WeaponSlot *param_1,char *param_2);


/* weapon_mines_reset_all */

extern void weapon_mines_reset_all(void);


/* weapon_mines_update */

extern void weapon_mines_update(void);


/* weapon_effects_clear_6a8af8 */

/* "clears every 0x18 dword record from 0x6A8AF8 to 0x6A8B58" */

extern void weapon_effects_clear_6a8af8(void);



