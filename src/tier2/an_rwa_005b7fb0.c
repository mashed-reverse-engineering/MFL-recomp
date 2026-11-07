/* rwa_wave_dict_find_wave_by_name_005b7fb0 (0x005b7fb0): wave dictionaries (RwaWaveDictFindWaveByName): first wave of the dictionary whose name equals NAME, or 0 */
#include "mfl_tier2.h"

typedef struct RwaLListEntry { struct RwaLListEntry *next, *prev; void *data; } RwaLListEntry;      /* data list entry, list head at dict + 0xc */

/*005be630*/ extern int rwa_string_compare_005be630(RwChar *a, RwChar *b);      /* 0: equal */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
char *rwa_wave_dict_find_wave_by_name_005b7fb0(char *dict, RwChar *name)
{
  RwaLListEntry *head = (RwaLListEntry *)(dict + 0xc), *e;

  for (e = head->prev; e != head; e = e->prev) {
    char *wave = e->data;
    RwChar *waveName = *(RwChar **)(wave + 4);
    if (waveName && !rwa_string_compare_005be630(waveName, name))
      return wave;
  }
  return 0;
}
