// cluster-entries: rwa_wave_free_005bc060 rwa_wave_create_005bc290
/* Installed cluster rwa_wave_free_005bc060 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_wave_free_005bc060.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Wave objects (RWA core): the wave block free (register function, wave in eax) and its only caller, the wave constructor.
   Cluster: rwa_wave_free_005bc060 is entered through the generated eax thunk. */
#include <string.h>
#include "mfl_tier2.h"

/*005bed80*/ extern void RwaFreeListFree(void *list, void *block);
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bcfc0*/ extern int _rwaWaveDefIsTargetFormatSupported(char *def, void *format);
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, void *format, int a3);
/*005bcbf0*/ extern int RwaWaveFormatCompare(const void *a, const void *b);
/*005c4af0*/ extern RwUInt32 rwa_wave_conv_get_target_size_005c4af0(void *format, int a2, void *target);
/*005bec50*/ extern void *rwa_unit243670_005bec50(void *list, RwUInt32 hint);      /* free list: allocate */
/*005bef40*/ extern void *RwaFreeListCreate(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwUInt32 a4, void *a5, RwUInt32 hint);
/*005be6b0*/ extern void _rwaUniqueIDInitialize(void *wave);
/*005b8050*/ extern void *RwaWaveDictGetCurrent(void *wave);
/*005b8010*/ extern void RwaWaveDictRemoveWave(void *dict, void *wave);
/*007db778*/ extern undefined g_data_7db778;      /* the shared wave free list */

#define RWA_HINT 0x30806
/* wave+0x54 */
#define WAVE_FROMLIST 0x1      /* the wave came from a free list */
#define WAVE_LOOPED   0x2
#define WAVE_INBLOCK  0x4      /* the wave lives in the caller's block: never freed here */
#define WAVE_STREAMED 0x8

typedef struct RwaWaveFormat {      /* 0x1c bytes */
  RwUInt32 w0, w4, size;
  unsigned char b0c, channels, b0e, b0f;
  void *extra;      /* +0x10 extra format data */
  RwUInt32 extraBytes;      /* +0x14 */
  unsigned char b18, b19, b1a, b1b;
} RwaWaveFormat;

/* format init as RWA's macro does it (the words that follow the fields are written by the struct copy anyway) */
#define WAVEFORMAT_INIT(f) ((f)->w0 = 0, (f)->w4 = 0, (f)->size = 0, (f)->b0c = 0, (f)->channels = 0, (f)->b18 = 0, \
                            (f)->b19 = 0, (f)->extra = 0, (f)->extraBytes = 0)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_wave_free_005bc060_n(char *wave)
{
  RwUInt32 flags = *(RwUInt32 *)(wave + 0x54);

  if (flags & WAVE_FROMLIST) {
    void *list = *(void **)(*(char **)(wave + 0xc) + 0x3c);      /* the definition's own list, else the shared one */
    RwaFreeListFree(list ? list : &g_data_7db778, wave);
  } else if (!(flags & WAVE_INBLOCK))
    FUN_005bf120(wave);
}

/* def / obj: the wave definition and its object; format: the source format; target: the wave's format (NULL or empty:
   the definition's default with the source's id and extra data); flags: WAVE_LOOPED / WAVE_STREAMED; block: memory for
   the wave (else from the definition's free list).  The definition's create callback (+0x0c) may refuse.  Returns the wave. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_create_005bc290(char *def, void *obj, RwaWaveFormat *format, RwaWaveFormat *target, RwUInt32 flags, char *block)
{
  RwaWaveFormat deflt;
  char *wave;
  RwaWaveFormat *wf;

  if (target && target->w4 && *(RwUInt32 *)(def + 0x24) > 0) {
    if (!_rwaWaveDefIsTargetFormatSupported(def, target))
      return 0;
  } else {
    void *id = *(void **)(def + 0x28) ? (*(void ***)(def + 0x28))[*(int *)(def + 0x2c)] : 0;

    if (id) {
      _rwaWaveDefFormatCopyToWaveFormat(id, &deflt, 1);
      deflt.w0 = format->w0;
      deflt.extra = format->extra;
      deflt.extraBytes = format->extraBytes;
      target = &deflt;
    } else if (!target)
      target = format;
  }
  if (RwaWaveFormatCompare(format, target)) {
    target->extra = format->extra;
    target->extraBytes = format->extraBytes;
  }
  target->size = rwa_wave_conv_get_target_size_005c4af0(format, 0, target);

  if (block) {
    wave = block;
    *(RwUInt32 *)(wave + 0x54) = WAVE_INBLOCK;
  } else {
    RwUInt32 extra = *(RwUInt32 *)(def + 0x34);      /* the definition's bytes behind the 0x5c-byte wave */

    if (!extra)
      wave = (char *)rwa_unit243670_005bec50(&g_data_7db778, RWA_HINT);
    else {
      if (!*(void **)(def + 0x3c)) {
        *(void **)(def + 0x3c) = RwaFreeListCreate(extra + 0x5c, 8, 1, 1, 0, RWA_HINT);
        if (!*(void **)(def + 0x3c))
          return 0;
      }
      wave = (char *)rwa_unit243670_005bec50(*(void **)(def + 0x3c), RWA_HINT);
    }
    if (!wave)
      return 0;
    *(RwUInt32 *)(wave + 0x54) = WAVE_FROMLIST;
  }

  _rwaUniqueIDInitialize(wave);
  *(char **)(wave + 0xc) = def;
  *(int *)(wave + 0x4c) = 0;
  *(int *)(wave + 0x48) = 0;
  *(void **)(wave + 0x58) = obj;
  wf = (RwaWaveFormat *)(wave + 0x10);      /* source format, with an own copy of the extra data */
  WAVEFORMAT_INIT(wf);
  *wf = *format;
  if (format->extra && format->extraBytes > 0) {
    wf->extra = FUN_005bf0d0(format->extraBytes, RWA_HINT);
    if (!wf->extra)
      return 0;
    memcpy(wf->extra, format->extra, format->extraBytes);
  }
  wf = (RwaWaveFormat *)(wave + 0x2c);      /* the wave's format, the same */
  WAVEFORMAT_INIT(wf);
  *wf = *target;
  if (target->extra && target->extraBytes > 0) {
    wf->extra = FUN_005bf0d0(target->extraBytes, RWA_HINT);
    if (!wf->extra)
      return 0;
    memcpy(wf->extra, target->extra, target->extraBytes);
  }
  if (wf->channels == 1)
    *(RwUInt32 *)(wave + 0x54) |= (flags & 1) ? WAVE_LOOPED : (flags & WAVE_LOOPED);
  *(RwUInt32 *)(wave + 0x54) |= flags & WAVE_STREAMED;
  if (*(void **)(*(char **)(wave + 0xc) + 0x34))
    *(char **)(wave + 0x50) = wave + 0x5c;
  {
    int (*create)(char *, void *) = *(int (**)(char *, void *))(*(char **)(wave + 0xc) + 0xc);

    if (create && !create(wave, obj)) {
      rwa_wave_free_005bc060_n(wave);
      return 0;
    }
  }
  RwaWaveDictRemoveWave(RwaWaveDictGetCurrent(wave), wave);      /* (symbol name from the oracle: it files the wave in the current dictionary) */
  return wave;
}

/* entry thunk at the original address of rwa_wave_free_005bc060: register inputs (eax) -> natural C rwa_wave_free_005bc060_n; ret 0 */
__declspec(naked) void rwa_wave_free_005bc060(void)
{
    __asm {
        push eax
        call rwa_wave_free_005bc060_n
        add esp, 4
        ret
    }
}
