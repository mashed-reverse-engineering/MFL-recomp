/* WorldRemoveClumpAtomic (MFL.exe 004f4770). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpWorldRemoveAtomic with AtomicDestroyTies / _rpTieDestroy inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
extern RwBool RwResourcesFreeResEntry(RwResEntry * entry);
typedef struct RpVertexNormal RpVertexNormal;
struct RpWorldSector
{
    RwInt32 type;

    RpTriangle *triangles;

    RwV3d *vertices;
    RpVertexNormal *normals;

    RwTexCoords *texCoords[rwMAXTEXTURECOORDS];

    RwRGBA *preLitLum;

    RwResEntry *repEntry;

    RwLinkList collAtomicsInWorldSector;

    RwLinkList lightsInWorldSector;

    RwBBox boundingBox;

    RwBBox tightBoundingBox;

    RpMeshHeader *mesh;

    RxPipeline *pipeline;

    RwUInt16 matListWindowBase;

    RwUInt16 numVertices;
    RwUInt16 numTriangles;
    RwUInt16 pad;
};
typedef struct RpTie RpTie;
struct RpTie
{

    RwLLLink lAtomicInWorldSector;
    RpAtomic *apAtom;

    RwLLLink lWorldSectorInAtomic;
    RpWorldSector *worldSector;
};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwResEntry, size) == 0x8);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwResEntry, owner) == 0xc);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_8, sizeof(RwResEntry) == 0x18);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_15, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_16, sizeof(RpTie) == 0x18);      /* MFL.exe 0x4f37de (WorldObjectOpen): push 0x18, entry size of the tie free list */
/* ---- end of the generated declarations ---- */

typedef struct RpWorldAtomicExt RpWorldAtomicExt;
struct RpWorldAtomicExt
{
    
    RpWorld            *world;

    RwObjectHasFrameSyncFunction oldSync;
};
#define ATOMICEXTFROMATOMIC(atomic) ((RpWorldAtomicExt *)(((RwUInt8 *)(atomic)) + atomicExtOffset))
extern RwInt32      atomicExtOffset;
typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwModuleInfo worldObjModule;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldAtomicExt, oldSync) == 4 && RW_OFFSETOF(rpWorldObjGlobals, lightTieFreeList) == 4);      /* MFL.exe 0x4f3d38 call [esi+0x4] (WorldAtomicSync: atomicExt->oldSync), atomicExt->world [esi]; 0x4f383f (WorldObjectOpen) second free list at globals+4, tie free list at +0 (0x4f47ec) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpTie, apAtom) == 8 && RW_OFFSETOF(RpTie, lWorldSectorInAtomic) == 0xc && RW_OFFSETOF(RpTie, worldSector) == 0x14);      /* MFL.exe 0x4f47ad..0x4f47de: link = tie + 0xc, [tie+8] / [tie+0x14] tested, links at +0 and +0xc unlinked */

/* AtomicDestroyTies(atomic) with _rpTieDestroy (static / private in baworobj.c, inlined in MFL.exe): a tie that still links an atomic
 * and a sector is unlinked from both and given back to the tie free list */
#define ATOMICDESTROYTIES(_atomic)                                      \
MACRO_START                                                             \
{                                                                       \
    RwLLLink           *_cur, *_end;                                    \
    RpTie              *_tie;                                           \
                                                                        \
    _end = rwLinkListGetTerminator(&(_atomic)->llWorldSectorsInAtomic); \
    _cur = rwLinkListGetFirstLLLink(&(_atomic)->llWorldSectorsInAtomic); \
    while (_cur != _end)                                                \
    {                                                                   \
        _tie = rwLLLinkGetData(_cur, RpTie, lWorldSectorInAtomic);      \
        _cur = rwLLLinkGetNext(_cur);                                   \
        if (_tie->apAtom && _tie->worldSector)                          \
        {                                                               \
            rwLinkListRemoveLLLink(&_tie->lWorldSectorInAtomic);        \
            rwLinkListRemoveLLLink(&_tie->lAtomicInWorldSector);        \
            RwFreeListFree(RPWORLDOBJGLOBAL(tieFreeList), _tie);        \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
RpAtomic * WorldRemoveClumpAtomic(RpAtomic * atomic, void *data)
{
    RpWorld * const     world = (RpWorld *)data;
    RpWorldAtomicExt   *atomicExt;

    atomicExt = ATOMICEXTFROMATOMIC(atomic);

    if ( atomicExt && (world == atomicExt->world))
    {
        /* RpWorldRemoveAtomic(world, atomic), inlined in MFL.exe */
        if (atomic->repEntry)
        {
            RwResourcesFreeResEntry(atomic->repEntry);
        }
        ATOMICDESTROYTIES(atomic);

        atomicExt->world = (RpWorld *)NULL;
    }

    return (atomic);
}
