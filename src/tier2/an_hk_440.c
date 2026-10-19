#include "mfl_tier2.h"
/*0065c0d0*/ extern undefined g_game_65c0d0;
static int debris_update_twice_r(int reg_eax);
extern void debris_update_twice(void);
extern char g_game_65c8f0[];

/* Size in bytes of one vehicle record (inferred from the loop stride) */
#define VEHICLE_RECORD_SIZE (0x208)

__declspec(naked) int debris_update_twice_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_update_twice
        ret
    }
}

/* debris_update_all_vehicles_twice */

/* "loops vehicle records calling debris_update_twice"
 * Runs debris_update_twice once per vehicle record (via debris_update_twice_r, which passes
 * the record pointer in eax), from g_game_65c0d0 up to g_game_65c8f0 (inferred).
 * Takes no parameters and returns nothing.
 */

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


