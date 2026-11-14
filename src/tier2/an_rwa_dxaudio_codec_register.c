/* RenderWare Audio (Criterion Software) DirectX audio codec registration, MFL.exe 0x005ce1e0 / 0x005ce240: the codec methods are
 * TRAP rows and the parameter table holds a trapped handler.  Written from the MFL.exe disassembly (capstone).  The original
 * writes / references the same addresses: T-TCA lines in rows_tca_g2.txt. */
#include "mfl_tier2.h"

/*005ba8d0*/ extern void *RwaObjDefCreate(void *base, int size, void *def, void *init, void *term, RwUInt32 hint);
/*005ba620*/ extern int RwaObjDefSetupParams(void *def, const void *params, int num_params, const void *methods, int num_methods);
/*005ba420*/ extern int RwaObjDefDestroy(void *def);
/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int flags);

/* codec definition methods (trap rows) */
/*005ce2b0*/ extern int RwaDXAudioCodecUnregister();
/*005cea00*/ extern int gap_5cea00();
/*005ceab0*/ extern int rwa_dxaudio_codec_def_fn30_005ceab0();
/*005ceda0*/ extern int rwa_dxaudio_codec_def_fn3c_005ceda0();

/*00949d5c*/ extern void *g_data_949d5c;                  /* the base object definition */
/*005f864c*/ extern undefined g_data_5f864c;              /* codec uuid */
/*0062e591*/ extern signed char g_data_62e591;
/*007dc1a0*/ extern undefined g_data_7dc1a0;
/*007dc1a8*/ extern undefined g_data_7dc1a8;              /* the DirectX audio codec definition */
/*007dc204*/ extern undefined g_data_7dc204;
/*0064bcf0*/ extern undefined g_rwa_dxaudio_codec_param_table;
/*0064bd38*/ extern undefined g_rwa_dxaudio_codec_method_table;
/*0064bd98*/ extern undefined g_data_64bd98;

/* TIER2: validated 2026-11-14 authority=diffemu */
void _RwaDXAudioCodecRegisterCommon(char *def)
{
  RwaObjDefAssignID(def, &g_data_5f864c, 0);
  *(RwUInt32 *)(def + 0x40) = 0x7b8;
  *(void **)(def + 0x28) = RwaDXAudioCodecUnregister;
  *(void **)(def + 0x34) = gap_5cea00;
  *(RwUInt32 *)(def + 0x40) = 0x7b8 | (RwUInt32)(g_data_62e591 - 1) << 28;
  *(RwUInt32 *)(def + 0x2c) = 0;
  *(void **)(def + 0x30) = rwa_dxaudio_codec_def_fn30_005ceab0;
  *(void **)(def + 0x3c) = rwa_dxaudio_codec_def_fn3c_005ceda0;
  *(RwUInt16 *)(def + 0x44) = 0x60;
  *(void **)(def + 0x54) = &g_data_64bd98;
}

/* TIER2: validated 2026-11-14 authority=diffemu */
void *RwaDXAudioCodecRegister(void)
{
  void *def = RwaObjDefCreate(g_data_949d5c, 0xc, &g_data_7dc1a8, &g_data_7dc204, &g_data_7dc1a0, 0x3080b);

  if (!def)
    return 0;
  _RwaDXAudioCodecRegisterCommon(def);
  if (!RwaObjDefSetupParams(def, &g_rwa_dxaudio_codec_param_table, 3, &g_rwa_dxaudio_codec_method_table, 4)) {
    RwaObjDefDestroy(def);
    return 0;
  }
  return def;
}
