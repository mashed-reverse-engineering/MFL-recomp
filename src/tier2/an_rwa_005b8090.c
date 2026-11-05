/* rwa_wave_dict_create_from_pack_005b8090 (0x005b8090): wave dictionaries (_rwaWaveDictCreateFromPack): set up a dictionary read from a stream (empty wave list, its uuid / name from the data behind the header) and link it into the dictionary list */
#include "mfl_tier2.h"

/*005be6c0*/ extern void _rwaUUIDSerialize(void *dst, const void *src, int swap);
/*005be810*/ extern void *rwa_unique_id_set_uuid_005be810(void *id, void *uuid);
/*005be7d0*/ extern void *rwa_unique_id_set_name_005be7d0(void *id, char *name);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;
typedef struct RwaRing { struct RwaRing *next, *prev; } RwaRing;

typedef struct RwaWaveDict {
  void *uuid;        /* +0x00 unique id: uuid, name, flags */
  char *name;        /* +0x04 */
  RwUInt32 idFlags;  /* +0x08 */
  RwaRing waves;     /* +0x0c */
  RwUInt32 count;    /* +0x14 */
  RwUInt32 w18;
  RwaLink link;      /* +0x1c: in the dictionary list */
  char data[1];      /* +0x24: serialised uuid (16 bytes, when there is one), then the name */
} RwaWaveDict;

/*007dabbc*/ extern RwaLink *g_data_7dabbc;      /* the dictionary list */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaWaveDict *rwa_wave_dict_create_from_pack_005b8090(RwaWaveDict *dict, int swap)
{
  char *p = dict->data;

  dict->waves.next = &dict->waves;
  dict->waves.prev = &dict->waves;
  dict->count = 0;
  if (dict->uuid) {
    _rwaUUIDSerialize(p, p, swap);
    rwa_unique_id_set_uuid_005be810(dict, p);
    p += 16;
  }
  if (dict->name) {
    rwa_unique_id_set_name_005be7d0(dict, p);
    rwstrlen(p);      /* length unused, as in the original */
  }
  dict->link.prev = 0;
  dict->link.next = 0;
  dict->link.next = g_data_7dabbc;      /* insert at the head */
  dict->link.prev = &g_data_7dabbc;
  g_data_7dabbc->prev = (RwaLink **)&dict->link;
  g_data_7dabbc = &dict->link;
  return dict;
}
