/* rwa_wave_dict_create_005b7ef0 (0x005b7ef0): wave dictionaries (_rwaWaveDictCreate): an empty dictionary in the caller's block or from the dictionary free list, linked into the dictionary list */
#include "mfl_tier2.h"

/*005bec50*/ extern void *rwa_unit243670_005bec50(void *list, RwUInt32 hint);      /* free list: allocate */
/*005be6b0*/ extern void _rwaUniqueIDInitialize(void *obj);

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;
typedef struct RwaRing { struct RwaRing *next, *prev; } RwaRing;

typedef struct RwaWaveDict {
  char uid[0xc];     /* +0x00 unique id: uuid, name, flags */
  RwaRing waves;     /* +0x0c */
  RwUInt32 count;    /* +0x14 */
  unsigned char b18, b19, b1a;
  unsigned char alloc;      /* +0x1b: DICT_FROMLIST / DICT_INBLOCK */
  RwaLink link;      /* +0x1c: in the dictionary list */
} RwaWaveDict;

#define DICT_FROMLIST 1
#define DICT_INBLOCK  4
#define RWA_HINT_WAVEDICT 0x30808
/*007dab98*/ extern char g_data_7dab98[];        /* dictionary free list */
/*007dabbc*/ extern RwaLink *g_data_7dabbc;      /* the dictionary list */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaWaveDict *rwa_wave_dict_create_005b7ef0(RwaWaveDict *block)
{
  RwaWaveDict *dict = block;

  if (dict)
    dict->alloc = DICT_INBLOCK;
  else {
    dict = (RwaWaveDict *)rwa_unit243670_005bec50(g_data_7dab98, RWA_HINT_WAVEDICT);
    if (!dict)
      return 0;
    dict->alloc = DICT_FROMLIST;
  }
  _rwaUniqueIDInitialize(dict);
  dict->link.next = 0;
  dict->link.prev = 0;
  dict->link.next = g_data_7dabbc;      /* insert at the head */
  dict->link.prev = &g_data_7dabbc;
  g_data_7dabbc->prev = (RwaLink **)&dict->link;
  g_data_7dabbc = &dict->link;
  dict->waves.prev = &dict->waves;
  dict->waves.next = &dict->waves;
  dict->count = 0;
  return dict;
}
