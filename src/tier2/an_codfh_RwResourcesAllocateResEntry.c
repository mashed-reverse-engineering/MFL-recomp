/* RwResourcesAllocateResEntry (MFL.exe 0x004ea960): RenderWare Graphics 3.7 (Criterion Software), allocates a resources arena entry
 * for an owner, evicting entries from the free list and then the used list until the heap can satisfy the request.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/baresour.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN / RWERROR / RwDebugSendMessage expanded (release build; the
 * messages are gone), the non-RWRESOURCESDYNAMIC path only (heap allocation via _rwResHeapAlloc), RwResourcesFreeResEntry inlined
 * as a static helper (the exe inlines it twice), rwLinkListAddLLLink / RemoveLLLink expanded, RWRESOURCESGLOBAL / RwFree expanded
 * to the byte offset into *RwEngineInstance, the exe's insufficient-resources error code is 0xc, the structs declared here (field
 * order from the exe, codfh baresour.h is not in the tree) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct RwLLLink RwLLLink;
struct RwLLLink {
    RwLLLink *next;
    RwLLLink *prev;
};
typedef struct RwLinkList {
    RwLLLink link;
} RwLinkList;
typedef struct rwResources {
    RwUInt32 maxSize;
    RwUInt32 currentSize;
    RwUInt32 reusageSize;
    void *memHeap;
    RwLinkList entriesA;
    RwLinkList entriesB;
    RwLinkList *freeEntries;
    RwLinkList *usedEntries;
} rwResources;
typedef struct rwResourcesGlobals {
    rwResources res;
} rwResourcesGlobals;
typedef void (*RwResEntryDestroyNotify)(struct RwResEntry *);
struct RwResEntry {
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);
RW_LAYOUT_ASSERT(rw_assert_res_current, RW_OFFSETOF(rwResources, currentSize) == 0x4);
RW_LAYOUT_ASSERT(rw_assert_res_reusage, RW_OFFSETOF(rwResources, reusageSize) == 0x8);
RW_LAYOUT_ASSERT(rw_assert_res_memheap, RW_OFFSETOF(rwResources, memHeap) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_res_free, RW_OFFSETOF(rwResources, freeEntries) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_res_used, RW_OFFSETOF(rwResources, usedEntries) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_entry_size, RW_OFFSETOF(RwResEntry, size) == 0x8);
RW_LAYOUT_ASSERT(rw_assert_entry_owner, RW_OFFSETOF(RwResEntry, owner) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_entry_ownerref, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_entry_destroy, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_entry_sizeof, sizeof(RwResEntry) == 0x18);

#define rwID_COREPLUGIN 0x01

/*007d2128*/ extern void *RwEngineInstance;
/*00947138*/ extern RwModuleInfo resourcesModule;
/*004f3190*/ extern void *__cdecl _rwResHeapAlloc(void *heap, RwUInt32 size);
/*004f3110*/ extern void __cdecl _rwResHeapFree(void *block);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
#define RESG (((rwResourcesGlobals *)((RwUInt8 *)RwEngineInstance + resourcesModule.globalsOffset))->res)

/* RwResourcesFreeResEntry body (the exe inlines it) */
#define FreeResEntry(entry) \
    do { \
        /* About to go away, call the destroy notify if applicable */ \
        if ((entry)->destroyNotify) { (entry)->destroyNotify(entry); } \
        /* unlink user of resource */ \
        if ((entry)->ownerRef) { *((entry)->ownerRef) = (RwResEntry *)0; } \
        if ((entry)->link.next) { \
            /* Remove the link list attachment */ \
            (entry)->link.prev->next = (entry)->link.next; \
            (entry)->link.next->prev = (entry)->link.prev; \
            /* Adjust the current size */ \
            RESG.currentSize -= (entry)->size; \
            /* Free from resources heap */ \
            _rwResHeapFree(entry); \
        } else { \
            /* This entry is not in a list and must be in main memory */ \
            RwFree(entry); \
        } \
    } while (0)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwResEntry *RwResourcesAllocateResEntry(void *owner, RwResEntry **ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify)
{
    RwResEntry *entry;
    RwBool exhaustedOptions;

    exhaustedOptions = 0;
    while (!exhaustedOptions)
    {
        RwLLLink *cur, *end;

        /* Allocate from the resources heap */
        entry = (RwResEntry *)_rwResHeapAlloc(RESG.memHeap, sizeof(RwResEntry) + size);

        if (entry)
        {
            /* We have an entry */
            /* rwLinkListAddLLLink(RWRESOURCESGLOBAL(res.usedEntries), &entry->link): the list expression is re-read at every use */
            entry->link.next = RESG.usedEntries->link.next;
            entry->link.prev = &RESG.usedEntries->link;
            RESG.usedEntries->link.next->prev = &entry->link;
            RESG.usedEntries->link.next = &entry->link;
            entry->owner = owner;
            entry->size = size;
            entry->ownerRef = ownerRef;
            entry->destroyNotify = destroyNotify;

            RESG.currentSize += size;

            if (ownerRef)
            {
                *ownerRef = entry;
            }

            return entry;
        }

        /* Try freeing up some memory from the not used list */
        cur = RESG.freeEntries->link.prev;
        end = &RESG.freeEntries->link;
        if (cur != end)
        {
            /* Free its memory */
            entry = (RwResEntry *)cur;

            FreeResEntry(entry);
        }
        else
        {
            /* Try freeing up some memory from the used list and try to alloc */
            cur = RESG.usedEntries->link.prev;
            end = &RESG.usedEntries->link;
            if (cur != end)
            {
                entry = (RwResEntry *)cur;

                /* Before we free it though, add its size to the usage */
                RESG.reusageSize += entry->size;

                FreeResEntry(entry);
            }
            else
            {
                /* We've emptied both lists, yet still we can't allocate resources */
                exhaustedOptions = 1;
            }
        }
    }

    /* Couldn't do it */
    if (ownerRef)
    {
        *ownerRef = (RwResEntry *)0;
    }
    {
        RwError error;

        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(0xc, size);
        RwErrorSet(&error);
    }

    return 0;
}
