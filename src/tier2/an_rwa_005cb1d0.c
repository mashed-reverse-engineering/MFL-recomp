/* rwa_dxaudio_output_create_005cb1d0 (0x005cb1d0): RwaDXAudioOutputCreate (object definition of RwaDXAudioOutputRegister) */
#include "mfl_tier2.h"

/*005bb0a0*/ extern void *FUN_005bb0a0(void *objdef, int b1, int b2, int b3, int b4, int b5);      /* RwaObjCreate */
/*007dc068*/ extern undefined g_data_7dc068;      /* the class object definition */

/* Thin constructor: the class object definition first, then the caller's arguments in the order the callee wants them. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_output_create_005cb1d0(int a1, int a2, int a3, int a4, int a5)
{
  return FUN_005bb0a0(&g_data_7dc068, a1, a3, a2, a4, a5);
}
