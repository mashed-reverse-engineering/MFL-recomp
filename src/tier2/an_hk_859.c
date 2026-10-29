#include "mfl_tier2.h"
/*00465f60*/ extern char *weapon_effect_get_slot_record();
/*004726a0*/ extern int sfx_play_at();
/*00481fd0*/ extern double random_range();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd370*/ extern float k_f_0_3;
/*005dd378*/ extern float k_f_0_1;
/*005ddb20*/ extern float k_f_0_9;
/*005e64a4*/ extern float k_f_0;
/*005de194*/ extern float k_f_0_13;
/*006a29bc*/ extern undefined4 g_weapon_mortar_6a29bc;
/*006a29c4*/ extern undefined g_weapon_mortar_6a29c4;
/*006a29c8*/ extern undefined4 g_weapon_6a29c8;
/*006a29d0*/ extern undefined4 g_weapon_6a29d0;
/*006a2a28*/ extern undefined g_weapon_6a2a28;

#define SHOT_SLOT_DWORDS (0x44) /* one shot record is 0x110 bytes */
#define SHOT_POOL_BYTES (0x2200) /* shot pool is 32 records */
#define PLAYER_RECORD_BYTES (0x1c) /* per player weapon record stride (bytes) */
#define PLAYER_RECORD_DWORDS (7) /* same stride in dwords */
#define NO_TARGET_ID (-1) /* shot target id when the mortar has no homing target */
#define SHOT_ACTIVE (1) /* dword index: non-zero while the shot is in use (inferred) */
#define SHOT_HOMING (0x11) /* dword index: 1 = fired at a target, 0 = fired along aim (inferred) */
#define SHOT_TIMER (0x13) /* dword index: float timer, 0.05f at launch (inferred) */
#define TRAIL_FIRST_DWORD (0x17) /* dword index of the position trail */
#define TRAIL_LENGTH (15) /* trail points written at launch */
#define SFX_MORTAR_FIRE_A (0x12) /* sound id (inferred) */
#define SFX_MORTAR_FIRE_B (0x1e) /* sound id (inferred) */

/* weapon_mortar_fire
 * Fires one mortar shell for the player's mortar weapon (inferred from the WEAPON_MORTAR global names).
 * weapon: the player's weapon slot; its player index selects the per player records.
 * action: 1 = fire pressed; any other value does nothing.
 * Takes a free shot record from the pool, spends one round, starts the 0.2 s fire cooldown and
 * fills the shot: origin at the launcher's frame position, velocity, a 15 point trail, then two sounds.
 * Returns nothing.
 */

void weapon_mortar_fire(WeaponSlot *weapon,int action)

