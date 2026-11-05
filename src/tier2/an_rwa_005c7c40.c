/* rwa_wave_conv_xbox_adpcm_005c7c40 (0x005c7c40): wave conversion PCM16 <-> Xbox ADPCM (_RwaWaveConvXBoxADPCM): encode / decode a buffer, de- and re-interleaving the channels */
#include <string.h>
#include "mfl_tier2.h"

/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */
/*005bf130*/ extern void *FUN_005bf130(RwUInt32 count, RwUInt32 size, RwUInt32 hint);      /* RwCalloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bcc50*/ extern RwUInt32 RwaWaveFormatGetFrameSize(const void *format);
/*005c7a20*/ extern void _rwaWaveConvXADPCMEncodeMono(const short *src, void *dst, int samples);
/*005c7960*/ extern void _rwaWaveConvXADPCMDecodeMono(const void *src, short *dst, RwUInt32 blocks);
/*005c7b00*/ extern void _rwaWaveConvXADPCMDecodeStereo(const void *src, short *dst, RwUInt32 blocks);
/*005f7924*/ extern undefined g_data_5f7924;      /* rwaWAVEFORMAT_PCM */
/*005f7954*/ extern undefined g_data_5f7954;      /* rwaWAVEFORMAT_XADPCM */
/*005f79b4*/ extern undefined g_data_5f79b4;      /* rwaWAVEFORMAT_IMAADPCM (encoded the same way) */

#define XADPCM_SAMPLES     64      /* samples per channel in one block */
#define XADPCM_BLOCK_BYTES 36      /* bytes of one mono block */
#define RWA_HINT           0x10806

typedef struct RwaWaveFormat {
  char pad0[4];
  const void *dataType;      /* +0x04 uuid */
  RwUInt32 length;           /* +0x08 bytes */
  unsigned char bits;        /* +0x0c */
  unsigned char channels;    /* +0x0d */
} RwaWaveFormat;

/* Converts src (from byte srcOffset on) into dst.  *dstOffset gets the matching byte offset in the target (a local when NULL),
   the result is the number of target bytes from there (only computed when dst is NULL).  Encoding PCM -> ADPCM works per
   channel on a de-interleaved copy and interleaves the blocks back frame by frame (from the start of dst); decoding handles
   mono and stereo (other channel counts: 0 bytes, *dstOffset as it was). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_wave_conv_xbox_adpcm_005c7c40(const RwaWaveFormat *srcFmt, const char *src, RwUInt32 srcOffset,
                                           const RwaWaveFormat *dstFmt, char *dst, RwUInt32 *dstOffset)
{
  RwUInt32 localOffset;
  RwUInt32 bytes = 0;
  RwUInt32 chBytes, encBytes, ch, n, frame;
  const char *in;
  char *out, *tmp;
  const short *s;
  short *d;

  if (!dstOffset)
    dstOffset = &localOffset;

  if (RwaUUIDCompare(srcFmt->dataType, &g_data_5f7924) == 0 &&
      (RwaUUIDCompare(dstFmt->dataType, &g_data_5f7954) == 0 || RwaUUIDCompare(dstFmt->dataType, &g_data_5f79b4) == 0)) {
    /* encode */
    chBytes = srcFmt->length / srcFmt->channels;      /* PCM bytes per channel */
    encBytes = ((chBytes >> 1) + XADPCM_SAMPLES - 1) / XADPCM_SAMPLES * XADPCM_BLOCK_BYTES;
    *dstOffset = ((srcOffset >> 1) + XADPCM_SAMPLES - 1) / XADPCM_SAMPLES * XADPCM_BLOCK_BYTES;
    bytes = srcFmt->channels * encBytes - *dstOffset;
    if (!dst)
      return bytes;

    if (dstFmt->channels > 1) {
      tmp = (char *)FUN_005bf130(1, srcFmt->length, RWA_HINT);
      if (!tmp)
        return 0;
      chBytes &= ~1u;
      for (ch = 0; ch < srcFmt->channels; ch++) {      /* de-interleave: channel ch -> tmp + ch * chBytes */
        s = (const short *)src + ch;
        d = (short *)(tmp + ch * chBytes);
        for (n = chBytes; n; n -= 2) {
          *d++ = *s;
          s += srcFmt->channels;
        }
      }
      in = tmp;
    } else
      in = tmp = (char *)src;

    in += srcOffset;
    for (ch = 0; ch < srcFmt->channels; ch++) {      /* channel ch -> its own run of blocks */
      out = dst + *dstOffset + ch * encBytes;
      memset(out, 0, encBytes);
      _rwaWaveConvXADPCMEncodeMono((const short *)in, out, (int)(chBytes - srcOffset) >> 1);
      in += chBytes;
    }

    if (dstFmt->channels > 1) {      /* interleave the per channel blocks, one frame (block) of each channel in turn */
      frame = RwaWaveFormatGetFrameSize(dstFmt);
      for (ch = 0; ch < dstFmt->channels; ch++) {
        in = dst + ch * encBytes;
        out = tmp + ch * frame;
        for (n = encBytes; n; n -= frame) {
          memcpy(out, in, frame);
          out += dstFmt->channels * frame;
          in += frame;
        }
      }
      memcpy(dst, tmp, dstFmt->channels * encBytes);
      FUN_005bf120(tmp);
    }
    return bytes;
  }

  /* decode: 36 byte blocks -> 64 samples per channel */
  if (srcFmt->channels == 1) {
    *dstOffset = srcOffset / XADPCM_BLOCK_BYTES * (XADPCM_SAMPLES * 2);
    bytes = srcFmt->length / XADPCM_BLOCK_BYTES * (XADPCM_SAMPLES * 2);
  } else if (srcFmt->channels == 2) {
    *dstOffset = srcOffset / (2 * XADPCM_BLOCK_BYTES) * (2 * XADPCM_SAMPLES * 2);
    bytes = srcFmt->length / (2 * XADPCM_BLOCK_BYTES) * (2 * XADPCM_SAMPLES * 2);
  }
  bytes -= *dstOffset;
  if (!dst)
    return bytes;
  src += srcOffset;
  dst += *dstOffset;
  memset(dst, 0, bytes);
  n = srcFmt->length / (srcFmt->channels * XADPCM_BLOCK_BYTES);
  if (srcFmt->channels == 1)
    _rwaWaveConvXADPCMDecodeMono(src, (short *)dst, n);
  else if (srcFmt->channels == 2)
    _rwaWaveConvXADPCMDecodeStereo(src, (short *)dst, n);
  return bytes;
}
