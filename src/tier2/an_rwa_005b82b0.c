/* rwa_wave_dict_stream_read_005b82b0 (0x005b82b0): wave dictionary: read (or append to) a dictionary from a RenderWare stream */
#include "mfl_tier2.h"

/*005bb6c0*/ extern int _rwaRwStreamReadChunkHeaderInfoSync(void *stream, void *header, int swap);
/*005bb750*/ extern int _rwaRwStreamFindChunkSync(void *stream, RwUInt32 type, RwUInt32 *size, RwUInt32 *version, int swap);
/*004dbe70*/ extern RwUInt32 RwStreamRead(void *stream, void *buffer, RwUInt32 length);
/*004dc190*/ extern void *RwStreamSkip(void *stream, RwUInt32 offset);
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bf0e0*/ extern void *FUN_005bf0e0(RwUInt32 size, RwUInt32 hint);      /* RWA aligned malloc */
/*005bf230*/ extern void rwa_aligned_free_005bf230(void *block);
/*005bf320*/ extern void FUN_005bf320(void *data, RwUInt32 bytes);      /* byte-swap 32-bit words */
/*005b8090*/ extern int _rwaWaveDictCreateFromPack(char *dict, int swap);
/*005b81c0*/ extern void RwaWaveDictDestroy(char *dict);
/*005bc950*/ extern void *FUN_005bc950(void *stream, void *a2, void *a3, void *a4, RwUInt32 flags);      /* read one wave into the current dictionary */
/*005be4e0*/ extern void RwaLListRemoveData(void *list, void *data);
/*005bc0a0*/ extern void RwaWaveDestroy(void *wave);
/*007dabfc*/ extern char *g_data_7dabfc;      /* the dictionary waves are read into */
/*007dabe8*/ extern void *g_sound_7dabe8;      /* shared 64 KB read buffer */
/*007dabec*/ extern RwUInt32 g_sound_7dabec;      /* its size */

#define rwaID_WAVEDICT   0x80a
#define rwaID_WAVELIST   0x80c
#define rwaID_WAVE       0x802
#define RWA_HINT         0x30808
#define READ_BUFFER_SIZE 0x10000
#define FLAG_SWAP        0x10      /* flags: the stream's data is byte-swapped */

typedef struct RwaChunkHeader { RwUInt32 type, length, version, buildNum, isComplex; } RwaChunkHeader;      /* RwChunkHeaderInfo, 0x14 */
typedef struct RwaListLink { struct RwaListLink *next, *prev; void *data; } RwaListLink;      /* dict+0x0c: list of waves */

/* dict NULL: create the dictionary from the stream's pack chunk; else skip the pack and add the waves to dict.  Then the wave
   list: count, then every wave chunk (a2..a4 / flags passed to the wave reader).  On a wave error every wave of the dictionary
   is destroyed (and a created dictionary too).  Returns the dictionary or NULL. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_dict_stream_read_005b82b0(char *dict, void *stream, void *a3, void *a4, void *a5, RwUInt32 flags)
{
  struct {      /* the locals in the original's frame order (stubbed callees see the words behind their arguments) */
    char *saved;      /* the previous current dictionary */
    RwUInt32 count;
    int created;
    void *buffer;      /* our own read buffer, when there was none */
    int missing;
    RwUInt32 i;
    RwaChunkHeader hdr;
  } f;
  int swap = (flags >> 4) & 1;

  f.created = 0;
  if (!_rwaRwStreamReadChunkHeaderInfoSync(stream, &f.hdr, swap) || f.hdr.type != rwaID_WAVEDICT || f.hdr.version == 0)
    return 0;
  if (dict == 0) {
    dict = (char *)FUN_005bf0d0(f.hdr.length, RWA_HINT);
    if (!dict)
      return 0;
    if (RwStreamRead(stream, dict, f.hdr.length) != f.hdr.length) {
      FUN_005bf120(dict);
      return 0;
    }
    swap = (*(unsigned char *)(dict + 0x1b) >> 3) & 1;      /* the pack says how its data is stored */
    if (swap)
      flags |= FLAG_SWAP;
    if (!_rwaWaveDictCreateFromPack(dict, swap)) {
      FUN_005bf120(dict);
      return 0;
    }
    f.created = 1;
  } else if (!RwStreamSkip(stream, f.hdr.length))
    return 0;

  if (!_rwaRwStreamReadChunkHeaderInfoSync(stream, &f.hdr, swap) || f.hdr.type != rwaID_WAVELIST || f.hdr.version == 0) {
    if (f.created)
      RwaWaveDictDestroy(dict);
    return 0;
  }
  f.missing = 0;
  f.buffer = 0;
  f.saved = g_data_7dabfc;
  g_data_7dabfc = dict;
  if (RwStreamRead(stream, &f.count, 4) != 4) {
    if (f.created) {      /* (an appended-to dictionary stays current: as the original) */
      RwaWaveDictDestroy(dict);
      g_data_7dabfc = f.saved;
    }
    return 0;
  }
  if (swap)
    FUN_005bf320(&f.count, 4);
  if (!g_sound_7dabe8) {
    f.buffer = FUN_005bf0e0(READ_BUFFER_SIZE, RWA_HINT);
    if (!f.buffer) {
      if (f.created) {
        RwaWaveDictDestroy(dict);
        g_data_7dabfc = f.saved;
      }
      return 0;
    }
    g_sound_7dabe8 = f.buffer;
    g_sound_7dabec = READ_BUFFER_SIZE;
  }
  for (f.i = 0; f.i < f.count; f.i++) {
    if (!_rwaRwStreamFindChunkSync(stream, rwaID_WAVE, 0, 0, swap))
      f.missing = 1;
    if (!FUN_005bc950(stream, a4, a3, a5, flags) || f.missing) {
      RwaListLink *head = (RwaListLink *)(dict + 0xc);

      while (head->prev != head) {      /* destroy every wave read so far */
        void *wave = head->prev->data;

        RwaLListRemoveData(head, wave);
        RwaWaveDestroy(wave);
      }
      if (f.created) {
        RwaWaveDictDestroy(dict);
        g_data_7dabfc = f.saved;
      }
      if (f.buffer) {
        g_sound_7dabe8 = 0;
        g_sound_7dabec = 0;
        rwa_aligned_free_005bf230(f.buffer);
      }
      return 0;
    }
  }
  if (f.buffer) {
    g_sound_7dabe8 = 0;
    g_sound_7dabec = 0;
    rwa_aligned_free_005bf230(f.buffer);
  }
  g_data_7dabfc = f.saved;
  return dict;
}
