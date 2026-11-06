/* rwa_wave_download_005bc120 (0x005bc120): waves (RwaWaveDownload): read samples through the wave's read callback, converting into the requested format when it differs */
#include "mfl_tier2.h"

/*005c4ac0*/ extern RwUInt32 _rwaWaveConvGetTargetOffset(void *format, void *buffer, RwUInt32 offset, void *source);
/*005bcbf0*/ extern int RwaWaveFormatCompare(void *a, void *b);      /* nonzero: same */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005c4b10*/ extern int _rwaWaveConv(void *srcFormat, void *src, RwUInt32 offset, void *dstFormat, void *dst);
/*005bcd50*/ extern void _rwaWaveFormatFreeMiscData(void *format);

typedef struct RwaWaveFormat {      /* 0x1c bytes */
  RwUInt32 sampleRate;
  void *dataType;
  RwUInt32 length;
  RwUInt32 w0c;
  void *extra;      /* +0x10 */
  RwUInt32 w14, w18;
} RwaWaveFormat;

typedef int (*RwaWaveRead)(char *wave, RwaWaveFormat *format, RwUInt32 offset, void *buffer);

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_download_005bc120(char *wave, RwaWaveFormat *format, RwUInt32 offset, void *buffer)
{
  RwaWaveRead read = *(RwaWaveRead *)(*(char **)(wave + 0xc) + 0x18);
  RwaWaveFormat *own = (RwaWaveFormat *)(wave + 0x2c), copy;
  void *dest = buffer, *heap = 0;
  int freeExtra = 1;
  RwUInt32 start, size;

  if (!read)
    return 0;
  if (!format)
    format = own;
  if (!format->dataType)
    format->dataType = own->dataType;
  if (format->extra)
    freeExtra = 0;
  start = _rwaWaveConvGetTargetOffset(format, buffer, offset, own);
  size = own->length - start;
  if (!RwaWaveFormatCompare(own, format)) {      /* other format: read into a temporary block, convert */
    heap = FUN_005bf0d0(size, 0x30806);
    if (!heap)
      return 0;
    dest = heap;
  }
  copy = *own;
  copy.length = size;
  if (!read(wave, &copy, start, dest))
    return 0;
  if (heap) {
    if (!_rwaWaveConv(&copy, dest, start, format, buffer)) {
      if (freeExtra)
        _rwaWaveFormatFreeMiscData(format);
      FUN_005bf120(heap);
      return 0;
    }
    if (freeExtra)
      _rwaWaveFormatFreeMiscData(format);
    FUN_005bf120(heap);
  }
  return wave;
}
