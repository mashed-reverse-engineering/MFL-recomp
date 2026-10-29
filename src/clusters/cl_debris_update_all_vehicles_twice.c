// cluster-entries: debris_update_all_vehicles_twice
/* Installed cluster debris_update_all_vehicles_twice (tools/cluster_install.py from output/cluster_pilot_zx/debris_update_all_vehicles_twice.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0065c0d0*/ extern undefined g_game_65c0d0;
extern char g_game_65c8f0[];

/* Size in bytes of one record (inferred from the loop stride); the same table is walked as debris banks (2 blocks of 10 slots) by debris_slot_release_by_clump */
#define VEHICLE_RECORD_SIZE (0x208)


/* debris_update_all_vehicles_twice */

/* "loops vehicle records calling debris_update_twice"
 * Runs debris_update_twice once per vehicle record (via debris_update_twice, which passes
 * the record pointer in eax), from g_game_65c0d0 up to g_game_65c8f0 (inferred).
 * Takes no parameters and returns nothing.
 */

static int debris_update_twice_r(int p0);
extern void debris_update_twice(void);
void debris_update_all_vehicles_twice(void)

{
  undefined *vehicle;

  /* Walk the vehicle records, one VEHICLE_RECORD_SIZE step at a time */
  vehicle = &g_game_65c0d0;
  do {
    debris_update_twice_r(vehicle);
    vehicle = vehicle + VEHICLE_RECORD_SIZE;
  } while ((LONG_PTR)vehicle < ((LONG_PTR)&g_game_65c8f0));
  return;
}

/* debris_update_twice_r: C -> original debris_update_twice (registers eax) */
__declspec(naked) int debris_update_twice_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_update_twice
        ret
    }
}
