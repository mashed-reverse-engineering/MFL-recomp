// cluster-entries: weapon_shotgun_slots_release
/* Installed cluster weapon_shotgun_slots_release (tools/cluster_install.py from output/cluster_pilot_te5/weapon_shotgun_slots_release.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

#define SHOTGUN_SLOT_COUNT 4

/* one shotgun effect slot (stride 0x24) */
typedef struct ShotgunSlot {
  unsigned char unknown_00[0x24];
} ShotgunSlot;
/*006aab80*/ extern ShotgunSlot g_weapon_shotgun_6aab80[SHOTGUN_SLOT_COUNT];

static void weapon_fx_clump_remove_c_r(void * p0);
extern void weapon_fx_clump_remove_c(void);
/* weapon_shotgun_slots_release: race start (weapon_shotgun_race_start jumps here), releases the clump of every shotgun effect slot (called through the 5 byte jump weapon_shotgun_race_start; the body sits behind weapon_shotgun_update) */
void weapon_shotgun_slots_release(void)
{
  int slot;

  for (slot = 0; slot < SHOTGUN_SLOT_COUNT; slot++) {
    weapon_fx_clump_remove_c_r(&g_weapon_shotgun_6aab80[slot]);
  }
}

/* weapon_fx_clump_remove_c_r: C -> original weapon_fx_clump_remove_c (registers esi) */
__declspec(naked) void weapon_fx_clump_remove_c_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_c
        pop esi
        ret
    }
}
