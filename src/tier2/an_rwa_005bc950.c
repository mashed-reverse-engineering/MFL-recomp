/* rwa_wave_stream_read_005bc950 (0x005bc950): wave: read one wave (pack + data chunks) from a RenderWare stream */
#include "mfl_tier2.h"

/*005bb6c0*/ extern int _rwaRwStreamReadChunkHeaderInfoSync(void *stream, void *header, int swap);
/*004dbe70*/ extern RwUInt32 RwStreamRead(void *stream, void *buffer, RwUInt32 length);
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bf0e0*/ extern void *FUN_005bf0e0(RwUInt32 size, RwUInt32 hint);      /* RWA aligned malloc */
/*005bf230*/ extern void rwa_aligned_free_005bf230(void *block);
/*005bc620*/ extern char *_rwaWaveCreateFromPack(void *pack, void *a2, void *a3, void *a4, RwUInt32 flags);
/*005bc520*/ extern int RwaWaveUpload(char *wave, RwUInt32 offset, const void *data, RwUInt32 bytes);
/*005bc0a0*/ extern void RwaWaveDestroy(void *wave);
/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0 = equal */
/*005bf360*/ extern void RwaMemEndianSwap(void *data, RwUInt32 bytes, RwUInt32 wordBytes);
/*007dabe8*/ extern void *g_sound_7dabe8;      /* shared read buffer (set while a dictionary is read) */
/*007dabec*/ extern RwUInt32 g_sound_7dabec;      /* its size */
/*005f7914*/ extern undefined g_data_5f7914;      /* the two wave format uuids that need the whole data in one block */
/*005f7944*/ extern undefined g_data_5f7944;

#define rwaID_WAVEPACK 0x803
#define rwaID_WAVEDATA 0x804
#define RWA_HINT       0x30806

typedef struct RwaChunkHeader { RwUInt32 type, length, version, buildNum, isComplex; } RwaChunkHeader;      /* RwChunkHeaderInfo, 0x14 */
typedef int (*RwaYieldFn)(void);      /* wave+0x0c -> +0x1c: called after every block until it returns != 1 */

/* The pack chunk makes the wave (a2..a4 / flags passed on); the data chunk is uploaded block by block through the shared
   read buffer, or in one block through an own buffer when the wave converts from one of the two block formats (or there is
   no shared buffer).  16-bit and wider samples are byte-swapped when flags say so.  Returns the wave, NULL on a header /
   pack error (the wave is destroyed when the data header is bad); a short read or upload ends the upload but keeps the wave. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_stream_read_005bc950(void *stream, void *a2, void *a3, void *a4, RwUInt32 flags)
{
  struct {      /* the locals in the original's frame order (the stubbed header reader sees the header words) */
    void *buffer;
    int owned;      /* buffer is ours (free it) */
    RwaChunkHeader hdr;
  } f;
  int swap = (flags >> 4) & 1;
  char *pack;
  char *wave;
  int convert, block;
  RwUInt32 off, chunk, total;

  f.buffer = g_sound_7dabe8;
  f.owned = 0;
  if (!_rwaRwStreamReadChunkHeaderInfoSync(stream, &f.hdr, swap) || f.hdr.type != rwaID_WAVEPACK || f.hdr.version == 0)
    return 0;
  pack = (char *)g_sound_7dabe8;
  if (!pack) {
    pack = (char *)FUN_005bf0d0(f.hdr.length, RWA_HINT);
    if (!pack)
      return 0;
  }
  if (RwStreamRead(stream, pack, f.hdr.length) != f.hdr.length)
    return 0;
  wave = _rwaWaveCreateFromPack(pack, a2, a3, a4, flags);
  if (!g_sound_7dabe8)
    FUN_005bf120(pack);
  if (!wave)
    return 0;
  if (!_rwaRwStreamReadChunkHeaderInfoSync(stream, &f.hdr, swap) || f.hdr.type != rwaID_WAVEDATA || f.hdr.version == 0) {
    RwaWaveDestroy(wave);
    return 0;
  }

  convert = RwaUUIDCompare(*(void **)(wave + 0x14), *(void **)(wave + 0x30));      /* source and wave format differ */
  block = !(RwaUUIDCompare(*(void **)(wave + 0x14), &g_data_5f7914) && RwaUUIDCompare(*(void **)(wave + 0x14), &g_data_5f7944) &&
            RwaUUIDCompare(*(void **)(wave + 0x30), &g_data_5f7914) && RwaUUIDCompare(*(void **)(wave + 0x30), &g_data_5f7944));
  if ((convert && block) || !f.buffer) {
    f.buffer = FUN_005bf0e0(f.hdr.length, RWA_HINT);
    f.owned = 1;
  }

  total = f.hdr.length;
  for (off = 0; off < total; off += chunk) {
    chunk = f.hdr.length;
    if (!f.owned) {
      chunk = g_sound_7dabec;
      if (chunk + off > total)
        chunk = total - off;
    }
    if (RwStreamRead(stream, f.buffer, chunk) != chunk)
      break;
    if (swap && *(unsigned char *)(wave + 0x1c) > 8)      /* bits per sample */
      RwaMemEndianSwap(f.buffer, chunk, *(unsigned char *)(wave + 0x1c) >> 3);
    if (!RwaWaveUpload(wave, off, f.buffer, chunk))
      break;
    while (*(RwaYieldFn *)(*(char **)(wave + 0xc) + 0x1c) && (*(RwaYieldFn *)(*(char **)(wave + 0xc) + 0x1c))() == 1)
      ;
  }
  if (f.owned)
    rwa_aligned_free_005bf230(f.buffer);
  return wave;
}
