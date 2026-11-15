/* RenderWare Audio (Criterion Software): soft voice output interface definition, MFL.exe 0x005cdbc0.
 * Tables: inputs 0x64bc10 (6 x 20 B), outputs 0x64bc88 (5 x 20 B) = the whole data row g_libspan_0064bc10 (224 B, ends where
 * g_rwa_dxaudio_codec_param_table starts); their words point only at .rdata names and UUIDs (no code). */
#include "mfl_tier2.h"

/*005bd960*/ extern int RwaObjInterfaceDefCreate(void *def);
/*005bd930*/ extern void RwaWaveDictAssignID(void *def, const void *uuid, int flags);
/*005bd810*/ extern void RwaObjInterfaceDefSetup(void *def, const void *inputs, int num_inputs, const void *outputs, int num_outputs);

/*005f863c*/ extern undefined g_rwa_soft_voice_int_uuid;
/*007dc15c*/ extern undefined g_rwa_soft_voice_int_def;
/*0064bc10*/ extern undefined g_rwa_soft_voice_int_inputs;
/*0064bc88*/ extern undefined g_rwa_soft_voice_int_outputs;

/* the output's interface definition (6 inputs, 5 outputs); NULL when it could not be created */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *rwa_soft_voice_interface_register_005cdbc0(void)
{
  if (!RwaObjInterfaceDefCreate(&g_rwa_soft_voice_int_def))
    return 0;
  RwaWaveDictAssignID(&g_rwa_soft_voice_int_def, &g_rwa_soft_voice_int_uuid, 0);
  RwaObjInterfaceDefSetup(&g_rwa_soft_voice_int_def, &g_rwa_soft_voice_int_inputs, 6, &g_rwa_soft_voice_int_outputs, 5);
  return &g_rwa_soft_voice_int_def;
}
