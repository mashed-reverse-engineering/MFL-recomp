/* WorldAtomicSync (MFL.exe 004f3d20). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * AtomicDestroyTies with _rpTieDestroy and WorldAttachAtomicSphere inlined by hand (MFL.exe has them inlined), release build macros
 * expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListAddLLLink(list,linkvar) ( (linkvar)->next = (list)->link.next, (linkvar)->prev = (&(list)->link), ((list)->link.next)->prev = (linkvar), (list)->link.next = (linkvar) )
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define GETCOORD(vect,y) (*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define RwV3dAssignMacro(_target,_source) ( *(_target) = *(_source) )
#define RwV3dAssign(o,a) RwV3dAssignMacro(o, a)
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
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
struct RpSector
{
    RwInt32 type;
};
typedef struct RpPlaneSector RpPlaneSector;
struct RpPlaneSector
{
    RwInt32 type;

    RwReal value;
    RpSector *leftSubTree;
    RpSector *rightSubTree;
    RwReal leftValue;
    RwReal rightValue;
};
typedef struct RpTie RpTie;
struct RpTie
{

    RwLLLink lAtomicInWorldSector;
    RpAtomic *apAtom;

    RwLLLink lWorldSectorInAtomic;
    RpWorldSector *worldSector;
};
extern const RwSphere *
RpAtomicGetWorldBoundingSphere(RpAtomic * atomic);
#define rpWORLDMAXBSPDEPTH 64
enum RwCriterionWorldID
{

    rwID_NAWORLDID = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x00),

    rwID_MATERIALMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x01),

    rwID_MESHMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x02),

    rwID_GEOMETRYMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x03),

    rwID_CLUMPMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x04),

    rwID_LIGHTMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x05),

    rwID_COLLISIONMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x06),

    rwID_WORLDMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x07),

    rwID_RANDOMMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x08),

    rwID_WORLDOBJMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x09),

    rwID_SECTORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0A),

    rwID_BINWORLDMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0B),

    rwID_WORLDPIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0D),

    rwID_BINMESHPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0E),

    rwID_RXWORLDDEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0F),

    rwID_NATIVEDATAPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10),

    rwID_VERTEXFMTPLUGIN = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x11),
    rwCRITERIONWORLDIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionWorldID RwCriterionWorldID;
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, sizeof(RpPlaneSector) == 0x18);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpPlaneSector, leftSubTree) == 8);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpPlaneSector, leftValue) == 0x10);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwResEntry, size) == 0x8);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwResEntry, owner) == 0xc);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_11, sizeof(RwResEntry) == 0x18);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RpPlaneSector, rightSubTree) == 0xc);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_19, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_20, sizeof(RpTie) == 0x18);      /* MFL.exe 0x4f37de (WorldObjectOpen): push 0x18, entry size of the tie free list */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RpSector, type) == 0);      /* MFL.exe 0x4f410d / 0x4f4111 (WorldLightSync): mov ecx,[esi]; test ecx,ecx; jge = sect->type < 0 (world sector); the plane sector's type at the same offset 0x4f417f */
/* ---- end of the generated declarations ---- */

typedef struct RpWorldAtomicExt RpWorldAtomicExt;
struct RpWorldAtomicExt
{
    
    RpWorld            *world;

    RwObjectHasFrameSyncFunction oldSync;
};
typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
#define ATOMICEXTFROMATOMIC(atomic) ((RpWorldAtomicExt *)(((RwUInt8 *)(atomic)) + atomicExtOffset))
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwInt32      atomicExtOffset;
extern RwModuleInfo worldObjModule;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldAtomicExt, oldSync) == 4 && RW_OFFSETOF(rpWorldObjGlobals, tieFreeList) == 0);      /* MFL.exe 0x4f3d38 call [esi+0x4] (atomicExt->oldSync), 0x4f3d46 atomicExt->world [esi]; 0x4f3d9f / 0x4f3e46 tie free list [globals+0] */

