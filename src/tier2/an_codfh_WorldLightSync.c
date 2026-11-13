/* WorldLightSync (MFL.exe 004f3fe0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * LightDestroyTies with _rpLightTieDestroy inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListAddLLLink(list,linkvar) ( (linkvar)->next = (list)->link.next, (linkvar)->prev = (&(list)->link), ((list)->link.next)->prev = (linkvar), (list)->link.next = (linkvar) )
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define GETCOORD(vect,y) (*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define rwObjectGetSubType(o) (((const RwObject *)(o))->subType)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
struct RwFrame
{
    RwObject object;

    RwLLLink inDirtyListLink;

    RwMatrix modelling;
    RwMatrix ltm;

    RwLinkList objectList;

    struct RwFrame *child;
    struct RwFrame *next;
    struct RwFrame *root;


};
extern RwMatrix *
RwFrameGetLTM(RwFrame * frame);
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
#define rpLIGHTPOSITIONINGSTART 0x80
enum RpLightType
{
    rpNALIGHTTYPE = 0,

    rpLIGHTDIRECTIONAL,
    rpLIGHTAMBIENT,

    rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART,
    rpLIGHTSPOT,
    rpLIGHTSPOTSOFT,
    rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightType RpLightType;
typedef struct RpLight RpLight;
struct RpLight
{
        RwObjectHasFrame object;
        RwReal radius;
        RwRGBAReal color;
        RwReal minusCosAngle;
        RwLinkList WorldSectorsInLight;
        RwLLLink inWorld;
        RwUInt16 lightFrame;
        RwUInt16 pad;
};
typedef struct RpLightTie RpLightTie;
struct RpLightTie
{

    RwLLLink lightInWorldSector;
    RpLight *light;

    RwLLLink WorldSectorInLight;
    RpWorldSector *sect;
};
#define RpLightGetFrameMacro(_light) ((RwFrame *)rwObjectGetParent((_light)))
#define RpLightGetTypeMacro(_light) ((RpLightType)rwObjectGetSubType((_light)))
#define RpLightGetFrame(_light) RpLightGetFrameMacro(_light)
#define RpLightGetType(_light) RpLightGetTypeMacro(_light)
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
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpLight, object.object.flags) == 0x2);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpLight, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_8, sizeof(RpPlaneSector) == 0x18);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpPlaneSector, leftSubTree) == 8);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpPlaneSector, leftValue) == 0x10);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpLight, radius) == 0x14);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpLight, color) == 0x18);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpLight, minusCosAngle) == 0x28);      /* an_codfh_RpLightStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RwResEntry, size) == 0x8);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RwResEntry, owner) == 0xc);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_18, sizeof(RwResEntry) == 0x18);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_19, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_20, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_22, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_23, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_24, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_25, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_26, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_27, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_28, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_29, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_30, RW_OFFSETOF(RpPlaneSector, rightSubTree) == 0xc);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_31, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_32, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
RW_LAYOUT_ASSERT(rw37_row_33, sizeof(RpLight) == 0x40);      /* exe: exe static lightTKList.sizeOfStruct at 0x0062ebcc */
RW_LAYOUT_ASSERT(rw37_row_34, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_35, sizeof(RpLightTie) == 0x18);      /* MFL.exe 0x4f3829 (WorldObjectOpen): push 0x18, entry size of the light tie free list */
RW_LAYOUT_ASSERT(rw37_row_36, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
RW_LAYOUT_ASSERT(rw37_row_37, RW_OFFSETOF(RpSector, type) == 0);      /* MFL.exe 0x4f410d / 0x4f4111 (WorldLightSync): mov ecx,[esi]; test ecx,ecx; jge = sect->type < 0 (world sector); the plane sector's type at the same offset 0x4f417f */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID 0x02
typedef struct RpWorldLightExt RpWorldLightExt;
struct RpWorldLightExt
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
#define LIGHTEXTFROMLIGHT(light) ((RpWorldLightExt *)(((RwUInt8 *)(light)) + lightExtOffset))
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwInt32      lightExtOffset;
extern RwModuleInfo worldObjModule;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldLightExt, oldSync) == 4 && RW_OFFSETOF(rpWorldObjGlobals, lightTieFreeList) == 4);      /* MFL.exe 0x4f3ff8 call [esi+0x4] (lightExt->oldSync), 0x4f3ff5 / 0x4f4010 lightExt->world [esi]; 0x4f4072 / 0x4f4123 light tie free list [globals+4] */

