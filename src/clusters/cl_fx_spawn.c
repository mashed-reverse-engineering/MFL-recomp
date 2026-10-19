// cluster-entries: fx_spawn
/* Installed cluster fx_spawn (tools/cluster_install.py from output/cluster_pilot11/fx_spawn.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*0049be80*/ extern void fx_system_reset_positions(int fx_system, int pos);
/*00472890*/ extern void sfx_play_voice_at(int id, int pos, int voice);
typedef struct { int clump; float life; int phase; int active; int fx_system; float pos[3]; } FxSlotI;
/*00651960*/ extern FxSlotI g_fx_651960[10];
/*00651aac*/ extern int g_fx_spawn_651aac;
/*008d98b0*/ extern int g_fx_8d98b0;
/* fx_spawn */

/* WARNING: Unknown calling convention */
/* "takes the next slot of the 32 byte stride effect table at 0x651960, sets its timer 6.0 and its
   transform" */

void fx_spawn_n(RwMatrix *in_eax, float *in_edi)

{
  int idx;
  FxSlotI *slot;
  
  slot = g_fx_651960 + g_fx_spawn_651aac;
  g_fx_651960[g_fx_spawn_651aac].phase = 0;
  g_fx_651960[g_fx_spawn_651aac].active = 1;
  g_fx_651960[g_fx_spawn_651aac].life = 6.0f;
  RwFrameTransform(*(RwFrame **)(slot->clump + 4),in_eax,rwCOMBINEREPLACE);
  slot->pos[0] = in_edi[0];
  slot->pos[1] = in_edi[1];
  slot->pos[2] = in_edi[2];
  fx_system_reset_positions(slot->fx_system,*(int *)(slot->clump + 4) + 0x40);
  idx = g_fx_spawn_651aac;
  g_fx_8d98b0 = idx;
  if (3 < idx) {
    g_fx_8d98b0 = (int)((unsigned int)idx - ((unsigned int)idx & 0xfffffffc));
  }
  sfx_play_voice_at(0x15,*(int *)(slot->clump + 4) + 0x40,g_fx_8d98b0);
  sfx_play_voice_at(0x14,*(int *)(slot->clump + 4) + 0x40,g_fx_8d98b0);
  g_fx_spawn_651aac = g_fx_spawn_651aac + 1;
  if (9 < g_fx_spawn_651aac) {
    g_fx_spawn_651aac = 0;
  }
  return;
}

/* entry thunk at the original address of fx_spawn: register inputs (eax,edi) -> natural C fx_spawn_n; ret 0 */
__declspec(naked) void fx_spawn(void)
{
    __asm {
        push edi
        push eax
        call fx_spawn_n
        add esp, 8
        ret
    }
}
