/* DestroyClumpAtomic (MFL.exe 004f7e00). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpAtomicDestroy with RpAtomicSetGeometry(atomic, NULL, 0) inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
extern const
RwPluginRegistry *_rwPluginRegistryDeInitObject(const RwPluginRegistry * reg,
                                               void *object);
#define rwPluginRegistryDeInitObject(reg,object) _rwPluginRegistryDeInitObject(reg, object)
extern RwBool RwResourcesFreeResEntry(RwResEntry * entry);
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
extern void _rwObjectHasFrameReleaseFrame(void *object);
#define rwObjectHasFrameReleaseFrame(object) _rwObjectHasFrameReleaseFrame(object)
extern RwBool
RpGeometryDestroy(RpGeometry *geometry);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
/* ---- end of the generated declarations ---- */

typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
#define RWCLUMPGLOBAL(var) (RWPLUGINOFFSET(rpClumpGlobals, RwEngineInstance, clumpModule.globalsOffset)->var)
extern RwPluginRegistry atomicTKList;
extern RwModuleInfo clumpModule;
extern RpWorld *RpAtomicGetWorld(const RpAtomic *atomic);
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpClumpGlobals, clumpFreeList) == 4);      /* MFL.exe 0x4f7e6a atomic free list [globals+0]; 0x4f70a1 (RpClumpDestroy) clump free list [globals+4] */

/* TIER2: unvalidated authority=diffemu */
RpAtomic * DestroyClumpAtomic(RpAtomic * atomic, void * pData __RWUNUSED__)
{
    /* RpAtomicDestroy(atomic) with RpAtomicSetGeometry(atomic, NULL, 0), inlined in MFL.exe */
    rwPluginRegistryDeInitObject(&atomicTKList, atomic);

    if (atomic->repEntry)
    {
        RwResourcesFreeResEntry(atomic->repEntry);
    }

    if (atomic->geometry)
    {
        RwFrame *frame;

        RpGeometryDestroy(atomic->geometry);
        frame = RpAtomicGetFrame(atomic);
        atomic->geometry = (RpGeometry *)NULL;
        if (frame && RpAtomicGetWorld(atomic))
        {
            RwFrameUpdateObjects(frame);
        }
    }

    rwObjectHasFrameReleaseFrame(atomic);

    RwFreeListFree(RWCLUMPGLOBAL(atomicFreeList), atomic);

    return (atomic);
}
