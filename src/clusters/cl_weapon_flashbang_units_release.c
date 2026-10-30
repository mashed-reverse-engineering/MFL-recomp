// cluster-entries: weapon_flashbang_units_release
/* Installed cluster weapon_flashbang_units_release (tools/cluster_install.py from output/cluster_pilot_te5/weapon_flashbang_units_release.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

#define FLASH_UNIT_COUNT 4
#define FLASH_SLOT_COUNT 4

/* a flash unit (stride 0x14: clump, ...) and a flash slot (stride 0x18: position, active flag, ...) */
typedef struct FlashUnit { unsigned char unknown_00[0x14]; } FlashUnit;
typedef struct FlashSlot { unsigned char unknown_00[0x18]; } FlashSlot;
/*006a5d18*/ extern FlashSlot g_weapon_6a5d18[FLASH_SLOT_COUNT];
/*006a5d78*/ extern FlashUnit g_weapon_6a5d78[FLASH_UNIT_COUNT];

static void weapon_fx_clump_remove_a_r(void * p0);
extern void weapon_fx_clump_remove_a(void);
static void flashunit_state_clear_r(void * p0);
extern void flashunit_state_clear(void);
/* weapon_flashbang_units_release: race start (weapon_flashbang_race_start jumps here): releases the clump of every flash unit and clears every flash slot */
void weapon_flashbang_units_release(void)
{
  int i;

  for (i = 0; i < FLASH_UNIT_COUNT; i++) {
    weapon_fx_clump_remove_a_r(&g_weapon_6a5d78[i]);
  }
  for (i = 0; i < FLASH_SLOT_COUNT; i++) {
    flashunit_state_clear_r(&g_weapon_6a5d18[i]);
  }
}

/* weapon_fx_clump_remove_a_r: C -> original weapon_fx_clump_remove_a (registers esi) */
__declspec(naked) void weapon_fx_clump_remove_a_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_a
        pop esi
        ret
    }
}

/* flashunit_state_clear_r: C -> original flashunit_state_clear (registers eax) */
__declspec(naked) void flashunit_state_clear_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call flashunit_state_clear
        ret
    }
}
