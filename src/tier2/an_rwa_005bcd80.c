/* rwa_wave_def_find_by_obj_callback_005bcd80 (0x005bcd80): wave definitions (_rwaWaveDefFindByObjCallback): enum callback, look in DEF's wave list for the wave whose key equals *SEARCH->key; found: SEARCH->found = it, 1 (stop) */
#include "mfl_tier2.h"

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;      /* wave link at wave + 0x40, key at wave + 0x30 */

typedef struct RwaWaveSearch {
  void **key;
  char *found;
} RwaWaveSearch;

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_wave_def_find_by_obj_callback_005bcd80(void *parent, char *def, RwaWaveSearch *search)
{
  RwaLink *head = (RwaLink *)(def + 0x20), *l = head->next;
  void *key;

  if (l == head)
    return 0;
  key = *search->key;
  do {
    if (*(void **)((char *)l - 0x10) == key) {
      search->found = (char *)l - 0x40;
      return 1;
    }
    l = l->next;
  } while (l != head);
  return 0;
}
