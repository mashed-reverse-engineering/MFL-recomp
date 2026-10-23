#include "mfl_tier2.h"
typedef int WeaponId;
/*004353c0*/ extern int menu_powerup_set_for_event();
/*00464b30*/ extern int powerup_random_weapon_type();
/*00464c00*/ extern int weapon_powerup_set_icon_texture();
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*005dd61c*/ extern float k_f_0_001;
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a93d0*/ extern undefined4 g_powerup_count;

#define POWERUP_VARIANT_CHAOS (2)
#define POWERUP_STATE_AVAILABLE (1)
#define POWERUP_SPECIAL_RANDOM (1)
#define DEFAULT_RESPAWN_TIME (120.0)

/* powerup_place */

/* Adds a powerup of powerup_type at position (float[3]: x, y, z) in the next free slot of g_powerups.
   Returns the new slot index, or -1 if rejected: table full (max 25 slots), an existing powerup
   within the k_f_0_001 squared distance (inferred threshold), or WEAPON_CHAOS (0x15) outside
   POWERUP_VARIANT_CHAOS. */

int powerup_place(float *position,int powerup_type)

{
  struct Powerup *powerup;
  float fVar1;
  int slot;
  int iVar3;
  WeaponId random_weapon;
  float *other_pos_y;
  int scan_index;
  Powerup *new_powerup;

  iVar3 = menu_powerup_set_for_event();
  slot = g_powerup_count;
  /* table full */
  if (0x18 < (int)g_powerup_count) {
    return -1;
  }
  /* reject if too close to any existing powerup, scanning from the last slot down to 0 */
  if (g_powerup_count != 0) {
    other_pos_y = g_powerups[g_powerup_count].position + 1;
    scan_index = g_powerup_count;
    do {
      scan_index = scan_index + -1;
      fVar1 = other_pos_y[-0x14] - position[1];
      if ((other_pos_y[-0x13] - position[2]) * (other_pos_y[-0x13] - position[2]) +
          (other_pos_y[-0x15] - *position) * (other_pos_y[-0x15] - *position) + fVar1 * fVar1 < k_f_0_001) {
        return -1;
      }
      other_pos_y = other_pos_y + -0x14;
    } while (scan_index != 0);
  }
  /* claim the next free slot (inferred: g_powerup_count is the first unused index) */
  new_powerup = g_powerups + g_powerup_count;
  g_powerups[g_powerup_count].special = 0;
  powerup = &g_powerups[slot];
  powerup->type = powerup_type;
  /* variant 2: chaos type becomes a random weapon and is flagged special; chaos is refused otherwise */
  if (iVar3 == POWERUP_VARIANT_CHAOS) {
    if (powerup_type != WEAPON_CHAOS) {
      return -1;
    }
    random_weapon = powerup_random_weapon_type();
    powerup->type = random_weapon;
    powerup->special = POWERUP_SPECIAL_RANDOM;
  }
  else if (powerup_type == WEAPON_CHAOS) {
    return -1;
  }
  /* icon and frame reset (inferred from the callee names) */
  weapon_powerup_set_icon_texture(new_powerup);
  RwFrameSetIdentity(*(RwFrame **)(new_powerup->clump + 4));
  /* position and spawn position */
  powerup->position[0] = *position;
  powerup->position[1] = position[1];
  powerup->position[2] = position[2];
  powerup->spawn_position[0] = *position;
  powerup->spawn_position[1] = position[1];
  iVar3 = g_powerup_count;
  fVar1 = position[2];
  g_powerup_count = g_powerup_count + 1;
  powerup->state = POWERUP_STATE_AVAILABLE;
  powerup->respawn_time = DEFAULT_RESPAWN_TIME;
  powerup->respawn_timer = 0.0;
  powerup->spawn_position[2] = fVar1;
  return iVar3;
}


