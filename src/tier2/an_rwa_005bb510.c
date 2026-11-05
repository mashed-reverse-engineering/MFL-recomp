/* rwa_voice_interface_register_005bb510 (0x005bb510): RwaVoiceInterfaceRegister, set up the voice interface definition once */
#include "mfl_tier2.h"

/*005bd960*/ extern int FUN_005bd960(void *def);      /* RwaObjInterfaceDefCreate: 0 = already registered / failed */
/*005bd930*/ extern void FUN_005bd930(void *def, const void *uuid, int flags);      /* assign the interface uuid */
/*005bd810*/ extern void FUN_005bd810(void *def, const void *inputs, int num_inputs, const void *outputs, int num_outputs);      /* RwaObjInterfaceDefSetup */

/*007daedc*/ extern undefined g_rwa_voice_int_def;
/*005f7734*/ extern undefined g_rwa_voice_int_uuid;
/*006499d8*/ extern undefined g_rwa_voice_int_inputs;      /* 8 parameter definitions */
/*00649a78*/ extern undefined g_rwa_voice_int_outputs;     /* 9 parameter definitions */

/* Returns the definition, or NULL when it could not be created. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_voice_interface_register_005bb510(void)
{
  if (!FUN_005bd960(&g_rwa_voice_int_def))
    return 0;
  FUN_005bd930(&g_rwa_voice_int_def, &g_rwa_voice_int_uuid, 0);
  FUN_005bd810(&g_rwa_voice_int_def, &g_rwa_voice_int_inputs, 8, &g_rwa_voice_int_outputs, 9);
  return &g_rwa_voice_int_def;
}