/* LightDestroyTies(light) with _rpLightTieDestroy (both inlined in MFL.exe, 0x4f402d..0x4f4082): unlink every tie of the light from
 * its sector and from the light, give it back to the light tie free list */
#define LIGHTDESTROYTIES(_light)                                        \
MACRO_START                                                             \
{                                                                       \
    RwLLLink           *_cur, *_end;                                    \
    RpLightTie         *_tie;                                           \
                                                                        \
    _cur = rwLinkListGetFirstLLLink(&(_light)->WorldSectorsInLight);    \
    _end = rwLinkListGetTerminator(&(_light)->WorldSectorsInLight);     \
    while (_cur != _end)                                                \
    {                                                                   \
        _tie = rwLLLinkGetData(_cur, RpLightTie, WorldSectorInLight);   \
        _cur = rwLLLinkGetNext(_cur);                                   \
        rwLinkListRemoveLLLink(&_tie->WorldSectorInLight);              \
        rwLinkListRemoveLLLink(&_tie->lightInWorldSector);              \
        RwFreeListFree(RPWORLDOBJGLOBAL(lightTieFreeList), _tie);       \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
RwObjectHasFrame * WorldLightSync(RwObjectHasFrame * object)
{
    RpLight            *light = (RpLight *) object;
    RpWorldLightExt    *lightExt = LIGHTEXTFROMLIGHT(light);

    if (lightExt->oldSync(object))
    {
        RpWorld            *world;
        RwFrame            *lightFrame;
        RpLightType         type;

        type = RpLightGetType(light);
        if ((RwInt32) type < (RwInt32) rpLIGHTPOSITIONINGSTART)
        {
            return (object);
        }

        world = lightExt->world;
        lightFrame = RpLightGetFrame(light);
        if (world && lightFrame)
        {
            RpSector           *sect;
            RpSector           *spaStack[rpWORLDMAXBSPDEPTH];
            RwInt32             nStack = 0;
            RwV3d               inf, sup;
            RwReal              radius = light->radius;

            LIGHTDESTROYTIES(light);

            inf = RwFrameGetLTM(lightFrame)->pos;
            sup = inf;

            sup.x += radius;
            sup.y += radius;
            sup.z += radius;

            inf.x -= radius;
            inf.y -= radius;
            inf.z -= radius;

            sect = world->rootSector;

            do
            {
                if (sect->type < 0)
                {
                    RpLightTie         *tie;

                    tie = (RpLightTie *)RwFreeListAlloc(RPWORLDOBJGLOBAL(lightTieFreeList),
                                                   rwMEMHINTDUR_EVENT | rwID_WORLDMODULE);

                    tie->sect = (RpWorldSector *) sect;
                    tie->light = light;

                    rwLinkListAddLLLink(&((RpWorldSector *)sect)->lightsInWorldSector,
                                        &tie->lightInWorldSector);
                    rwLinkListAddLLLink(&light->WorldSectorsInLight,
                                        &tie->WorldSectorInLight);

                    sect = spaStack[nStack--];
                }
                else
                {
                    RpPlaneSector      *pspPlane = (RpPlaneSector *) sect;

                    if (GETCOORD(inf, sect->type) < pspPlane->leftValue)
                    {
                        sect = pspPlane->leftSubTree;

                        if (pspPlane->rightValue < GETCOORD(sup, pspPlane->type))
                        {
                            spaStack[++nStack] = pspPlane->rightSubTree;
                        }
                    }
                    else
                    {
                        if (pspPlane->rightValue < GETCOORD(sup, pspPlane->type))
                        {
                            sect = pspPlane->rightSubTree;
                        }
                        else
                        {
                            sect = spaStack[nStack--];
                        }
                    }
                }
            }
            while (nStack >= 0);
        }
    }
    else
    {
        RWERROR((E_RW_NULLP));
    }

    return (object);
}
