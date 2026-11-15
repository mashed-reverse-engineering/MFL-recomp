/* RtDictDestroy (MFL.exe 00569300). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/dict/rtdict.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSList RwSList;
extern RwBool _rwSListDestroy(RwSList *sList);
extern void _rwSListDestroyEndEntries(RwSList *sList, RwInt32 amount);
extern void *_rwSListGetEntry(RwSList *sList, RwInt32 entry);
extern RwInt32 _rwSListGetNumEntries(const RwSList *sList);
extern void *_rwSListGetBegin(RwSList *sList);
extern void *_rwSListGetEnd(RwSList *sList);
#define rwSListDestroy(sList) _rwSListDestroy(sList)
#define rwSListDestroyEndEntries(sList,amount) _rwSListDestroyEndEntries(sList, amount)
#define rwSListGetEntry(sList,entry) _rwSListGetEntry(sList, entry)
#define rwSListGetNumEntries(sList) _rwSListGetNumEntries(sList)
#define rwSListGetBegin(sList) _rwSListGetBegin(sList)
#define rwSListGetEnd(sList) _rwSListGetEnd(sList)
typedef struct RtDictSchema RtDictSchema;
typedef struct RtDict RtDict;
typedef void *RtDictEntryType;
typedef const void *RtDictEntryConstType;
typedef void *(RtDictEntryAddRefCallBack)(RtDictEntryType entry);
typedef RwBool (RtDictEntryDestroyCallBack)(RtDictEntryType entry);
typedef const RwChar *(RtDictEntryGetNameCallBack)(RtDictEntryConstType entry);
typedef RwUInt32 (RtDictEntryStreamGetSizeCallBack)(RtDictEntryConstType entry);
typedef RtDictEntryType (RtDictEntryStreamReadCallBack)(RwStream *stream);
typedef RtDict *(RtDictStreamReadCompatibilityCallBack)(RwStream *stream);
typedef const RtDictEntryType (RtDictEntryStreamWriteCallBack)(RtDictEntryConstType entry, RwStream *stream);
struct RtDict
{
    RtDictSchema *schema;

    RwSList *entries;

};
struct RtDictSchema
{
    const RwChar *name;

    RwUInt32 dictChunkType;

    RwUInt32 entryChunkType;

    RwUInt32 compatibilityVersion;

    RwSList *dictionaries;

    RtDict *current;

    RtDictEntryAddRefCallBack *addRefCB;

    RtDictEntryDestroyCallBack *destroyCB;

    RtDictEntryGetNameCallBack *getNameCB;

    RtDictEntryStreamGetSizeCallBack *streamGetSizeCB;

    RtDictEntryStreamReadCallBack *streamReadCB;

    RtDictStreamReadCompatibilityCallBack *streamReadCompatibilityCB;

    RtDictEntryStreamWriteCallBack *streamWriteCB;

};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RtDictSchema, getNameCB) == 0x20);      /* an_codfh_RtDictFindNamedEntry.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RtDict, entries) == 4);      /* an_codfh_RtDictFindNamedEntry.c */
/* ---- end of the generated declarations ---- */

#define IteratorInit(_iter,_list) MACRO_START { (_iter).end = _rwSListGetEnd((_list)); (_iter).cur = _rwSListGetBegin((_list)); } MACRO_STOP
#define IteratorValid(_iter) ((_iter).cur != (_iter).end)
#define IteratorNext(_iter) ((_iter).cur++)
#define IteratorGetEntry(_iter) (*(RtDictEntryType *)((_iter).cur))
typedef union _rwPtrDictEntry _rwPtrDictEntry;
union _rwPtrDictEntry
{
    RtDictEntryType       ptrEntry;
    RtDictEntryConstType constptrEntry;
};
typedef struct Iterator Iterator;
struct Iterator
{
    void **cur;
    void **end;
};
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=static */
RwBool RtDictDestroy(RtDict *dict)
{

    if (dict == dict->schema->current)
    {
        dict->schema->current = (RtDict *) NULL;
    }

    /* RtDictSchemaRemoveDict(dict->schema, dict), static in rtdict.c, inlined in MFL.exe (0x569315..0x569362): the last entry of
     * the schema's dictionary list moves into the removed one's slot */
    {
        RtDictSchema *schema = dict->schema;
        RtDict **current = (RtDict **)(rwSListGetBegin(schema->dictionaries));
        RtDict **end = (RtDict **)(rwSListGetEnd(schema->dictionaries));

        while (current!=end)
        {
            if (dict == *current)
            {
                *current = *(RtDict **)rwSListGetEntry(
                                        schema->dictionaries,
                                        rwSListGetNumEntries(schema->dictionaries)-1);
                rwSListDestroyEndEntries(schema->dictionaries, 1);
                break;
            }
            ++current;
        }
    }

    /* _rtDictDestruct(dict), static in rtdict.c, inlined in MFL.exe (0x569365..0x56939d): every entry goes to the schema's
     * destroy callback, then the entry list is destroyed */
    {
        Iterator    iter;

        IteratorInit(iter, dict->entries);
        while (IteratorValid(iter))
        {
            _rwPtrDictEntry ptrDictEntry;

            ptrDictEntry.ptrEntry = IteratorGetEntry(iter);

            IteratorNext(iter);

            (dict->schema->destroyCB)(ptrDictEntry.ptrEntry);
        }

        rwSListDestroy(dict->entries);
    }

    RwFree(dict);

    return (TRUE);
}
