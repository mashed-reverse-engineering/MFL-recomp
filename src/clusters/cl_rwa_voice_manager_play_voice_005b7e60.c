// cluster-entries: rwa_voice_manager_play_voice_005b7e60 rwa_voice_manager_control_005b7eb0
/* Installed cluster rwa_voice_manager_play_voice_005b7e60 (tools/cluster_install.py from output/cluster_pilot_l2/rwa_voice_manager_play_voice_005b7e60.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA virtual voice manager (voices/rwavoicemanager.c): (re)starting a voice (register function, voice in eax) and the voice
   control entry (0 play, 1 stop, 2 update the priority).  Cluster: rwa_voice_manager_play_voice_005b7e60 is entered through the
   generated eax thunk; the stop (esi) and prioritize (ecx) functions are called through generated adapters. */
#include "mfl_tier2.h"

typedef struct RwaVoice {
  struct RwaVoice *next, *prev;     /* +0x00 */
  void *handle;                     /* +0x08 */
  RwUInt32 flags;                   /* +0x0c 1 playing, 2 started */
  RwInt32 weight;                   /* +0x10 real voices the voice needs */
  RwUInt32 w14[4];
  RwUInt32 startPosition;           /* +0x24 */
} RwaVoice;


/*007dab7c*/ extern RwInt32 g_data_7dab7c;        /* playing voices */
/*007dab80*/ extern RwInt32 g_data_7dab80;        /* real voices in use */

static void _RwaVoiceManagerStartVoice_r(RwaVoice * p0);
extern void _RwaVoiceManagerStartVoice(void);
static void FUN_005b7960_r(RwaVoice * p0);
extern void FUN_005b7960(void);
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_play_voice_005b7e60_n(RwaVoice *v)
{
  if (v->flags & 1)
    _RwaVoiceManagerStartVoice_r(v);
  FUN_005b7960_r(v);
  v->weight = 1;
  v->flags |= 2;
  v->startPosition = 0;
  g_data_7dab80++;
  g_data_7dab7c++;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_control_005b7eb0(RwaVoice *v, int op)
{
  if (op == 0)
    rwa_voice_manager_play_voice_005b7e60_n(v);
  else if (op == 1)
    _RwaVoiceManagerStartVoice_r(v);
  else if (op == 2 && (v->flags & 1))
    FUN_005b7960_r(v);
}

/* _RwaVoiceManagerStartVoice_r: C -> original _RwaVoiceManagerStartVoice (registers esi) */
__declspec(naked) void _RwaVoiceManagerStartVoice_r(RwaVoice * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call _RwaVoiceManagerStartVoice
        pop esi
        ret
    }
}

/* FUN_005b7960_r: C -> original FUN_005b7960 (registers ecx) */
__declspec(naked) void FUN_005b7960_r(RwaVoice * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_005b7960
        ret
    }
}

/* entry thunk at the original address of rwa_voice_manager_play_voice_005b7e60: register inputs (eax) -> natural C rwa_voice_manager_play_voice_005b7e60_n; ret 0 */
__declspec(naked) void rwa_voice_manager_play_voice_005b7e60(void)
{
    __asm {
        push eax
        call rwa_voice_manager_play_voice_005b7e60_n
        add esp, 4
        ret
    }
}
