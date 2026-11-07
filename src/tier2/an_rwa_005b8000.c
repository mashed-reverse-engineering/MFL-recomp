/* rwa_wave_dict_get_wave_count_005b8000 (0x005b8000): wave dictionaries (RwaWaveDictGetWaveCount): number of waves in the dictionary's wave list (dict + 0xc) */
#include "mfl_tier2.h"

/*005be5a0*/ extern int RwaLListGetCount(void *list);

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_wave_dict_get_wave_count_005b8000(char *dict)
{
  return RwaLListGetCount(dict + 0xc);
}
