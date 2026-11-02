/* RtDictSchemaStreamReadDict (0x005693c0): RenderWare toolkit, read a dictionary of a schema from a stream */
#include "mfl_tier2.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern int RwStreamFindChunk(void *stream, int type, unsigned int *length, unsigned int *version);
extern void *RwStreamRead(void *stream, void *buffer, unsigned int length);
extern void *FUN_0053eac0();
extern void *RwErrorSet(void *err);
extern void *_rwSListCreate(int size, unsigned int hint);
extern void **_rwSListGetNewEntry(void *list, unsigned int hint);
extern void _rwSListDestroy(void *list);
extern void **FUN_005543d0(void *list);   /* _rwSListGetBegin */
extern int FUN_005543e0(void *list);   /* _rwSListGetNumEntries */
extern void **_rwSListGetEnd(void *list);
extern void **_rwSListGetEntry(void *list, int index);
extern void _rwSListDestroyEndEntries(void *list, int n);

typedef struct RtDictSchema RtDictSchema;
typedef struct RtDict {
  RtDictSchema *schema;   /* 0x0 */
  void *entries;   /* 0x4: SList of the entries */
} RtDict;

struct RtDictSchema
{
  int pad0;
  unsigned int hint;   /* 0x4: memory hint */
  int chunkType;   /* 0x8: chunk id of one entry */
  unsigned int minVersion;   /* 0xc: older streams go to the legacy reader */
  void *dicts;   /* 0x10: SList of the dictionaries read so far */
  RtDict *current;   /* 0x14 */
  void (*addEntry)(void *entry);   /* 0x18, called through the dictionary's schema */
  void (*freeEntry)(void *entry);   /* 0x1c */
  unsigned char pad20[8];
  void *(*readEntry)(void *stream);   /* 0x28 */
  RtDict *(*readLegacy)(void *stream);   /* 0x2c */
};

/* Reads the struct chunk (entry count), then `count` entries (each: a chunk of schema->chunkType read by schema->readEntry) into a new dictionary that is
   registered in schema->dicts.  Any failure unregisters and frees what was built and returns 0.  The entry count is read into the argument slot of
   `stream` (the original reuses it), `s` is the stream. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
RtDict *RtDictSchemaStreamReadDict(RtDictSchema *schema, void *stream)
{
  struct {
    unsigned int version, length;
    int pluginID, errorCode;
  } l;
  void *s = stream;
  RtDict *dict;
  void **slot, **it, **end, **hit;
  void *entry;
  RtDictSchema *sc;

  if (!RwStreamFindChunk(s, 1, &l.length, &l.version))
    return 0;
  if (l.version < schema->minVersion)
    return schema->readLegacy(s);
  if (!RwStreamRead(s, &stream, l.length))
    return 0;
  dict = (RtDict *)RwMalloc(8, schema->hint | 0x30000);
  if (!dict) {
    l.pluginID = 0x1bf;
    l.errorCode = (int)FUN_0053eac0(0x80000013, 8);
    RwErrorSet(&l.pluginID);
    return 0;
  }
  dict->entries = _rwSListCreate(4, schema->hint | 0x30000);
  if (!dict->entries)
    goto free_dict;
  dict->schema = schema;
  slot = _rwSListGetNewEntry(schema->dicts, schema->hint | 0x30000);
  if (!slot) {
    _rwSListDestroy(dict->entries);
    goto free_dict;
  }
  *slot = dict;
  while (stream) {
    stream = (void *)((int)stream - 1);
    if (!RwStreamFindChunk(s, schema->chunkType, &l.length, &l.version))
      goto fail;
    entry = schema->readEntry(s);
    if (!entry)
      goto fail;
    slot = _rwSListGetNewEntry(dict->entries, dict->schema->hint | 0x30000);
    *slot = entry;
    dict->schema->addEntry(entry);
    schema->freeEntry(entry);
  }
  return dict;

fail:
  if (dict == dict->schema->current)
    dict->schema->current = 0;
  sc = dict->schema;
  it = FUN_005543d0(sc->dicts);
  end = _rwSListGetEnd(sc->dicts);
  if (it != end) {
    for (hit = it; hit != end; hit++)
      if ((RtDict *)*hit == dict) {
        *hit = *_rwSListGetEntry(sc->dicts, FUN_005543e0(sc->dicts) - 1);
        _rwSListDestroyEndEntries(sc->dicts, 1);
        break;
      }
  }
  end = _rwSListGetEnd(dict->entries);
  for (it = FUN_005543d0(dict->entries); it != end; it++)
    dict->schema->freeEntry(*it);
  _rwSListDestroy(dict->entries);
free_dict:
  RwFree(dict);
  return 0;
}
