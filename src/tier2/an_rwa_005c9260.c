/* rwa_stream_codec_interface_register_005c9260 (0x005c9260): RwaStreamCodecInterfaceRegister, set up the stream codec interface definition once */
#include "mfl_tier2.h"

/*005bd960*/ extern int FUN_005bd960(void *def);      /* RwaObjInterfaceDefCreate: 0 = already registered / failed */
/*005bd930*/ extern void FUN_005bd930(void *def, const void *uuid, int flags);      /* assign the interface uuid */
/*005bd810*/ extern void FUN_005bd810(void *def, const void *inputs, int num_inputs, const void *outputs, int num_outputs);      /* RwaObjInterfaceDefSetup */

/*007dbf6c*/ extern undefined g_rwa_stream_codec_int_def;
/*005f826c*/ extern undefined g_rwa_stream_codec_int_uuid;
/*0064aec0*/ extern undefined g_rwa_stream_codec_int_inputs;      /* 3 parameter definitions */
/*0064af00*/ extern undefined g_rwa_stream_codec_int_outputs;     /* 4 parameter definitions */

/* Returns the definition, or NULL when it could not be created. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_stream_codec_interface_register_005c9260(void)
{
  if (!FUN_005bd960(&g_rwa_stream_codec_int_def))
    return 0;
  FUN_005bd930(&g_rwa_stream_codec_int_def, &g_rwa_stream_codec_int_uuid, 0);
  FUN_005bd810(&g_rwa_stream_codec_int_def, &g_rwa_stream_codec_int_inputs, 3, &g_rwa_stream_codec_int_outputs, 4);
  return &g_rwa_stream_codec_int_def;
}
