/* WorldSectorRender (MFL.exe 004f57a0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * WorldSectorRenderAtomics written as a macro (MFL.exe inlines it), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN
 * -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define rwObjectTestFlags(o,f) ((((const RwObject *)(o))->flags) & (RwUInt8)(f))
#define THREADSAFE_CHECK_ISCALLEDMAIN() 
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
enum RwFrustumTestResult
{
    rwSPHEREOUTSIDE = 0,
    rwSPHEREBOUNDARY = 1,
    rwSPHEREINSIDE = 2,
    rwFRUSTUMTESTRESULTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFrustumTestResult RwFrustumTestResult;
extern RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera *
                                                     camera,
                                                     const RwSphere *
                                                     sphere);
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
extern RpWorldSector * RpWorldSectorRender(RpWorldSector *worldSector);
enum RpAtomicFlag
{
    rpATOMICCOLLISIONTEST = 0x01,

    rpATOMICRENDER = 0x04,

    rpATOMICFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicFlag RpAtomicFlag;
typedef struct RpTie RpTie;
struct RpTie
{

    RwLLLink lAtomicInWorldSector;
    RpAtomic *apAtom;

    RwLLLink lWorldSectorInAtomic;
    RpWorldSector *worldSector;
};
#define RpAtomicRenderMacro(_atomic) ((_atomic)->renderCallBack(_atomic))
#define RpAtomicRender(_atomic) RpAtomicRenderMacro(_atomic)
extern const RwSphere *
RpAtomicGetWorldBoundingSphere(RpAtomic * atomic);
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
/* ---- end of the generated declarations ---- */

/* WorldSectorRenderAtomics is inlined in MFL.exe: a macro here (diffemu cannot relocate a call to a static helper) */
#define WorldSectorRenderAtomics(worldSector) WORLDSECTORRENDERATOMICS_BODY(worldSector)
#define WORLDSECTORRENDERATOMICS_BODY(worldSector)                                         \
{                                                                                          \
    RwLLLink           *cur, *end;                                                         \
    RpTie              *tie;                                                               \
    RpAtomic           *atomic;                                                            \
                                                                                           \
    cur = rwLinkListGetFirstLLLink(&(worldSector)->collAtomicsInWorldSector);              \
    end = rwLinkListGetTerminator(&(worldSector)->collAtomicsInWorldSector);               \
                                                                                           \
    while (cur != end)                                                                     \
    {                                                                                      \
        tie = rwLLLinkGetData(cur, RpTie, lAtomicInWorldSector);                           \
        atomic = tie->apAtom;                                                              \
                                                                                           \
        if (rwObjectTestFlags(atomic, rpATOMICRENDER))                                     \
        {                                                                                  \
            if (atomic->renderFrame != RWSRCGLOBAL(renderFrame))                           \
            {                                                                              \
                const RwSphere     *atomicBoundingSphere;                                  \
                                                                                           \
                atomicBoundingSphere = RpAtomicGetWorldBoundingSphere(atomic);             \
                                                                                           \
                if (RwCameraFrustumTestSphere((RwCamera *) RWSRCGLOBAL(curCamera),         \
                                              atomicBoundingSphere) != rwSPHEREOUTSIDE)    \
                {                                                                          \
                    RpAtomicRender(atomic);                                                \
                }                                                                          \
                                                                                           \
                atomic->renderFrame = RWSRCGLOBAL(renderFrame);                            \
            }                                                                              \
        }                                                                                  \
                                                                                           \
        cur = rwLLLinkGetNext(cur);                                                        \
    }                                                                                      \
}
#if 0      /* the codfh static helper, kept as the macro above */
static void WorldSectorRenderAtomics_codfh(RpWorldSector * worldSector)
{
    RwLLLink           *cur, *end;
    RpTie              *tie;
    RpAtomic           *atomic;

	THREADSAFE_CHECK_ISCALLEDMAIN();

    cur = rwLinkListGetFirstLLLink(&worldSector->collAtomicsInWorldSector);
    end = rwLinkListGetTerminator(&worldSector->collAtomicsInWorldSector);

    while (cur != end)
    {
        
        tie = rwLLLinkGetData(cur, RpTie, lAtomicInWorldSector);
        atomic = tie->apAtom;

        if (rwObjectTestFlags(atomic, rpATOMICRENDER))
        {
            
            if (atomic->renderFrame != RWSRCGLOBAL(renderFrame))
            {
                const RwSphere     *atomicBoundingSphere;

                atomicBoundingSphere = RpAtomicGetWorldBoundingSphere(atomic);

                if (RwCameraFrustumTestSphere((RwCamera *) RWSRCGLOBAL(curCamera),
                                              atomicBoundingSphere) != rwSPHEREOUTSIDE)
                {
                    RpAtomicRender(atomic);
                }

                atomic->renderFrame = RWSRCGLOBAL(renderFrame);
            }
        }

        cur = rwLLLinkGetNext(cur);
    }

    return;
}
#endif
/* TIER2: unvalidated 2026-11-14 authority=static */
RpWorldSector * WorldSectorRender(RpWorldSector * sector, void * pData __RWUNUSED__)
{

    if (RpWorldSectorRender(sector))
    {
        WorldSectorRenderAtomics(sector);
    }

    return (sector);
}
