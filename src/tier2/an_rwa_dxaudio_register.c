/* RenderWare Audio (Criterion Software) DirectX audio module registration, MFL.exe 0x005ccee0-0x005ccfc0: register / unregister the
 * module's object definition tables and set up the voice interface definition.  Written from the MFL.exe disassembly (objdump);
 * same shape as the base module rows (an_rwa_005b66f0.c) and the interface rows (an_rwa_005db4b0.c). */
#include "mfl_tier2.h"

/*005b9fa0*/ extern int _rwaObjDefRegister(void *table, int count);      /* calls the table's functions; 1: ok */
/*005bd960*/ extern int FUN_005bd960(void *def);      /* RwaObjInterfaceDefCreate: 0 = already registered / failed */
/*005bd930*/ extern void FUN_005bd930(void *def, const void *uuid, int flags);      /* assign the interface uuid */
/*005bd810*/ extern void FUN_005bd810(void *def, const void *inputs, int num_inputs, const void *outputs, int num_outputs);      /* RwaObjInterfaceDefSetup */

/*0064b774*/ extern undefined g_rwa_dxaudio_core_register_fns;        /* 4 functions */
/*0064b71c*/ extern undefined g_rwa_dxaudio_class_register_fns;       /* 9 */
/*0064b764*/ extern undefined g_rwa_dxaudio_wave_register_fns;        /* 2 */
/*0064b784*/ extern undefined g_rwa_dxaudio_core_unregister_fns;      /* 4 */
/*0064b740*/ extern undefined g_rwa_dxaudio_class_unregister_fns;     /* 9 */
/*0064b76c*/ extern undefined g_rwa_dxaudio_wave_unregister_fns;      /* 2 */
/*007dc134*/ extern undefined g_rwa_dxaudio_voice_int_def;
/*005f837c*/ extern undefined g_rwa_dxaudio_voice_int_uuid;
/*0064b798*/ extern undefined g_rwa_dxaudio_voice_int_inputs;         /* 28 parameter definitions */
/*0064b9c8*/ extern undefined g_rwa_dxaudio_voice_int_outputs;        /* 29 parameter definitions */

int RwaDXAudioUnregister(void);

/* Registers the three tables in order; the first failure unregisters everything again.  1: all registered. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
int RwaDXAudioRegister(void)
{
  if (_rwaObjDefRegister(&g_rwa_dxaudio_core_register_fns, 4) && _rwaObjDefRegister(&g_rwa_dxaudio_class_register_fns, 9) &&
      _rwaObjDefRegister(&g_rwa_dxaudio_wave_register_fns, 2))
    return 1;
  RwaDXAudioUnregister();
  return 0;
}

/* 1: all three unregister tables ran through. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
int RwaDXAudioUnregister(void)
{
  if (_rwaObjDefRegister(&g_rwa_dxaudio_core_unregister_fns, 4) == 1 && _rwaObjDefRegister(&g_rwa_dxaudio_class_unregister_fns, 9) == 1 &&
      _rwaObjDefRegister(&g_rwa_dxaudio_wave_unregister_fns, 2) == 1)
    return 1;
  return 0;
}

/* Returns the voice interface definition, or NULL when it could not be created. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void *rwa_dxaudio_voice_interface_register_005ccf70(void)
{
  if (!FUN_005bd960(&g_rwa_dxaudio_voice_int_def))
    return 0;
  FUN_005bd930(&g_rwa_dxaudio_voice_int_def, &g_rwa_dxaudio_voice_int_uuid, 0);
  FUN_005bd810(&g_rwa_dxaudio_voice_int_def, &g_rwa_dxaudio_voice_int_inputs, 28, &g_rwa_dxaudio_voice_int_outputs, 29);
  return &g_rwa_dxaudio_voice_int_def;
}
