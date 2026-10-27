// cluster-entries: weapon_drum_update
/* Installed cluster weapon_drum_update (tools/cluster_install.py from output/cluster_pilot_zx/weapon_drum_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0045fb00*/ extern int weapon_drum_update_one();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*006a5a40*/ extern undefined g_weapon_6a5a40;
/*006a5c70*/ extern undefined4 g_weapon_drum_6a5c70;
extern char g_weapon_6a5c60[];
extern char g_weapon_drum_6a5d20[];
#define DRUM_KIND_FRAME_TRANSFORM (1)
#define DRUM_KIND_EXPLOSION_CLEAR (2)
#define DRUM_SLOT_WORDS (0xb)
#define WEAPON_RECORD_SIZE (0x44)

/* weapon_drum_update
 *
 * Per-frame update of the weapon drums and the weapon records.
 * Walks the drum slot table (g_weapon_drum_6a5c70 .. g_weapon_drum_6a5d20, DRUM_SLOT_WORDS ints per slot),
 * then calls weapon_drum_update_one() on every weapon record (g_weapon_6a5a40 .. g_weapon_6a5c60, WEAPON_RECORD_SIZE bytes each).
 * Takes no parameters, returns nothing.
 * Slot layout relative to `slot` (inferred): slot[-1] = modelling matrix pointer, slot[0] = kind,
 * slot[4] = pointer whose +4 field is the RwFrame to transform, slot[5] and slot[6] = countdowns (inferred).
 */

static int weapon_fx_explosion_pool_clear_r(int p0);
extern void weapon_fx_explosion_pool_clear(void);
void weapon_drum_update(void)

{
  int kind;
  int *slot;
  undefined *weapon;

  /* Pass 1: drum slots. Kind 0 = empty, so only active slots are processed. */
  slot = &g_weapon_drum_6a5c70;
  do {
    kind = *slot;
    if (kind != 0) {
      /* Kind 1: move the drum's frame to its matrix (inferred: drum position update). */
      if (kind == DRUM_KIND_FRAME_TRANSFORM) {
        RwFrameTransform(*(RwFrame **)(slot[4] + 4),(RwMatrix *)slot[-1],rwCOMBINEREPLACE);
      }
      /* Kind 2: clear the explosion pool for this slot (inferred from callee name). */
      else if (kind == DRUM_KIND_EXPLOSION_CLEAR) {
        weapon_fx_explosion_pool_clear_r(slot + -4);
      }
      /* Tick the countdowns down to zero, never below it. */
      if (slot[6] != 0) {
        slot[6] = slot[6] + -1;
      }
      if (slot[5] != 0) {
        slot[5] = slot[5] + -1;
      }
    }
    slot = slot + DRUM_SLOT_WORDS;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_weapon_drum_6a5d20));
  /* Pass 2: every weapon record gets its per-record update. */
  weapon = &g_weapon_6a5a40;
  do {
    weapon_drum_update_one(weapon);
    weapon = weapon + WEAPON_RECORD_SIZE;
  } while ((LONG_PTR)weapon < ((LONG_PTR)&g_weapon_6a5c60));
  return;
}

/* weapon_fx_explosion_pool_clear_r: C -> original weapon_fx_explosion_pool_clear (registers edi) */
__declspec(naked) int weapon_fx_explosion_pool_clear_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_fx_explosion_pool_clear
        pop edi
        ret
    }
}
