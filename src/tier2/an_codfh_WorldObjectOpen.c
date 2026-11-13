/* WorldObjectOpen (MFL.exe 004f37c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RpLight RpLight;
typedef struct RpLightTie RpLightTie;
struct RpLightTie
{

    RwLLLink lightInWorldSector;
    RpLight *light;

    RwLLLink WorldSectorInLight;
    RpWorldSector *sect;
};
typedef struct RpTie RpTie;
struct RpTie
{

    RwLLLink lAtomicInWorldSector;
    RpAtomic *apAtom;

    RwLLLink lWorldSectorInAtomic;
    RpWorldSector *worldSector;
};
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
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpTie) == 0x18);      /* MFL.exe 0x4f37de (WorldObjectOpen): push 0x18, entry size of the tie free list */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RpLightTie) == 0x18);      /* MFL.exe 0x4f3829 (WorldObjectOpen): push 0x18, entry size of the light tie free list */
/* ---- end of the generated declarations ---- */

typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpWorldObjGlobals, lightTieFreeList) == 4);      /* MFL.exe 0x4f383f: mov [ecx+edx+0x4],eax (second free list) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(rpWorldObjGlobals, clumpsInFrustumID) == 8);      /* MFL.exe 0x4f3889: mov [ecx+eax+0x8],0 */
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwModuleInfo worldObjModule;
extern RwInt32 _rpTieFreeListBlockSize;
extern RwInt32 _rpTieFreeListPreallocBlocks;
extern RwFreeList _rpTieFreeList;
extern RwInt32 _rpLightTieFreeListBlockSize;
extern RwInt32 _rpLightTieFreeListPreallocBlocks;
extern RwFreeList _rpLightTieFreeList;
/* TIER2: unvalidated authority=diffemu */
void * WorldObjectOpen(void *instance, RwInt32 offset, RwInt32 size __RWUNUSED__)
{

    worldObjModule.globalsOffset = offset;

    RPWORLDOBJGLOBAL(tieFreeList) =
        RwFreeListCreateAndPreallocateSpace(sizeof(RpTie), _rpTieFreeListBlockSize,
        sizeof(RwUInt32),
        _rpTieFreeListPreallocBlocks, &_rpTieFreeList,
        rwMEMHINTDUR_GLOBAL | rwID_WORLDMODULE);
    if (!RPWORLDOBJGLOBAL(tieFreeList))
    {
        
        return (NULL);
    }

    RPWORLDOBJGLOBAL(lightTieFreeList) =
        RwFreeListCreateAndPreallocateSpace(sizeof(RpLightTie), _rpLightTieFreeListBlockSize,
        sizeof(RwUInt32),
        _rpLightTieFreeListPreallocBlocks, &_rpLightTieFreeList,
        rwMEMHINTDUR_GLOBAL | rwID_WORLDMODULE);
    if (!RPWORLDOBJGLOBAL(lightTieFreeList))
    {
        
        RwFreeListDestroy(RPWORLDOBJGLOBAL(tieFreeList));
        RPWORLDOBJGLOBAL(tieFreeList) = (RwFreeList *)NULL;
        return (NULL);
    }

    RWSRCGLOBAL(renderFrame) = 1;

    RPWORLDOBJGLOBAL(clumpsInFrustumID) = 0;

    worldObjModule.numInstances++;

    return (instance);
}
