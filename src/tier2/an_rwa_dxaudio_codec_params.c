/* RenderWare Audio (Criterion Software) DirectShow codec voice: the two format parameter getters, MFL.exe 0x005cf190 and
 * 0x005cf1b0.  Written from the MFL.exe disassembly (objdump).  (The pin enumerator constructor 0x005cfc80 that W6 had in this
 * file is an int3 trap row now: it is reached only from trapped AU2 code, see tools/delink/rw_trap.txt.) */
#include "mfl_tier2.h"

/*005cee40*/ extern RwUInt32 _rwaDXAudioCodecGetInputFormat(char *voice);
/*005cee50*/ extern RwUInt32 _rwaDXAudioCodecGetOutputFormat(char *voice);

/* parameter getter (voice, parameter, value out); returns the voice */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_dxaudio_codec_input_format_param_handler_005cf190(char *voice, void *param, RwUInt32 *value)
{
  *value = _rwaDXAudioCodecGetInputFormat(voice);
  return voice;
}

/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_dxaudio_codec_output_format_param_handler_005cf1b0(char *voice, void *param, RwUInt32 *value)
{
  *value = _rwaDXAudioCodecGetOutputFormat(voice);
  return voice;
}
