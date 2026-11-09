/* rwResourcesOpen (MFL.exe 0x004ea720): RenderWare Graphics 3.7 (Criterion Software), opens the resources module: stores the
 * module's globals offset, allocates the resources arena heap and sets up the two entry lists.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/baresour.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN / RWMESSAGE expanded (release build), the static ResourcesInit
 * inlined (as the exe has it), RWERROR expanded as the release build has it (_rwerror + RwErrorSet), RWRESOURCESGLOBAL / RWSRCGLOBAL
 * (resArenaInitSize) expanded to the byte offset into *RwEngineInstance, rwLinkListInitialize expanded, the structs declared here
 * (field order from the exe, not from codfh baresour.h) with compile-time layout asserts, the exe's insufficient-resources error
 * code is 0xc (not the E_RW_* constant of codfh).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;      /* offset of the module's globals in RwGlobals */
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
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);
RW_LAYOUT_ASSERT(rw_assert_res_memheap, RW_OFFSETOF(rwResources, memHeap) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_res_entriesA, RW_OFFSETOF(rwResources, entriesA) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_res_entriesB, RW_OFFSETOF(rwResources, entriesB) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_res_free, RW_OFFSETOF(rwResources, freeEntries) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_res_used, RW_OFFSETOF(rwResources, usedEntries) == 0x24);

#define E_RW_NOMEM ((RwInt32)0x80000013)
#define rwID_COREPLUGIN 0x01
#define RW_RESARENAINITSIZE_OFFSET 0x128      /* RwGlobals.resArenaInitSize */
#define RES_HINT 0x4040b      /* rwMEMHINTDUR_GLOBAL | rwID_RESOURCESMODULE */

/*007d2128*/ extern void *RwEngineInstance;
/*00947138*/ extern RwModuleInfo resourcesModule;
/*004f30c0*/ extern RwBool __cdecl _rwResHeapInit(void *heap, RwUInt32 size);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
#define RWRESOURCESGLOBAL_res (((rwResourcesGlobals *)((RwUInt8 *)RwEngineInstance + resourcesModule.globalsOffset))->res)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwResourcesOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    RwUInt32 arenaSize;
    rwResources *res;

    /* Grab the globals offset (same for all instances) */
    resourcesModule.globalsOffset = offset;

    arenaSize = *(RwUInt32 *)((RwUInt8 *)RwEngineInstance + RW_RESARENAINITSIZE_OFFSET);
    res = &RWRESOURCESGLOBAL_res;

    if (arenaSize > 0)
    {
        /* Set up a single block of memory for the resources heap */
        res->memHeap = RwMalloc(arenaSize, RES_HINT);
        if (!res->memHeap)
        {
            RwError error;

            error.pluginID = rwID_COREPLUGIN;
            error.errorCode = _rwerror(E_RW_NOMEM, arenaSize);
            RwErrorSet(&error);
            return 0;
        }

        if (!_rwResHeapInit(res->memHeap, arenaSize))
        {
            RwError error;

            RwFree(res->memHeap);
            error.pluginID = rwID_COREPLUGIN;
            error.errorCode = _rwerror(0xc, 0);
            RwErrorSet(&error);
            return 0;
        }
    }
    else
    {
        res->memHeap = 0;
    }

    res->entriesA.link.next = &res->entriesA.link;
    res->entriesA.link.prev = &res->entriesA.link;
    res->entriesB.link.next = &res->entriesB.link;
    res->entriesB.link.prev = &res->entriesB.link;

    res->usedEntries = &res->entriesA;
    res->freeEntries = &res->entriesB;

    res->maxSize = arenaSize;
    res->currentSize = 0;
    res->reusageSize = 0;

    /* One more module instance */
    ++resourcesModule.numInstances;

    return instance;
}
