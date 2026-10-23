#include "mfl_tier2.h"
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*004220f0*/ extern int vehicle_get_position_158();
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*00437f50*/ extern int screen_shake_set();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004726a0*/ extern int sfx_play_at();
/*00485660*/ extern int particle_pool_find_oldest_or_free();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse();
/*004a1680*/ extern int player_rumble();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4720*/ extern void FUN_004c4720(void *p0, void *p1, void *p2, undefined4 p3);
/*004c4c30*/ extern int FUN_004c4c30(undefined4 p0, void *p1, void *p2);
/*004c51a0*/ extern void FUN_004c51a0(void *p0, undefined4 p1, void *p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*00569110*/ extern RwBool __cdecl RtLineSphereIntersectionTest(void * p0, void * p1, void * p2);
/*005dd40c*/ extern float k_f_1_5;
/*005ddafc*/ extern float k_f_0_05;
/*005de1d0*/ extern float k_f_1_25;
/*005de3e4*/ extern float k_f_40;
/*0062ac74*/ extern float g_car_62ac74;

#define SFX_SHOTGUN_FIRE (0x15) /* inferred: sfx id played at the owner car */
#define SHOTGUN_FIELD_COOLDOWN (3) /* index into the weapon state (inferred) */
#define SHOTGUN_FIELD_AMMO (4) /* index into the weapon state (inferred) */
#define SHOTGUN_FIELD_OWNER (5) /* index into the weapon state: owner car pointer (inferred) */
#define SHOTGUN_COOLDOWN (8) /* value written to the cooldown field after a shot (inferred) */
#define SHOTGUN_SPARE_BITS (0x3e800000) /* 0.25f, stored in a local that is never read */
#define MUZZLE_COUNT (2) /* two muzzle frames per shot (inferred) */
#define MUZZLE_FRAME_OFFSET (4) /* offset of the RwFrame pointer in a muzzle object (inferred) */
#define SPARK_FX_SCALE_MIN_BITS (0x3f666666) /* 0.9f */
#define SPARK_FX_SCALE_MAX_BITS (0x3f8ccccd) /* 1.1f */
#define MUZZLE_FX_SIZE_BITS (0x42b40000) /* 90.0f, third argument of fx_emitter_spawn_b_default (inferred) */
#define SPARK_ALPHA (0x80) /* alpha byte of the spark colour (inferred) */
#define SHOTGUN_DAMAGE_BITS (0x41000000) /* 8.0f, damage passed to car_damage_at_world_point */
#define CAR_POSITION_OFFSET (0x30) /* car field: world position (inferred) */
#define CAR_SLOT_OFFSET (0xb0) /* car field: race slot index (inferred) */
#define RACE_SLOT_COUNT (4) /* car slots scanned for hits */
#define LOCAL_PLAYER_SLOT (1) /* slot that gets screen shake (inferred) */
#define SCREEN_SHAKE_AMOUNT (10) /* (inferred) */
#define RUMBLE_KIND (2) /* (inferred) */
#define RUMBLE_FRAMES (0xf) /* (inferred) */
#define RUMBLE_STRENGTH (0xff) /* (inferred) */

/* weapon_shotgun_fire_shot */

/* "one shotgun blast: sfx, ray (line-sphere test) against the cars, rwp impulse and damage" */
/*
 * Fires one shotgun blast for the weapon `weapon` (int array, owned by a car):
 *   weapon[1], weapon[2]  muzzle objects (RwFrame pointer at +4), one per barrel (inferred)
 *   weapon[3]             cooldown, set to SHOTGUN_COOLDOWN after the shot
 *   weapon[4]             ammo, decremented once per shot
 *   weapon[5]             owner car; its slot (+0xb0) is skipped as a target
 * For each muzzle: plays the sound, spawns the muzzle flash, and asks FUN_004c4c30 for a
 * world hit along the muzzle direction. A hit spawns sparks. Then every other active car
 * slot is tested against the ray and hit cars get the rwp impulse and damage.
 * Returns nothing.
 */

void weapon_shotgun_fire_shot(int *weapon)

{
  float fx_scale;
  double spark_dist;
  RwMatrix *muzzle_ltm;
  undefined4 weapon_rw;
  int slot_or_result; /* muzzle hit flag before the slot loop, then the car slot index */
  int active_or_hit; /* slot active flag, then the ray hit flag */
  int *muzzle_entry;
  double rand_scale;
  RwV3d *muzzle_pos_ptr;
  float *hit_normal_ptr;
  struct {
    undefined1 spark_r; /* spark colour bytes: r, g, b, a */
    undefined1 spark_g;
    undefined1 spark_b;
    undefined1 spark_a;
    float spark_pos_x; /* spark position = muzzle + dir * hit_dist * 1.5 + hit_normal * 0.05 */
    float spark_pos_y;
    float spark_pos_z;
    RwV3d muzzle_dir; /* normalized muzzle forward vector */
    RwV3d muzzle_pos; /* muzzle world position */
    float far_pos_x; /* muzzle + dir * 1.5 */
    float far_pos_y;
    float far_pos_z;
    float impulse_x; /* impulse direction: dir * 40, y negated */
    float impulse_y;
    float impulse_z;
    int muzzles_left; /* muzzle loop counter */
    float flash_pos_x; /* muzzle + dir: muzzle flash position */
    float flash_pos_y;
    float flash_pos_z;
    undefined4 spare_b0; /* dead store, never read */
    RwV3d impact_scale; /* spark scale (1.5, 1.5, 1.0) */
    undefined1 hit_t [4]; /* out value of RtLineSphereIntersectionTest */
    undefined1 car_pos [12]; /* car centre from vehicle_get_position_158 */
    float car_sphere_radius; /* scaled by 1.25 but never passed on (inferred, check disassembly) */
    undefined1 impulse_offset [12]; /* from FUN_004c4720(muzzle_pos, far_pos) */
    float hit_normal_x; /* surface normal written by FUN_004c4c30 (inferred) */
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_74[4];
    undefined1 spark_scratch [40]; /* scratch passed to FUN_004c51a0 (inferred) */
    float hit_dist; /* distance to the world hit (inferred) */
    undefined1 pad_44[4];
    RwMatrix spark_matrix; /* spark transform */
  } shot;

  sfx_play_at(SFX_SHOTGUN_FIRE,weapon[SHOTGUN_FIELD_OWNER] + CAR_POSITION_OFFSET);
  weapon[SHOTGUN_FIELD_AMMO] = weapon[SHOTGUN_FIELD_AMMO] + -1;
  weapon[SHOTGUN_FIELD_COOLDOWN] = SHOTGUN_COOLDOWN;
  shot.spare_b0 = SHOTGUN_SPARE_BITS;
  shot.muzzles_left = MUZZLE_COUNT;
  /* one pass per muzzle: origin, direction, muzzle flash, world hit */
  muzzle_entry = weapon;
  do {
    muzzle_entry = muzzle_entry + 1;
    muzzle_ltm = RwFrameGetLTM(*(RwFrame **)(*muzzle_entry + MUZZLE_FRAME_OFFSET));
    shot.muzzle_pos.x = (muzzle_ltm->pos).x;
    shot.muzzle_pos.y = (muzzle_ltm->pos).y;
    shot.muzzle_pos.z = (muzzle_ltm->pos).z;
    shot.far_pos_x = shot.muzzle_pos.x;
    shot.far_pos_y = shot.muzzle_pos.y;
    shot.far_pos_z = shot.muzzle_pos.z;
    shot.flash_pos_x = shot.muzzle_pos.x;
    shot.flash_pos_y = shot.muzzle_pos.y;
    shot.flash_pos_z = shot.muzzle_pos.z;
    RwV3dNormalize(&shot.muzzle_dir,&muzzle_ltm->at);
    shot.flash_pos_x = shot.flash_pos_x + shot.muzzle_dir.x;
    shot.impulse_x = shot.muzzle_dir.x;
    shot.flash_pos_y = shot.flash_pos_y + shot.muzzle_dir.y;
    shot.impulse_y = shot.muzzle_dir.y;
    shot.impulse_z = shot.muzzle_dir.z;
    shot.flash_pos_z = shot.flash_pos_z + shot.muzzle_dir.z;
    shot.far_pos_x = shot.muzzle_dir.x * k_f_1_5 + shot.far_pos_x;
    shot.far_pos_y = shot.muzzle_dir.y * k_f_1_5 + shot.far_pos_y;
    shot.far_pos_z = shot.muzzle_dir.z * k_f_1_5 + shot.far_pos_z;
    /* muzzle flash, scale jittered between 0.9 and 1.1 */
    rand_scale = (double)rp_random_float_range(SPARK_FX_SCALE_MIN_BITS,SPARK_FX_SCALE_MAX_BITS);
    fx_emitter_spawn_b_default(&shot.flash_pos_x,&g_car_62ac74,MUZZLE_FX_SIZE_BITS,(fx_scale = (float)rand_scale, *(int *)&fx_scale));
    hit_normal_ptr = &shot.hit_normal_x;
    muzzle_pos_ptr = &shot.muzzle_pos;
    weapon_rw = weapon_get_rw_801b14();
    slot_or_result = FUN_004c4c30(weapon_rw,muzzle_pos_ptr,hit_normal_ptr);
    /* world hit along the muzzle: spawn sparks at the hit point */
    if (slot_or_result != 0) {
      spark_dist = (double)shot.hit_dist * (double)k_f_1_5;
      shot.spark_r = 0;
      shot.spark_g = 0;
      shot.spark_b = 0;
      shot.spark_a = SPARK_ALPHA;
      shot.spark_pos_x = shot.muzzle_pos.x;
      shot.spark_pos_y = shot.muzzle_pos.y;
      shot.spark_pos_z = shot.muzzle_pos.z;
      shot.spark_pos_x = (float)(spark_dist * (double)shot.muzzle_dir.x + (double)shot.spark_pos_x);
      shot.spark_pos_y = (float)(spark_dist * (double)shot.muzzle_dir.y + (double)shot.spark_pos_y);
      shot.spark_pos_z = (float)(spark_dist * (double)shot.muzzle_dir.z + (double)shot.spark_pos_z);
      shot.spark_pos_x = (float)((double)shot.hit_normal_x * (double)k_f_0_05 + (double)shot.spark_pos_x);
      shot.spark_pos_y = (float)((double)shot.hit_normal_y * (double)k_f_0_05 + (double)shot.spark_pos_y);
      shot.spark_pos_z = (float)((double)shot.hit_normal_z * (double)k_f_0_05 + (double)shot.spark_pos_z);
      FUN_004c51a0(&shot.spark_matrix,(undefined4)shot.spark_scratch,&shot.spark_pos_x);
      shot.impact_scale.y = 1.5;
      shot.impact_scale.x = 1.5;
      shot.impact_scale.z = 1.0;
      RwMatrixScale(&shot.spark_matrix,&shot.impact_scale,rwCOMBINEPRECONCAT);
      particle_pool_find_oldest_or_free(1,&shot.spark_matrix,&shot.spark_r);
    }
    /* impulse direction for this muzzle, scaled by 40 */
    slot_or_result = 0;
    shot.impulse_x = shot.impulse_x * k_f_40;
    shot.impulse_z = shot.impulse_z * k_f_40;
    shot.impulse_y = -(shot.impulse_y * k_f_40);
    /* car loop: every active slot except the owner's is tested against the ray */
    do {
      active_or_hit = race_setup_slot_used(slot_or_result);
      if ((active_or_hit != 0) && (*(int *)(weapon[SHOTGUN_FIELD_OWNER] + CAR_SLOT_OFFSET) != slot_or_result)) {
        vehicle_get_position_158(slot_or_result,shot.car_pos);
        shot.car_sphere_radius = shot.car_sphere_radius * k_f_1_25;
        active_or_hit = RtLineSphereIntersectionTest(&shot.muzzle_pos,shot.car_pos,shot.hit_t);
        /* hit car: rwp mode, impulse, damage; the local player gets screen shake */
        if (active_or_hit != 0) {
          car_enter_rwp_mode(slot_or_result);
          FUN_004c4720(shot.impulse_offset,&shot.muzzle_pos,&shot.far_pos_x,0);
          car_apply_rwp_impulse(slot_or_result,&shot.impulse_x,shot.impulse_offset);
          car_damage_at_world_point(slot_or_result,SHOTGUN_DAMAGE_BITS,&shot.muzzle_pos);
          if (slot_or_result == LOCAL_PLAYER_SLOT) {
            screen_shake_set(SCREEN_SHAKE_AMOUNT);
          }
          player_rumble(slot_or_result,RUMBLE_KIND,RUMBLE_FRAMES,RUMBLE_STRENGTH);
        }
      }
      slot_or_result = slot_or_result + 1;
    } while (slot_or_result < RACE_SLOT_COUNT);
    shot.muzzles_left = shot.muzzles_left + -1;
  } while (shot.muzzles_left != 0);
  return;
}