/* TIER2: unvalidated authority=diffemu */
RwObjectHasFrame * WorldAtomicSync(RwObjectHasFrame * type)
{
    RpAtomic           *atomic = (RpAtomic *) type;
    RpWorldAtomicExt   *atomicExt = ATOMICEXTFROMATOMIC(type);

    if (atomicExt->oldSync(type))
    {
        RpWorld            *world = atomicExt->world;

        if (world)
        {
            /* AtomicDestroyTies(atomic) with _rpTieDestroy, inlined in MFL.exe (0x4f3d54..0x4f3dae): a tie that still links an atomic
             * and a sector is unlinked from both and given back to the tie free list */
            {
                RwLLLink           *cur, *end;
                RpTie              *tie;

                end = rwLinkListGetTerminator(&atomic->llWorldSectorsInAtomic);
                cur = rwLinkListGetFirstLLLink(&atomic->llWorldSectorsInAtomic);
                while (cur != end)
                {
                    tie = rwLLLinkGetData(cur, RpTie, lWorldSectorInAtomic);
                    cur = rwLLLinkGetNext(cur);
                    if (tie->apAtom && tie->worldSector)
                    {
                        rwLinkListRemoveLLLink(&tie->lWorldSectorInAtomic);
                        rwLinkListRemoveLLLink(&tie->lAtomicInWorldSector);
                        RwFreeListFree(RPWORLDOBJGLOBAL(tieFreeList), tie);
                    }
                }
            }

            /* WorldAttachAtomicSphere(world, atomic), inlined in MFL.exe (0x4f3db0..0x4f3ed3): a tie for every world sector the
             * atomic's world bounding sphere box touches (BSP walk with an explicit stack) */
            {
                RpSector           *spSect;
                RpSector           *spaStack[rpWORLDMAXBSPDEPTH];
                RwInt32             nStack = 0;
                RwV3d               inf, sup;
                const RwSphere     *worldSphere;

                worldSphere = RpAtomicGetWorldBoundingSphere(atomic);

                RwV3dAssign(&inf, &worldSphere->center);
                RwV3dAssign(&sup, &inf);

                inf.x -= worldSphere->radius;
                inf.y -= worldSphere->radius;
                inf.z -= worldSphere->radius;

                sup.x += worldSphere->radius;
                sup.y += worldSphere->radius;
                sup.z += worldSphere->radius;

                spSect = world->rootSector;

                do
                {
                    if (spSect->type < 0)
                    {
                        RpTie              *tie;

                        tie = (RpTie *)RwFreeListAlloc(RPWORLDOBJGLOBAL(tieFreeList),
                                             rwMEMHINTDUR_EVENT | rwID_WORLDMODULE);

                        tie->worldSector = (RpWorldSector *) spSect;
                        tie->apAtom = atomic;

                        rwLinkListAddLLLink(&((RpWorldSector *)spSect)->collAtomicsInWorldSector,
                                            &tie->lAtomicInWorldSector);

                        rwLinkListAddLLLink(&atomic->llWorldSectorsInAtomic,
                                            &tie->lWorldSectorInAtomic);

                        spSect = spaStack[nStack--];
                    }
                    else
                    {
                        RpPlaneSector      *pspPlane = (RpPlaneSector *) spSect;

                        if (GETCOORD(inf, spSect->type) < pspPlane->leftValue)
                        {
                            spSect = pspPlane->leftSubTree;

                            if (pspPlane->rightValue <
                                GETCOORD(sup, pspPlane->type))
                            {
                                spaStack[++nStack] = pspPlane->rightSubTree;
                            }
                        }
                        else
                        {
                            if (pspPlane->rightValue <
                                GETCOORD(sup, pspPlane->type))
                            {
                                spSect = pspPlane->rightSubTree;
                            }
                            else
                            {
                                spSect = spaStack[nStack--];
                            }
                        }
                    }
                }
                while (nStack >= 0);
            }
        }

        return (type);
    }
    return ((RwObjectHasFrame *)NULL);
}
