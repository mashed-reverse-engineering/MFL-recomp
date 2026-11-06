/* rwa_wave_alloc_data_005bb390 (0x005bb390): waves: allocate the sample data (+0x34 bytes) and default the target format to the source format */
#include "mfl_tier2.h"

/*005bf130*/ extern void *FUN_005bf130(RwUInt32 count, RwUInt32 size, RwUInt32 hint);      /* RWA calloc */

typedef struct RwaWaveFormat { RwUInt32 w[7]; } RwaWaveFormat;      /* 0x1c bytes */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_alloc_data_005bb390(char *wave)
{
  void *data;

  *(RwUInt32 *)(wave + 0x50) = 0;
  data = FUN_005bf130(1, *(RwUInt32 *)(wave + 0x34), 0x30806);
  *(void **)(wave + 0x4c) = data;
  if (!data)
    return 0;
  if (*(RwUInt32 *)(wave + 0x30) == 0)
    *(RwaWaveFormat *)(wave + 0x2c) = *(RwaWaveFormat *)(wave + 0x10);
  return wave;
}
