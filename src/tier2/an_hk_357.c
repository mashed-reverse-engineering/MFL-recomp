#include "mfl_tier2.h"
/*00486c40*/ extern int particles_flush();
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern undefined g_weapon_6a5d78;
/*008bf000*/ extern undefined g_weapon_8bf000;
static int weapon_flashbang_flare_particles_r(undefined4 * reg_esi);
extern void weapon_flashbang_flare_particles(void);
__declspec(naked) int weapon_flashbang_flare_particles_r(undefined4 * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_flashbang_flare_particles
        pop esi
        ret
    }
}

static int weapon_flashbang_state_step_r(int reg_eax);
extern void weapon_flashbang_state_step(void);
extern char g_weapon_6a5dc8[];

#define FLASHBANG_STATE_STRIDE (0x14)
#define FLASHBANG_FLARE_STRIDE (0x18)

__declspec(naked) int weapon_flashbang_state_step_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_flashbang_state_step
        ret
    }
}

/*
 * weapon_flashbang_update: per-frame update of the flashbang weapon.
 * Steps every flashbang state entry (g_weapon_6a5d78 .. g_weapon_6a5dc8),
 * then advances every flare particle entry (g_weapon_6a5d18 .. g_weapon_6a5d78),
 * then flushes the particle system with g_weapon_8bf000 (inferred).
 * Takes no parameters and returns nothing.
 */
void weapon_flashbang_update(void)

{
  undefined *entry;

  /* Walk the state table in FLASHBANG_STATE_STRIDE steps (inferred: one entry per flashbang instance). */
  entry = &g_weapon_6a5d78;
  do {
    weapon_flashbang_state_step_r(entry);
    entry = entry + FLASHBANG_STATE_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_weapon_6a5dc8));
  /* Walk the flare table in FLASHBANG_FLARE_STRIDE steps (inferred: one entry per flare). */
  entry = &g_weapon_6a5d18;
  do {
    weapon_flashbang_flare_particles_r(entry);
    entry = entry + FLASHBANG_FLARE_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_weapon_6a5d78));
  particles_flush(&g_weapon_8bf000);
  return;
}


