#include "mfl_tier2.h"
/*00463450*/ extern int weapon_oil_pick_slot_forward();
/*00467ea0*/ extern char *weapon_get_rw_801b14();
/*00467fd0*/ extern int __cdecl course_surface_id_is_active_check(undefined4 p0);
/*00472890*/ extern int sfx_play_voice_at();
/*00481fd0*/ extern double random_range();
/*004c4720*/ extern void FUN_004c4720(void *p0, void *p1, void *p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(void *p0, undefined4 p1, void *p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd378*/ extern float k_f_0_1;
/*005de2fc*/ extern float k_f_0_04;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*006a7c78*/ extern undefined g_weapon_6a7c78;
/*006a7cb8*/ extern undefined g_weapon_6a7cb8;
/*006a7cbc*/ extern undefined g_weapon_6a7cbc;
/*006a7cc0*/ extern undefined g_weapon_oil_6a7cc0;

#define PLAYER_VEC3_STRIDE (0xc) /* bytes per player in the g_weapon_6a7cb8/bc/c0 arrays (3 floats: last muzzle position) */
#define PLAYER_COUNTER_STRIDE (0x10) /* bytes per player in g_weapon_6a7c78 (float counter, inferred: fire budget) */
#define SFX_VOICE_OIL_FIRE (0x17) /* voice id passed to sfx_play_voice_at (inferred) */
#define RANDOM_ANGLE_MAX_BITS (0x43b40000) /* 360.0f bit pattern, random_range takes float bits (inferred) */
#define JITTER_MIN_BITS (0xbe4ccccd) /* -0.2f bit pattern */
#define JITTER_MAX_BITS (0x3e4ccccd) /* 0.2f bit pattern */
/* weapon_oil_fire */


/*
 * Oil weapon fire emitter for one weapon slot (inferred from the body).
 * slot: the WeaponSlot; player, muzzle_position are read.
 * Runs only when the muzzle moved since the last sample of this player:
 * squared distance to the stored position >= k_f_0_1. Then the stored
 * position is updated, a ray goes from the muzzle down by float_1_0, and on a
 * hit whose surface is not active (course_surface_id_is_active_check == 0) a
 * fire effect is placed at the hit point (lifted by k_f_0_04 along the hit
 * normal) with a random yaw and horizontal jitter, and the oil fire voice
 * plays. The per-player counter drops by k_f_0_1 at the end of a moved step;
 * an active surface returns early and skips that decrement.
 * Returns nothing.
 */
void weapon_oil_fire(WeaponSlot *slot)

{
  uint player_index;
  float delta_x;
  float delta_z;
  float delta_y;
  int scratch_int; /* reused: player byte offset, then world_intersect_line / surface check result */
  char *world;
  double random_value;
  float *ray_start;
  float *muzzle_or_ray_end; /* reused: muzzle pointer, then the ray end point */
  RwOpCombineType combine_op;
  struct {
    float fStack_b4;
    float fStack_b0;
    float fStack_ac;
    undefined4 uStack_a8;
    float fStack_a4;
    float fStack_a0;
    float fStack_9c;
    float fStack_98;
    float fStack_94;
    float fStack_90;
  } ray;
  RwV3d jitter;
  struct {
    float fStack_80;
    float fStack_7c;
    float fStack_78;
    int iStack_74;
    undefined1 auStack_70 [36];
    char *iStack_4c;
    undefined4 uStack_48;
    char *iStack_44;
  } hit; /* world_intersect_line output: hit normal (fStack_80..78, inferred) and surface data */
  RwMatrix fire_matrix;

  player_index = slot->player;
  scratch_int = player_index * PLAYER_VEC3_STRIDE;
  delta_x = slot->muzzle_position[0] - *(float *)((char *)&g_weapon_6a7cb8 + scratch_int);
  muzzle_or_ray_end = slot->muzzle_position;
  delta_y = slot->muzzle_position[1] - *(float *)((char *)&g_weapon_6a7cbc + scratch_int);
  delta_z = slot->muzzle_position[2] - *(float *)((char *)&g_weapon_oil_6a7cc0 + scratch_int);
  /* Only act when the muzzle moved far enough since the last sample */
  if (!(delta_z * delta_z + delta_x * delta_x + delta_y * delta_y < k_f_0_1)) {      /* NaN enters the body in the original (test ah,5; jnp) */
    *(float *)((char *)&g_weapon_6a7cb8 + scratch_int) = *muzzle_or_ray_end;
    *(float *)((char *)&g_weapon_6a7cbc + scratch_int) = slot->muzzle_position[1];
    *(float *)((char *)&g_weapon_oil_6a7cc0 + scratch_int) = slot->muzzle_position[2];
    /* Ray from the muzzle straight down by float_1_0 */
    ray.fStack_a4 = *muzzle_or_ray_end;
    ray.fStack_a0 = slot->muzzle_position[1];
    ray.fStack_9c = slot->muzzle_position[2];
    ray.fStack_98 = *muzzle_or_ray_end;
    ray.fStack_94 = slot->muzzle_position[1] - float_1_0;
    ray.fStack_90 = slot->muzzle_position[2];
    muzzle_or_ray_end = &hit.fStack_80;
    ray_start = &ray.fStack_a4;
    world = weapon_get_rw_801b14();
    scratch_int = world_intersect_line((RpWorld *)world,(undefined4)ray_start,(undefined4)muzzle_or_ray_end);
    if (scratch_int != 0) {
      /* Surface check: an active surface returns early, no spawn and no counter change */
      ray.uStack_a8 = hit.uStack_48;
      scratch_int = course_surface_id_is_active_check
                        (*(undefined4 *)
                          (*(char **)(hit.iStack_44 + 0x10) +
                          ((uint)*(ushort *)(*(char **)(hit.iStack_4c + 4) + 6 + hit.iStack_74 * 8) +
                          (uint)*(ushort *)(hit.iStack_4c + 0x80)) * 4));
      if (scratch_int != 0) {
        return;
      }
      /* Spawn: lift the hit point along the normal, random yaw and jitter, then fire */
      FUN_004c4720(&ray.fStack_b4,&ray.fStack_a4,&ray.fStack_98,(undefined4)ray.uStack_a8);
      ray.fStack_b4 = hit.fStack_80 * k_f_0_04 + ray.fStack_b4;
      ray.fStack_b0 = hit.fStack_7c * k_f_0_04 + ray.fStack_b0;
      ray.fStack_ac = hit.fStack_78 * k_f_0_04 + ray.fStack_ac;
      FUN_004c51a0(&fire_matrix,(undefined4)hit.auStack_70,&ray.fStack_b4);
      combine_op = rwCOMBINEPRECONCAT;
      random_value = (double)random_range(0,RANDOM_ANGLE_MAX_BITS);
      RwMatrixRotate(&fire_matrix,(RwV3d *)&g_weapon_62ac80,(float)random_value,combine_op);
      random_value = (double)random_range(JITTER_MIN_BITS,JITTER_MAX_BITS);
      jitter.x = (RwReal)random_value;
      jitter.y = 0.0;
      random_value = (double)random_range(JITTER_MIN_BITS,JITTER_MAX_BITS);
      jitter.z = (RwReal)random_value;
      RwMatrixTranslate(&fire_matrix,&jitter,rwCOMBINEPOSTCONCAT);
      weapon_oil_pick_slot_forward(&fire_matrix);
      sfx_play_voice_at(SFX_VOICE_OIL_FIRE,&ray.fStack_b4,slot->player);
    }
    /* Per-player counter drops once per moved step */
    *(float *)((char *)&g_weapon_6a7c78 + player_index * PLAYER_COUNTER_STRIDE) =
         *(float *)((char *)&g_weapon_6a7c78 + player_index * PLAYER_COUNTER_STRIDE) - k_f_0_1;
  }
  return;
}


