#include "mfl_tier2.h"
/*00408aa0*/ extern int __cdecl fx_find_index_by_rwp_id(undefined4 p0, undefined4 p1);
/*00465390*/ extern int weapon_effect_slot_clear();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00465f60*/ extern int weapon_effect_get_slot_record();
/*00467e90*/ extern char *course_get_rpworld_00467e90();
/*00472890*/ extern int sfx_play_voice_at();
/*00482070*/ extern double dot_product_3d_clamped();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*0049be80*/ extern int fx_system_reset_positions();
/*004b3284*/ extern double crt_acos_body(double p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*005dd364*/ extern float k_f_n1;
/*005ddad4*/ extern float k_f_90;
/*005e64a4*/ extern float k_f_0;
/*005debe8*/ extern float k_f_0_075;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*006a5ddc*/ extern undefined g_weapon_rocket_6a5ddc;
/*006a5de0*/ extern undefined g_weapon_rocket_6a5de0;
/*006a5de4*/ extern undefined g_weapon_rocket_6a5de4;
/*006a5de8*/ extern undefined g_weapon_rocket_6a5de8;
/*006a5dec*/ extern undefined g_weapon_rocket_6a5dec;
/*006a5df0*/ extern undefined g_weapon_rocket_6a5df0;
/*006a5df4*/ extern undefined g_weapon_rocket_6a5df4;
/*006a5df8*/ extern undefined g_weapon_rocket_6a5df8;
/*006a5dfc*/ extern undefined g_weapon_rocket_6a5dfc;
/*006a5e00*/ extern undefined g_weapon_rocket_6a5e00;
/*006a5e04*/ extern undefined g_weapon_rocket_6a5e04;
/*006a5e08*/ extern undefined g_weapon_rocket_6a5e08;
/*006a5e0c*/ extern undefined g_weapon_rocket_6a5e0c;
/*006a5e10*/ extern undefined g_weapon_rocket_6a5e10;
/*006a5e14*/ extern undefined g_weapon_rocket_6a5e14;
/*006a5e18*/ extern undefined g_weapon_rocket_6a5e18;
/*006a5e1c*/ extern undefined g_weapon_rocket_6a5e1c;
/*006a5e20*/ extern undefined g_rockets;
/*006a5e24*/ extern undefined g_weapon_rocket_6a5e24;
/*006a5e28*/ extern undefined g_weapon_rocket_6a5e28;
/*006a5e2c*/ extern undefined4 g_weapon_6a5e2c;
/*006a5e30*/ extern undefined g_weapon_rocket_6a5e30;
/*006a5e38*/ extern undefined4 g_weapon_rocket_6a5e38;
/*006a5ff8*/ extern undefined g_weapon_6a5ff8;
/*006a6000*/ extern undefined g_rocket_6a6000;
/*006a600c*/ extern undefined g_weapon_rocket_6a600c;
/*006a6010*/ extern undefined g_weapon_rocket_6a6010;
/*006a6018*/ extern undefined g_weapon_rocket_6a6018;

/* per player block in the g_weapon_* globals is PLAYER_STRIDE bytes (inferred); rocket records are ROCKET_RECORD_SIZE bytes, ROCKET_RECORD_WORDS as undefined4 index (inferred) */
#define PLAYER_STRIDE (0x2c)
#define ROCKET_RECORD_SIZE (0x6c)
#define ROCKET_RECORD_WORDS (27)
#define FLOAT_HALF_BITS (0x3f000000) /* 0.5f bit pattern (inferred: stored into the per player block) */
#define FX_INDEX_NONE (-1) /* (inferred) "no fx / no value" marker */
#define SFX_ROCKET_VOICE_1 (0x12) /* voice id (inferred role: launch sound) */
#define SFX_ROCKET_VOICE_2 (0x14) /* voice id (inferred role: second launch sound) */
/* not exactly 180/pi (57.29577951); this is the literal in the binary */
#define DEG_PER_RAD_APPROX (57.29577791868205)

/* weapon_rocket_fire */


/* Fires one rocket for the player owning `slot` (inferred, from the rocket record writes).
 * slot: WeaponSlot of the firing player.
 * event_type: only 2 does anything (inferred: fire event), other values return at once.
 * Takes one from the player's rocket count (g_weapon_6a5ff8 + player_off, inferred), fills the rocket record
 * at rocket_index = remaining count + player (inferred): spawn position from the player's LTM, angles from
 * slot->aim_vector against the g_weapon_62ac80 / g_car_62ac68 / g_car_62ac74 directions, then moves the rocket's
 * clump into the course world and plays two launch voices. Returns nothing. */
void weapon_rocket_fire(WeaponSlot *slot,int event_type)

{
  RwV3d *spawn_pos;
  float angle_a;
  uint player;
  double ninety;
  int value_b;
  undefined4 fx_index;
  RwMatrix *ltm;
  RpWorld *world;
  RpWorld *course_world;
  int rocket_index;
  int rocket_off;
  int player_off;
  float *aim_dir;
  double scratch;
  double angle_b;
  RwReal speed;
  uint player_id;
  RpClump *clump;

  /* player block offset; the record call's result is unused here */
  player = slot->player;
  player_off = player * PLAYER_STRIDE;
  weapon_effect_get_slot_record(player);
  /* only the fire event (2) continues */
  if (event_type != 2) {
    return;
  }
  /* no rockets left for this player: nothing to fire */
  if (*(int *)((char *)&g_weapon_6a5ff8 + player_off) < 1) {
    return;
  }
  rocket_index = *(int *)((char *)&g_weapon_6a5ff8 + player_off) + -1;
  *(int *)((char *)&g_weapon_6a5ff8 + player_off) = rocket_index;
  rocket_index = rocket_index + player;
  rocket_off = rocket_index * ROCKET_RECORD_SIZE;
  *(undefined4 *)((char *)&g_weapon_rocket_6a600c + player_off) = 1;
  *(undefined4 *)((char *)&g_rockets + rocket_off) = 1;
  *(undefined4 *)((char *)&g_weapon_rocket_6a6018 + player_off) = FLOAT_HALF_BITS;
  if (*(int *)((char *)&g_rocket_6a6000 + player_off) == 0) {
    value_b = FX_INDEX_NONE;
    *(undefined4 *)((char *)&g_weapon_rocket_6a5e28 + rocket_off) = FX_INDEX_NONE;
  }
  else {
    value_b = weapon_effect_get_slot_value_b(slot->player);
    if (value_b == -1) {
      player_id = player;
      fx_index = weapon_effect_get_slot_value_a(slot->player);
      fx_index = fx_find_index_by_rwp_id(fx_index,player_id);
      *(undefined4 *)((char *)&g_weapon_rocket_6a5e28 + rocket_off) = fx_index;
      *(undefined4 *)((char *)&g_weapon_rocket_6a5e24 + rocket_off) = FX_INDEX_NONE;
      goto LAB_00460b34;
    }
    *(undefined4 *)((char *)&g_weapon_rocket_6a5e28 + rocket_off) = FX_INDEX_NONE;
  }
  *(int *)((char *)&g_weapon_rocket_6a5e24 + rocket_off) = value_b;
LAB_00460b34:
  /* spawn_pos = translation of the LTM of the frame at (*(g_weapon_rocket_6a6010 + player_off) + 4) (inferred) */
  spawn_pos = (RwV3d *)((char *)&g_weapon_rocket_6a5ddc + rocket_off);
  ltm =RwFrameGetLTM(*(RwFrame **)(*(char **)((char *)&g_weapon_rocket_6a6010 + player_off) + 4));
  spawn_pos->x = (ltm->pos).x;
  *(RwReal *)((char *)&g_weapon_rocket_6a5de0 + rocket_off) = (ltm->pos).y;
  *(RwReal *)((char *)&g_weapon_rocket_6a5de4 + rocket_off) = (ltm->pos).z;
  RwFrameTranslate(*(RwFrame **)((&g_weapon_6a5e2c)[rocket_index * ROCKET_RECORD_WORDS] + 4),spawn_pos,rwCOMBINEREPLACE);
  *(undefined4 *)((char *)&g_weapon_rocket_6a5de8 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5df8 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5df4 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5df0 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5dec + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5dfc + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5e04 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5e08 + rocket_off) = 0;
  /* speed = length of the 3 floats at slot +0x90 (inferred: velocity), stored with a small bias (k_f_0_075) */
  speed = RwV3dLength((RwV3d *)&slot->field_0x90);
  aim_dir = slot->aim_vector;
  *(float *)((char *)&g_weapon_rocket_6a5e00 + rocket_off) = speed + k_f_0_075;
  *(uint *)((char *)&g_weapon_rocket_6a5e0c + rocket_off) = player;
  /* angle_a: acos of the aim dot product, in degrees; negated when aim_dir has a negative dot with g_car_62ac68 (inferred) */
  scratch = (double)dot_product_3d_clamped(&g_weapon_62ac80,aim_dir);
  scratch = (double)crt_acos_body((double)scratch);
  angle_a = (float)(scratch * (double)(DEG_PER_RAD_APPROX));
  scratch = (double)dot_product_3d_clamped(&g_car_62ac68,aim_dir);
  if (scratch < (double)k_f_0) {
    angle_a = angle_a * k_f_n1;
  }
  *(float *)((char *)&g_weapon_rocket_6a5e18 + rocket_off) = angle_a;
  /* angle_b: acos of the second aim dot product, in radians, converted to degrees minus 90 below (inferred) */
  scratch = (double)dot_product_3d_clamped(&g_car_62ac74,aim_dir);
  angle_b = (double)crt_acos_body((double)scratch);
  scratch = (double)(DEG_PER_RAD_APPROX);
  fx_index = (&g_weapon_6a5e2c)[rocket_index * ROCKET_RECORD_WORDS];
  ninety = (double)k_f_90;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5e14 + rocket_off) = 0;
  *(undefined4 *)((char *)&g_weapon_rocket_6a5e10 + rocket_off) = 0;
  *(float *)((char *)&g_weapon_rocket_6a5e1c + rocket_off) = (float)(angle_b * scratch - ninety);
  *(undefined4 *)((char *)&g_weapon_rocket_6a5e30 + rocket_off) = 1;
  /* fx_index holds the rocket's clump here (reused local). Move the clump into the course world if it is not already there */
  world = (RpWorld *)clump_get_world((RpClump *)fx_index);
  course_world = (RpWorld *)course_get_rpworld_00467e90();
  if (world != course_world) {
    if (world != (RpWorld *)0x0) {
      RpWorldRemoveClump(world,(RpClump *)(&g_weapon_6a5e2c)[rocket_index * ROCKET_RECORD_WORDS]);
    }
    clump = (RpClump *)(&g_weapon_6a5e2c)[rocket_index * ROCKET_RECORD_WORDS];
    world = (RpWorld *)course_get_rpworld_00467e90();
    RpWorldAddClump(world,clump);
  }
  /* set atomics flag 1 on the rocket clump (inferred: visible), reset its fx positions, play two launch voices at spawn_pos */
  clump_set_flag_on_atomics((RpClump *)(&g_weapon_6a5e2c)[rocket_index * ROCKET_RECORD_WORDS], 0x1);
  fx_system_reset_positions((&g_weapon_rocket_6a5e38)[rocket_index * ROCKET_RECORD_WORDS],spawn_pos);
  sfx_play_voice_at(SFX_ROCKET_VOICE_1,spawn_pos,*(undefined4 *)((char *)&g_weapon_rocket_6a5e0c + rocket_off));
  sfx_play_voice_at(SFX_ROCKET_VOICE_2,spawn_pos,*(undefined4 *)((char *)&g_weapon_rocket_6a5e0c + rocket_off));
  weapon_effect_slot_clear(slot->player);
  return;
}


