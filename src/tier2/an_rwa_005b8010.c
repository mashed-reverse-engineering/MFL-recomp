/* rwa_wave_dict_add_wave_005b8010 (0x005b8010): wave dictionaries: add WAVE to the dictionary's wave list (dict + 0xc) unless it is already there; DICT, or 0 when the list add fails */
#include "mfl_tier2.h"

/*005be530*/ extern int RwaLListFindDataIndex(void *list, void *data);      /* -1: not in the list */
/*005be4a0*/ extern void *RwaLListAddData(void *list, void *data);         /* 0: out of memory */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
char *rwa_wave_dict_add_wave_005b8010(char *dict, void *wave)
{
  char *list = dict + 0xc;

  if (RwaLListFindDataIndex(list, wave) != -1 && dict)
    return dict;
  return RwaLListAddData(list, wave) ? dict : 0;
}
