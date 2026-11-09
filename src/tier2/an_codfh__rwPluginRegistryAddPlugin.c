/* _rwPluginRegistryAddPlugin (MFL.exe 0x004e9ad0): RenderWare Graphics 3.7 (Criterion Software), registers a plugin with a plugin
 * registry: reserves its slice of the object, and appends a registry entry from the toolkit free list.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/batkreg.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN / RWERROR expanded as the release build has them (_rwerror +
 * RwErrorSet), RWSRCGLOBAL(memoryAlloc) / RwMalloc / RwFree / RwFreeListAlloc expanded to the byte offsets into *RwEngineInstance,
 * _rwGetNumEngineInstances and the three default plugin callbacks are the exe's shared stub functions, the structs declared here
 * (field order from the exe, codfh batkreg.h is not in the tree) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegEntry {
    RwInt32 offset;
    RwInt32 size;
    RwUInt32 pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    void *alwaysCB;
    void *rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    void *errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);
RW_LAYOUT_ASSERT(rw_assert_entry_pluginid, RW_OFFSETOF(RwPluginRegEntry, pluginID) == 0x8);
RW_LAYOUT_ASSERT(rw_assert_entry_construct, RW_OFFSETOF(RwPluginRegEntry, constructCB) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_entry_copy, RW_OFFSETOF(RwPluginRegEntry, copyCB) == 0x28);
RW_LAYOUT_ASSERT(rw_assert_entry_next, RW_OFFSETOF(RwPluginRegEntry, nextRegEntry) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_entry_parent, RW_OFFSETOF(RwPluginRegEntry, parentRegistry) == 0x38);
RW_LAYOUT_ASSERT(rw_assert_reg_max, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);
RW_LAYOUT_ASSERT(rw_assert_reg_first, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_reg_last, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);

#define E_RW_PLUGININIT ((RwInt32)0x80000017)
#define rwID_COREPLUGIN 0x01
#define HINT_GLOBAL 0x40000      /* rwMEMHINTDUR_GLOBAL */
#define ENG_MEMORYALLOC 0x118      /* RwGlobals.memoryAlloc (RwFreeListAlloc) */

typedef void *(*RwMemoryAllocFunc)(void *, RwUInt32);

/*007d2128*/ extern void *RwEngineInstance;
/*007d5180*/ extern void *toolkitRegEntries;
/*007d5184*/ extern RwPluginRegistry **toolkitNonFLRegList;
/*007d5188*/ extern RwUInt32 numRegToolkits;
/*004d2f00*/ extern RwInt32 FUN_004d2f00(void);      /* _rwGetNumEngineInstances */
/*004e9340*/ extern void *LAB_004e9340(void *fl, RwUInt32 hint);      /* _rwFreeListAllocReal */
/*0053eac0*/ extern void *FUN_0053eac0(void *, RwInt32, RwInt32);      /* PluginDefaultConstructor / Destructor / Copy (one shared stub) */
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry *reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB,
                                   RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB)
{
    RwPluginRegEntry *entry;
    RwInt32 newStructSize;

    if (!toolkitRegEntries)
    {
        /* Not a good time to do this: no free list, the library has not been initialised */
        return -1;
    }

    /* Make sure that the library has not been opened yet */
    if (FUN_004d2f00() != 0)
    {
        RwError error;

        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_PLUGININIT);
        RwErrorSet(&error);
        return -1;
    }

    /* add the registry to the list if freelists aren't used */
    if (*(RwMemoryAllocFunc *)((RwUInt8 *)RwEngineInstance + ENG_MEMORYALLOC) != (RwMemoryAllocFunc)LAB_004e9340)
    {
        RwUInt32 i;

        for (i = 0; i < numRegToolkits; i += 1)
        {
            if (reg == toolkitNonFLRegList[i])
            {
                break;
            }
        }

        if (numRegToolkits == i)
        {
            RwPluginRegistry **newRegistryList;
            RwUInt32 j;

            newRegistryList = (RwPluginRegistry **)RwMalloc(sizeof(RwPluginRegistry *) * (numRegToolkits + 1), HINT_GLOBAL);

            j = 0;
            if (NULL != toolkitNonFLRegList)
            {
                for (; j < numRegToolkits; j += 1)
                {
                    newRegistryList[j] = toolkitNonFLRegList[j];
                }

                RwFree(toolkitNonFLRegList);
            }

            newRegistryList[j] = reg;

            numRegToolkits += 1;

            toolkitNonFLRegList = newRegistryList;
        }
    }

    /* Make sure it's not in the list yet */
    entry = reg->firstRegEntry;
    while (entry)
    {
        if (entry->pluginID == pluginID)
        {
            /* Can't register twice, return the already registered offset */
            RwError error;

            error.pluginID = rwID_COREPLUGIN;
            error.errorCode = _rwerror(E_RW_PLUGININIT);
            RwErrorSet(&error);
            return entry->offset;
        }
        entry = entry->nextRegEntry;
    }

    /* Increase structure size, but keep longword alignment */
    newStructSize = reg->sizeOfStruct + ((size + 3) & ~3);

    if (reg->maxSizeOfStruct && (newStructSize > reg->maxSizeOfStruct))
    {
        /* We've exceeded our limit, so fail */
        return -1;
    }

    entry = (RwPluginRegEntry *)(*(RwMemoryAllocFunc *)((RwUInt8 *)RwEngineInstance + ENG_MEMORYALLOC))(toolkitRegEntries, HINT_GLOBAL);

    if (entry)
    {
        /* Fill it the entry */
        entry->offset = reg->sizeOfStruct;

        reg->sizeOfStruct = newStructSize;

        entry->size = size;
        entry->pluginID = pluginID;
        entry->readCB = 0;
        entry->writeCB = 0;
        entry->getSizeCB = 0;
        entry->alwaysCB = 0;
        entry->rightsCB = 0;
        entry->constructCB = constructCB ? constructCB : (RwPluginObjectConstructor)FUN_0053eac0;
        entry->destructCB = destructCB ? destructCB : (RwPluginObjectDestructor)FUN_0053eac0;
        entry->copyCB = copyCB ? copyCB : (RwPluginObjectCopy)FUN_0053eac0;
        entry->errStrCB = 0;
        entry->nextRegEntry = 0;
        entry->prevRegEntry = 0;

        /* Point back to the parent */
        entry->parentRegistry = reg;

        /* Append it to the list */
        if (reg->firstRegEntry == NULL)
        {
            /* First in the list, just shove it in */
            reg->firstRegEntry = entry;
            reg->lastRegEntry = entry;
        }
        else
        {
            /* Whack it on the end */
            reg->lastRegEntry->nextRegEntry = entry;
            entry->prevRegEntry = reg->lastRegEntry;

            /* New end of the list */
            reg->lastRegEntry = entry;
        }

        return entry->offset;
    }

    /* Failed to allocate a freelist entry */
    return -1;
}