{
  float *shot_f;
  float *origin;
  RwV3d *spread;
  float vel_z;
  uint player;
  undefined4 *slot_scan;
  RwMatrix *muzzle_ltm;
  float *trail_cursor;
  int ammo_left;
  undefined4 *shot;
  double rnd;
  RwV3d to_target;

  player = weapon->player;
  if (((action == 1) && (-1 < *(int *)((char *)&g_weapon_mortar_6a29c4 + player * PLAYER_RECORD_BYTES))) &&
     !(*(float *)&(&g_weapon_6a29c8)[player * PLAYER_RECORD_DWORDS] > k_f_0)) {      /* NaN cooldown fires (original: test ah,0x41; je) */
    /* find the first free shot record in the pool; shot is NULL if the pool is full */
    slot_scan = (undefined4 *)&g_weapon_6a2a28;
    do {
      shot = slot_scan;
      if (slot_scan[1] == 0) break;
      slot_scan = slot_scan + SHOT_SLOT_DWORDS;
      shot = (undefined4 *)0x0;
    } while ((char *)slot_scan < (char *)&g_weapon_6a2a28 + SHOT_POOL_BYTES);
    ammo_left = *(int *)((char *)&g_weapon_mortar_6a29c4 + player * PLAYER_RECORD_BYTES) + -1;
    *(int *)((char *)&g_weapon_mortar_6a29c4 + player * PLAYER_RECORD_BYTES) = ammo_left;
    (&g_weapon_6a29c8)[player * PLAYER_RECORD_DWORDS] = 0x3e4ccccd;
    shot_f = (float *)shot;
    if (-1 < ammo_left) {
      shot[0x14] = 0;
      shot_f[0xe] = 0;
      shot_f[0xf] = 0;
      shot_f[0x10] = 0;
      ammo_left = (&g_weapon_6a29d0)[player * PLAYER_RECORD_DWORDS];
      shot[SHOT_ACTIVE] = 1;
      *(undefined4 *)&shot_f[SHOT_TIMER] = 0x3d4ccccd;
      muzzle_ltm = RwFrameGetLTM(*(RwFrame **)(ammo_left + 4));
      origin = (float *)(shot + 5);
      *origin = (muzzle_ltm->pos).x;
      shot_f[6] = (muzzle_ltm->pos).y;
      shot_f[7] = (muzzle_ltm->pos).z;
      shot_f[0xb] = (muzzle_ltm->pos).x;
      shot_f[0xc] = (muzzle_ltm->pos).y;
      shot_f[0xd] = (muzzle_ltm->pos).z;
      ammo_left = (&g_weapon_mortar_6a29bc)[player * PLAYER_RECORD_DWORDS];
      shot_f[0x15] = shot_f[6];
      /* no homing target: shot follows the weapon aim vector plus the weapon's velocity */
      if (ammo_left == NO_TARGET_ID) {
        shot_f[8] = weapon->aim_vector[0] * k_f_0_13;
        shot_f[9] = weapon->aim_vector[1] * k_f_0_13;
        shot_f[10] = weapon->aim_vector[2] * k_f_0_13;
        shot_f[8] = *(float *)&weapon->field_0x90 * k_f_0_9 + shot_f[8];
        shot_f[9] = *(float *)&weapon->field_0x94 * k_f_0_9 + shot_f[9];
        vel_z = *(float *)&weapon->field_0x98;
        *shot = NO_TARGET_ID;
        vel_z = vel_z * k_f_0_9;
        shot[SHOT_HOMING] = 0;
        vel_z = vel_z + shot_f[10];
      }
      /* homing target: velocity points from the launcher to the target, plus a random spread */
      else {
        trail_cursor = (float *)weapon_effect_get_slot_record(weapon->player);
        to_target.x = *trail_cursor - *origin;
        to_target.y = trail_cursor[1] - shot_f[6];
        to_target.z = trail_cursor[2] - shot_f[7];
        RwV3dNormalize(&to_target,&to_target);
        spread = (RwV3d *)(shot + 2);
        rnd = (double)random_range(0xbf800000,0x3f800000);
        spread->x = (float)rnd;
        shot_f[3] = 0;
        rnd = (double)random_range(0xbf800000,0x3f800000);
        *(float *)&shot_f[4] = (float)rnd;
        RwV3dNormalize(spread,spread);
        vel_z = spread->x * k_f_0_3;
        *shot = (&g_weapon_mortar_6a29bc)[player * PLAYER_RECORD_DWORDS];
        shot[SHOT_HOMING] = 1;
        spread->x = vel_z;
        shot_f[3] = shot_f[3] * k_f_0_3;
        shot_f[4] = shot_f[4] * k_f_0_3;
        shot_f[8] = to_target.x * k_f_0_1;
        shot_f[9] = to_target.y * k_f_0_1;
        vel_z = to_target.z * k_f_0_1;
      }
      *(float *)&shot_f[10] = vel_z;
      /* fill the trail with the launch position (the loop reuses ammo_left as its counter) */
      ammo_left = TRAIL_LENGTH;
      trail_cursor = (float *)(shot + TRAIL_FIRST_DWORD);
      do {
        *trail_cursor = *origin;
        trail_cursor[1] = shot_f[6];
        ammo_left = ammo_left + -1;
        trail_cursor[2] = shot_f[7];
        trail_cursor = trail_cursor + 3;
      } while (ammo_left != 0);
      sfx_play_at(SFX_MORTAR_FIRE_A,origin);
      sfx_play_at(SFX_MORTAR_FIRE_B,origin);
    }
  }
  return;
}
