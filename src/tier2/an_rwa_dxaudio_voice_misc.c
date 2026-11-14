/* RenderWare Audio (Criterion Software) DirectX audio voice / output / wave methods, MFL.exe 0x005ca710, 0x005ca780, 0x005cc2a0
 * and 0x005cce20.  Written from the MFL.exe disassembly (capstone).  A voice whose wave format has flag 8 (format + 0x50) plays
 * through the software buffer at voice + 0x11c; the others through their DirectSound buffer fields. */
#include "mfl_tier2.h"

/*005bc520*/ extern int RwaWaveUpload(void *wave, void *data, RwUInt32 offset, RwUInt32 size);
/*005cd410*/ extern int _rwaDSBufferUpload(void *buffer, void *data, RwUInt32 offset, RwUInt32 size);

/* sets the play position (in samples) */
/* TIER2: validated 2026-11-14 authority=diffemu */
void _rwaDXAudioVoiceSetWavePos(char *voice, RwUInt32 pos)
{
  char *format = *(char **)(voice + 0x94);
  unsigned char *wave_format;
  RwUInt32 bytes;

  if (format[0x50] & 8) {
    char *soft = *(char **)(voice + 0x11c);
    *(RwUInt32 *)(soft + 0x88) = 0;
    *(RwUInt32 *)(soft + 0x8c) = pos;
    return;
  }
  wave_format = *(unsigned char **)(voice + 0x84);
  bytes = (wave_format[0x38] >> 3) * wave_format[0x39] * pos;      /* bits per sample / 8 * channels */
  *(RwUInt32 *)(voice + 0x8c) = bytes;
  *(RwUInt32 *)(voice + 0x90) = bytes;
  *(RwUInt32 *)(voice + 0x88) = pos;
  *(RwUInt32 *)(voice + 0x28) |= 0x400;
}

/* switches looping on or off */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void _rwaDXAudioVoiceSetLoop(char *voice, RwBool loop)
{
  char *format = *(char **)(voice + 0x94);

  if (format[0x50] & 8) {
    char *soft = *(char **)(voice + 0x11c);
    if (loop)
      *(RwUInt32 *)(soft + 0xcc) |= 8;
    else
      *(RwUInt32 *)(soft + 0xcc) &= ~8u;
    return;
  }
  if (loop)
    *(RwUInt32 *)(voice + 0x28) |= 0x800;
  else
    *(RwUInt32 *)(voice + 0x28) &= ~0x800u;
}

/* sets the output gain and marks every voice of the output's list (head at output + 0xc) for a volume update */
/* TIER2: unvalidated 2026-11-14 authority=shadow */
void _rwaDXAudioOutputSetGain(char *output, float gain)
{
  char **head = (char **)(output + 0xc);
  char *node;
  char *soft;

  *(float *)(output + 0x38) = gain;
  for (node = *head; node != (char *)head; node = *(char **)node) {
    *(float *)(node + 0x1c) = *(float *)(node + 0x1c) * 1.0f;      /* the original re-stores the voice gain through the x87 stack (fld / fstp) */
    *(RwUInt32 *)(node + 0x14) |= 0x40;
  }
  soft = *(char **)(output + 0x11c);
  if (soft)
    *(float *)(soft + 0x30) = gain;
}

/* wave upload: through the software wave or the DirectSound buffer; the wave, or NULL when the upload failed */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
char *rwa_dxaudio_wave_upload_005cce20(char *wave, void *data, RwUInt32 offset, RwUInt32 size)
{
  char *format = *(char **)(*(char **)(wave + 0x58) + 0x20);

  if (format[0x50] & 8) {
    if (!RwaWaveUpload(*(void **)(wave + 0x4c), data, offset, size))
      return 0;
  } else if (!_rwaDSBufferUpload(wave + 0x5c, data, offset, size))
    return 0;
  return wave;
}
