/* rwa_dxaudio_wave_download_005cce80 (0x005cce80): DirectX audio wave: download a range of the wave (software waves through RwaWaveDownload, else from the DirectSound buffer) */
#include "mfl_tier2.h"

/*005bc120*/ extern char *RwaWaveDownload(void *wave, void *format, RwUInt32 offset, void *buffer);
/*005cd4b0*/ extern int _rwaDSBufferDownload(void *buffer, RwUInt32 offset, void *dest, RwUInt32 bytes);

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_dxaudio_wave_download_005cce80(char *wave, char *format, RwUInt32 a3, void *a4)
{
  if (*(unsigned char *)(*(char **)(*(char **)(wave + 0x58) + 0x20) + 0x50) & 8) {
    if (!RwaWaveDownload(*(void **)(wave + 0x4c), format, a3, a4))
      return 0;
  } else if (!_rwaDSBufferDownload(wave + 0x5c, a3, a4, *(RwUInt32 *)(format + 8)))
    return 0;
  return wave;
}
