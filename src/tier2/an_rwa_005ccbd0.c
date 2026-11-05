/* rwa_dxaudio_wave_create_005ccbd0 (0x005ccbd0): RwaDXAudioWaveCreate (wave definition of RwaDXAudioWaveRegister) */
#include "mfl_tier2.h"

/*005bc290*/ extern void *FUN_005bc290(void *objdef, int b1, int b2, int b3, int b4, int b5);      /* RwaWaveCreate */
/*007dc0e8*/ extern undefined g_data_7dc0e8;      /* the class object definition */

/* Thin constructor: the class object definition first, then the caller's arguments in the order the callee wants them. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_wave_create_005ccbd0(int a1, int a2, int a3, int a4, int a5)
{
  return FUN_005bc290(&g_data_7dc0e8, a1, a2, a3, a4, a5);
}
