/* rwa_wave_destroy_005bc0a0 (0x005bc0a0): waves (RwaWaveDestroy): run the definition's destroy hook, free both formats' misc data, leave every dictionary, free the uuid, then release the wave memory (free list, heap or not owned) */
#include "mfl_tier2.h"

typedef void (*RwaWaveHook)(char *wave);

/*005bcd50*/ extern void _rwaWaveFormatFreeMiscData(void *format);
/*005b8060*/ extern void RwaWaveDictRemoveAllWave(void *wave);
/*005be850*/ extern void *_rwaUniqueIDFreeData(void *id);
/*005bed80*/ extern void FUN_005bed80(void *freelist, void *p);      /* RwaFreeListFree */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*007db778*/ extern undefined g_data_7db778;      /* the shared wave free list */

#define WAVE_DEF(w) (*(char **)((w) + 0xc))
#define WAVE_FLAGS(w) (*(RwUInt32 *)((w) + 0x54))      /* 1: from a free list, 4: memory not owned */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_wave_destroy_005bc0a0(char *wave)
{
  RwaWaveHook destroy = *(RwaWaveHook *)(WAVE_DEF(wave) + 0x10);
  void *freelist;

  if (destroy)
    destroy(wave);
  _rwaWaveFormatFreeMiscData(wave + 0x10);
  _rwaWaveFormatFreeMiscData(wave + 0x2c);
  RwaWaveDictRemoveAllWave(wave);
  _rwaUniqueIDFreeData(wave);
  if (WAVE_FLAGS(wave) & 1) {
    freelist = *(void **)(WAVE_DEF(wave) + 0x3c);
    FUN_005bed80(freelist ? freelist : &g_data_7db778, wave);
  } else if (!(WAVE_FLAGS(wave) & 4))
    FUN_005bf120(wave);
}
