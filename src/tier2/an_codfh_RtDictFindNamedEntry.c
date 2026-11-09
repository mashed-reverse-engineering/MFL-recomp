/* RtDictFindNamedEntry (MFL.exe 0x00569280): RenderWare Graphics 3.7 (Criterion Software), looks an entry up by name in a
 * dictionary (linear scan with the schema's get-name callback).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/dict/rtdict.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), the Iterator macros written out,
 * rwstrcmp expanded to RwGlobals.stringFuncs.vecStrcmp (0xe8; its slot is taken before the get-name call, as the exe does), the
 * structs declared here (codfh rtdict.h) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef const RwChar *(*RtDictEntryGetNameCallBack)(const void *entry);
typedef int (*RwStrcmpFn)(const RwChar *string1, const RwChar *string2);
typedef struct RtDictSchemaHead {
    const RwChar *name;
    RwUInt32 dictChunkType;
    RwUInt32 entryChunkType;
    RwUInt32 compatibilityVersion;
    void *dictionaries;                 /* RwSList * */
    void *current;                      /* RtDict * */
    void *addRefCB;
    void *destroyCB;
    RtDictEntryGetNameCallBack getNameCB;      /* 0x20 */
} RtDictSchemaHead;
typedef struct RtDict {
    RtDictSchemaHead *schema;
    void *entries;                      /* RwSList * of entry pointers */
} RtDict;
RW_LAYOUT_ASSERT(rw_assert_schema_getname, RW_OFFSETOF(RtDictSchemaHead, getNameCB) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_dict_entries, RW_OFFSETOF(RtDict, entries) == 4);

#define RW_GLOBALS_STRCMP 0xe8      /* RwGlobals.stringFuncs (0xc4) .vecStrcmp (10th pointer) */

/*007d2128*/ extern void *RwEngineInstance;
/*005d60f0*/ extern void **_rwSListGetEnd(void *sList);
/*005543d0*/ extern void **rwSListGetBegin(void *sList);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *RtDictFindNamedEntry(RtDict *dict, const RwChar *name)
{
    RtDictEntryGetNameCallBack getNameCB = dict->schema->getNameCB;
    void **end = _rwSListGetEnd(dict->entries);
    void **cur = rwSListGetBegin(dict->entries);

    while (cur != end)
    {
        void *entry = *cur;
        RwStrcmpFn *strcmpSlot = (RwStrcmpFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRCMP);

        if (0 == (*strcmpSlot)(getNameCB(entry), name))
        {
            return entry;
        }
        cur++;
    }

    /* Not found */
    return NULL;
}
