/* rwa_wave_upload_005bc520 (0x005bc520): waves (RwaWaveUpload): hand data to the wave's upload callback, converted to the wave's own format first when the source format differs */
#include "mfl_tier2.h"

typedef struct RwaWaveFormat { RwUInt32 sampleRate; const void *dataType; RwUInt32 length; RwUInt32 w[4]; } RwaWaveFormat;      /* 0x1c */

typedef struct RwaWave RwaWave;
typedef RwaWave *(*RwaWaveUploadFn)(RwaWave *wave, RwUInt32 offset, const void *data, RwUInt32 bytes);
typedef int (*RwaYieldFn)(void);
struct RwaWave {
  char pad0[0xc];
  char *def;               /* +0x0c: +0x14 upload, +0x1c yield */
  RwaWaveFormat source;    /* +0x10 */
  RwaWaveFormat format;    /* +0x2c */
};

/*005bcbf0*/ extern int RwaWaveFormatCompare(const RwaWaveFormat *a, const RwaWaveFormat *b);      /* != 0: equal */
/*005c4ac0*/ extern RwUInt32 _rwaWaveConvGetTargetOffset(const RwaWaveFormat *src, const void *data, RwUInt32 offset, const RwaWaveFormat *dst);
/*005c4af0*/ extern RwUInt32 rwa_wave_conv_get_target_size_005c4af0(const RwaWaveFormat *src, const void *data, const RwaWaveFormat *dst);
/*005bf0e0*/ extern void *FUN_005bf0e0(RwUInt32 size, RwUInt32 hint);      /* RWA aligned malloc */
/*005bf230*/ extern void rwa_aligned_free_005bf230(void *block);
/*005c4b10*/ extern RwUInt32 rwa_wave_conv_005c4b10(const RwaWaveFormat *srcFmt, const void *srcData, RwUInt32 srcOffset, RwaWaveFormat *dstFmt, void *dstData);

#define RWA_HINT 0x30806

/* Returns the upload callback's result, NULL without a callback or when the conversion fails.  After a converted upload the
   definition's yield callback runs until it returns 0. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaWave *rwa_wave_upload_005bc520(RwaWave *wave, RwUInt32 offset, const void *data, RwUInt32 bytes)
{
  RwaWaveUploadFn upload = *(RwaWaveUploadFn *)(wave->def + 0x14);
  RwaWaveFormat src;
  RwUInt32 dstOffset, size;
  char *buf = 0;
  RwaWave *r;
  RwaYieldFn yield;

  if (!upload)
    return 0;
  if (RwaWaveFormatCompare(&wave->source, &wave->format))
    r = upload(wave, offset, data, bytes);
  else {
    src = wave->source;
    dstOffset = _rwaWaveConvGetTargetOffset(&src, data, offset, &wave->format);
    src.length = bytes;
    size = rwa_wave_conv_get_target_size_005c4af0(&src, data, &wave->format);
    buf = (char *)FUN_005bf0e0(size, RWA_HINT);
    if (!buf)
      return 0;
    if (!rwa_wave_conv_005c4b10(&src, data, 0, &wave->format, buf)) {
      rwa_aligned_free_005bf230(buf);
      return 0;
    }
    r = upload(wave, dstOffset, buf, size);
  }
  if (buf) {
    while ((yield = *(RwaYieldFn *)(r->def + 0x1c)) != 0 && yield())
      ;
    rwa_aligned_free_005bf230(buf);
  }
  return r;
}
