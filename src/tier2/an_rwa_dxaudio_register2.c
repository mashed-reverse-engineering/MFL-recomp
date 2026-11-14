/* RenderWare Audio (Criterion Software) DirectX audio registration rows, MFL.exe 0x005c9950 (voice), 0x005cb240 (output) and
 * 0x005ccc10 (wave).  Written from the MFL.exe disassembly (capstone); same shape as the registration rows in an_rwa_005c9910.c /
 * an_rwa_005cb200.c / an_rwa_dxaudio_register.c.  Each one creates the object definition, fills it, sets up its parameter tables
 * and returns it (NULL on failure). */
#include "mfl_tier2.h"

/*005ba5c0*/ extern void *RwaObjDefFindByUUID(void *base, const void *uuid, void *flags);
/*005ba8d0*/ extern void *RwaObjDefCreate(void *base, int size, void *def, void *init, void *term, RwUInt32 hint);
/*005ba620*/ extern int RwaObjDefSetupParams(void *def, const void *params, int num_params, const void *methods, int num_methods);
/*005ba420*/ extern int RwaObjDefDestroy(void *def);
/*005b9fd0*/ extern void RwaObjDefSetInterface(void *def, void *interface_def);
/*005bd230*/ extern char *RwaWaveDefCreate(void *base, void *def);
/*005bc030*/ extern void RwaWaveDefAssignID(void *def, const void *uuid, int flags);
/*005bcf60*/ extern void RwaWaveDefAssignSupportedFormats(void *def, const void *formats, int count);
/*005c99f0*/ extern void _RwaDXAudioVoiceRegisterCommon(void *def);
/*005cb2d0*/ extern void _RwaDXAudioOutputRegisterCommon(void *def);
/*005ccf70*/ extern void *rwa_dxaudio_voice_interface_register_005ccf70(void);
/*005cdbc0*/ extern void *rwa_soft_voice_interface_register_005cdbc0(void);

/* wave definition methods */
/*005cccd0*/ extern int rwa_dxaudio_wave_create_005cccd0();
/*005ccdf0*/ extern int rwa_dxaudio_wave_free_data_005ccdf0();
/*005cce20*/ extern char *rwa_dxaudio_wave_upload_005cce20(char *wave, void *data, RwUInt32 offset, RwUInt32 size);
/*005cce80*/ extern int rwa_dxaudio_wave_download_005cce80();

/*00949d5c*/ extern void *g_data_949d5c;                  /* the base object definition */
/*005f835c*/ extern undefined g_sound_5f835c;             /* virtual voice definition uuid */
/*005f8f7c*/ extern undefined g_data_5f8f7c;
/*005f836c*/ extern undefined g_data_5f836c;              /* wave definition uuid */
/*007dbfe8*/ extern undefined g_data_7dbfe8;
/*007dc000*/ extern undefined g_data_7dc000;
/*007dc008*/ extern undefined g_data_7dc008;              /* the DirectX audio voice definition */
/*007dc064*/ extern undefined4 g_data_7dc064;             /* voice registered */
/*007dc068*/ extern undefined g_data_7dc068;              /* the DirectX audio output definition */
/*007dc0c4*/ extern undefined g_data_7dc0c4;
/*007dc0cc*/ extern undefined g_data_7dc0cc;
/*007dc0e4*/ extern undefined4 g_data_7dc0e4;             /* output registered */
/*007dc0e8*/ extern undefined g_data_7dc0e8;              /* the DirectX audio wave definition */
/*0064b070*/ extern undefined g_rwa_dxaudio_voice_param_table;
/*0064b310*/ extern undefined g_rwa_dxaudio_voice_method_table;
/*0064b5c8*/ extern undefined g_rwa_dxaudio_output_param_table;
/*0064b658*/ extern undefined g_rwa_dxaudio_output_method_table;
/*0064b6d0*/ extern undefined g_rwa_dxaudio_wave_formats;
/*0064b6e0*/ extern undefined g_rwa_dxaudio_wave_formats_alt;

/* TIER2: validated 2026-11-14 authority=diffemu */
void *RwaDXAudioVoiceRegisterUsingInterface(void)
{
  void *def = RwaObjDefCreate(RwaObjDefFindByUUID(0, &g_sound_5f835c, 0), 0xc, &g_data_7dc008, &g_data_7dbfe8, &g_data_7dc000, 0x3080d);
  void *interface_def;

  if (!def)
    return 0;
  _RwaDXAudioVoiceRegisterCommon(def);
  if (!RwaObjDefSetupParams(def, &g_rwa_dxaudio_voice_param_table, 0x1c, &g_rwa_dxaudio_voice_method_table, 0x1d)) {
    RwaObjDefDestroy(def);
    return 0;
  }
  interface_def = rwa_dxaudio_voice_interface_register_005ccf70();
  if (!interface_def) {
    RwaObjDefDestroy(def);
    return 0;
  }
  RwaObjDefSetInterface(def, interface_def);
  g_data_7dc064 = 1;
  return def;
}

/* TIER2: validated 2026-11-14 authority=diffemu */
void *RwaDXAudioOutputRegisterUsingInterface(void)
{
  void *def = RwaObjDefCreate(g_data_949d5c, 0xc, &g_data_7dc068, &g_data_7dc0cc, &g_data_7dc0c4, 0x3080c);
  void *interface_def;

  if (!def)
    return 0;
  _RwaDXAudioOutputRegisterCommon(def);
  if (!RwaObjDefSetupParams(def, &g_rwa_dxaudio_output_param_table, 6, &g_rwa_dxaudio_output_method_table, 5)) {
    RwaObjDefDestroy(def);
    return 0;
  }
  interface_def = rwa_soft_voice_interface_register_005cdbc0();
  if (!interface_def) {
    RwaObjDefDestroy(def);
    return 0;
  }
  RwaObjDefSetInterface(def, interface_def);
  g_data_7dc0e4 = 1;
  return def;
}

/* the wave definition: four methods, 12 / 4 sizes, 4 supported formats; also hands the alternative format list to the
   definition found under 0x5f8f7c */
/* TIER2: validated 2026-11-14 authority=diffemu */
void *RwaDXAudioWaveRegister(void)
{
  char *def = RwaWaveDefCreate(RwaObjDefFindByUUID(0, &g_sound_5f835c, 0), &g_data_7dc0e8);
  char *other;

  if (!def)
    return 0;
  RwaWaveDefAssignID(def, &g_data_5f836c, 0);
  *(void **)(def + 0xc) = rwa_dxaudio_wave_create_005cccd0;
  *(void **)(def + 0x10) = rwa_dxaudio_wave_free_data_005ccdf0;
  *(void **)(def + 0x14) = rwa_dxaudio_wave_upload_005cce20;
  *(void **)(def + 0x18) = rwa_dxaudio_wave_download_005cce80;
  *(RwUInt32 *)(def + 0x34) = 0xc;
  *(RwUInt32 *)(def + 0x38) = 4;
  RwaWaveDefAssignSupportedFormats(def, &g_rwa_dxaudio_wave_formats, 4);
  other = RwaObjDefFindByUUID(0, &g_data_5f8f7c, 0);
  if (other) {
    other = *(char **)(other + 0x54);
    *(void **)(other + 4) = &g_rwa_dxaudio_wave_formats_alt;
    *(RwUInt32 *)(other + 8) = 6;
  }
  return def;
}
