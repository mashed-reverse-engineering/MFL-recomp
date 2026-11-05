/* rwa_wave_def_create_005bd230 (0x005bd230): wave definitions (RwaWaveDefCreate): an empty wave definition filed under its object definition */
#include "mfl_tier2.h"

/*005bec50*/ extern void *rwa_unit243670_005bec50(void *list, RwUInt32 hint);      /* free list: allocate */
/*005be6b0*/ extern void _rwaUniqueIDInitialize(void *id);
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*007db7a0*/ extern undefined g_data_7db7a0;      /* wave definitions free list */
/*00949d5c*/ extern char *g_data_949d5c;          /* the NULL object definition */

#define RWA_HINT 0x30805
/* def+0x48 */
#define WAVEDEF_OWNSFORMATS 0x1      /* +0x28 was allocated here */
#define WAVEDEF_FROMLIST    0x2

/* objDef: the owning object definition (NULL: the NULL definition); mem: memory for the definition (else from the free
   list).  Returns the definition, NULL when the free list is empty. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_wave_def_create_005bd230(char *objDef, char *mem)
{
  char *def = mem;
  RwaLink *link, *list;

  if (!def) {
    def = (char *)rwa_unit243670_005bec50(&g_data_7db7a0, RWA_HINT);
    if (!def)
      return 0;
    *(RwUInt32 *)(def + 0x48) = WAVEDEF_FROMLIST;
  } else
    *(RwUInt32 *)(def + 0x48) = 0;
  _rwaUniqueIDInitialize(def);
  *(void **)(def + 0x0c) = 0;      /* callbacks */
  *(void **)(def + 0x10) = 0;
  *(void **)(def + 0x14) = 0;
  *(void **)(def + 0x1c) = 0;
  *(void **)(def + 0x18) = 0;
  *(void **)(def + 0x20) = 0;
  *(RwUInt32 *)(def + 0x34) = 0;
  *(RwUInt32 *)(def + 0x38) = 4;
  *(void **)(def + 0x3c) = 0;
  if (*(RwUInt32 *)(def + 0x48) & WAVEDEF_OWNSFORMATS) {
    if (*(void **)(def + 0x28))
      FUN_005bf120(*(void **)(def + 0x28));
    *(RwUInt32 *)(def + 0x48) &= ~WAVEDEF_OWNSFORMATS;
  }
  *(void **)(def + 0x28) = 0;      /* supported formats */
  *(RwUInt32 *)(def + 0x24) = 0;
  *(RwUInt32 *)(def + 0x2c) = 0;
  link = (RwaLink *)(def + 0x40);
  link->prev = 0;
  link->next = 0;
  if (!objDef)
    objDef = g_data_949d5c;
  *(char **)(def + 0x30) = objDef;
  list = (RwaLink *)(objDef + 0x20);      /* the object definition's wave definitions */
  link->prev = list;
  link->next = list->next;
  list->next->prev = link;
  list->next = link;
  return def;
}
