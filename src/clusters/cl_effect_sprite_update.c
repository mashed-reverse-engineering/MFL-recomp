// cluster-entries: effect_sprite_update
/* Installed cluster effect_sprite_update (tools/cluster_install.py from output/cluster_pilot11/effect_sprite_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*007ef7f4*/ extern float g_tick_seconds;
/* effect_sprite_update */

/* WARNING: Unknown calling convention */
/* "advances age, scales the frame by the progress, resets the sprite when age >= lifetime" */

static int effect_sprite_reset_r(undefined4 * p0);
extern void effect_sprite_reset(void);
void effect_sprite_update_n(char *in_esi)

{
  RwV3d local_c;
  float progress;

  *(float *)(in_esi + 0x54) = g_tick_seconds + *(float *)(in_esi + 0x54);
  progress = g_tick_seconds * *(float *)(in_esi + 0x50) + *(float *)(in_esi + 0x48);
  *(float *)(in_esi + 0x48) = progress;
  if (*(float *)(in_esi + 0x4c) <= progress) {
    effect_sprite_reset_r((undefined4 *)in_esi);
    return;
  }
  local_c.x = progress;
  local_c.y = progress;
  local_c.z = progress;
  RwFrameTransform(*(RwFrame **)(*(int *)(in_esi + 0x44) + 4),(RwMatrix *)in_esi,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)(*(int *)(in_esi + 0x44) + 4),&local_c,rwCOMBINEPRECONCAT);
  return;
}

/* effect_sprite_reset_r: C -> original effect_sprite_reset (registers eax) */
__declspec(naked) int effect_sprite_reset_r(undefined4 * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call effect_sprite_reset
        ret
    }
}

/* entry thunk at the original address of effect_sprite_update: register inputs (esi) -> natural C effect_sprite_update_n; ret 0 */
__declspec(naked) void effect_sprite_update(void)
{
    __asm {
        push esi
        call effect_sprite_update_n
        add esp, 4
        ret
    }
}
