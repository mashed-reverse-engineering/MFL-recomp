/* rwa_dxaudio_output_register_def_005cb200 (0x005cb200): DirectX audio output: object definition from the NULL definition */
#include "mfl_tier2.h"

/*005ba8d0*/ extern void *RwaObjDefCreate(void *base, int a2, void *def, int a4, int a5, RwUInt32 hint);
/*005cb2d0*/ extern void _RwaDXAudioOutputRegisterCommon(void *def);
/*00949d5c*/ extern void *g_data_949d5c;      /* the NULL object definition */
/*007dc068*/ extern undefined g_data_7dc068;  /* the DirectX audio output definition */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_output_register_def_005cb200(void)
{
  void *def = RwaObjDefCreate(g_data_949d5c, 0, &g_data_7dc068, 0, 0, 0x3080c);

  if (!def)
    return 0;
  _RwaDXAudioOutputRegisterCommon(def);
  return def;
}
