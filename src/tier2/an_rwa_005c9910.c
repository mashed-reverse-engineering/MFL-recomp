/* rwa_dxaudio_voice_register_def_005c9910 (0x005c9910): DirectX audio voice (RwaDXAudioVoiceRegister): object definition derived from the virtual voice definition */
#include "mfl_tier2.h"

/*005ba5c0*/ extern void *RwaObjDefFindByUUID(void *a1, const void *uuid, void *a3);
/*005ba8d0*/ extern void *RwaObjDefCreate(void *base, int a2, void *def, int a4, int a5, RwUInt32 hint);
/*005c99f0*/ extern void _RwaDXAudioVoiceRegisterCommon(void *def);
/*005f835c*/ extern undefined g_sound_5f835c;      /* virtual voice definition uuid */
/*007dc008*/ extern undefined g_data_7dc008;      /* the DirectX audio voice definition */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_dxaudio_voice_register_def_005c9910(void)
{
  void *def = RwaObjDefCreate(RwaObjDefFindByUUID(0, &g_sound_5f835c, 0), 0, &g_data_7dc008, 0, 0, 0x3080d);

  if (!def)
    return 0;
  _RwaDXAudioVoiceRegisterCommon(def);
  return def;
}
