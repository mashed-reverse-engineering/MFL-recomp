/* rwa_wave_def_find_by_uuid_callback_005bce50 (0x005bce50): wave definitions (_RwaWaveDefFindByUUIDCallback): search one module's list for the uuid in query[0], result in query[1] */
#include "mfl_tier2.h"

/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0: equal */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_wave_def_find_by_uuid_callback_005bce50(void *unused, char *module, void **query)
{
  RwaLink *head = (RwaLink *)(module + 0x20), *p;

  for (p = head->next; p != head; p = p->next)
    if (!RwaUUIDCompare(*(void **)((char *)p - 0x40), query[0])) {
      query[1] = (char *)p - 0x40;
      return 1;
    }
  return 0;
}
