#include "mfl_tier2.h"
/*0065c0d0*/ extern undefined g_game_65c0d0;
static int debris_half_update_visible_both_r(int reg_eax);
extern void debris_half_update_visible_both(void);
extern char g_game_65c8f0[];
#define DEBRIS_RECORD_SIZE (0x208)
__declspec(naked) int debris_half_update_visible_both_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_half_update_visible_both
        ret
    }
}

/* debris_update_visible_all_vehicles */

/* "loops 0x208 byte vehicle debris records, calls debris_half_update_visible_both"
 * Walks every DEBRIS_RECORD_SIZE-byte debris record from g_game_65c0d0 up to
 * g_game_65c8f0 and refreshes each one through debris_half_update_visible_both
 * (record pointer passed in eax). Takes no parameters, returns nothing.
 * (inferred from the name and the loop) */

void debris_update_visible_all_vehicles(void)

{
  undefined *debris_record;

  /* Loop over the debris record array, one record per iteration */
  debris_record = &g_game_65c0d0;
  do {
    debris_half_update_visible_both_r(debris_record);
    debris_record = debris_record + DEBRIS_RECORD_SIZE;
  } while ((LONG_PTR)debris_record < ((LONG_PTR)&g_game_65c8f0));
  return;
}


