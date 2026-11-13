/* PVSWorldReadChunk (MFL.exe 00548fd0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvs.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helpers
 * PVSInitializeSectors / PVSFindDiagonalSizeOfBBox inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwReal RwV3dLength(const RwV3d * in);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
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
typedef RpWorldSector *(*RpWorldSectorCallBack)(RpWorldSector *worldSector, void *data);
#define RpWorldGetBBoxMacro(_world) (&((_world)->boundingBox))
#define RpWorldGetBBox(_world) RpWorldGetBBoxMacro(_world)
extern RpWorld *RpWorldForAllWorldSectors(RpWorld * world,
                                              RpWorldSectorCallBack
                                              fpCallBack, void *pData);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
/* ---- end of the generated declarations ---- */

#define PVSCACHEFROMWORLD(world) ((RpPVSCache *)(((char *)(world))+rpPVSGlobals.worldOffset))
typedef             RwBool(*RpPVSProgressCallBack) (RwInt32 msg,
                                                     RwReal value);
typedef struct _rpPVSPolyList *_rpPVSPolyListPtr;
typedef struct RpPVS RpPVS;      /* only the pointer rpPVSGlobals.CurrPVS is declared here */
typedef struct RpPVSCache RpPVSCache;
struct RpPVSCache
{
    RwBool              processed;
    RwBool              formatted;
    RwInt32             NumWorldSectors;

    RwInt32             ptotal;
    RwInt32             paccept;

    RwBool              hooked;

    RwUInt32            nextID;

    RwInt32             viscount;

    RpPVSProgressCallBack progressCallBack;

    _rpPVSPolyListPtr         polygons;

    RpWorldSectorCallBackRender     renderCallBack;
};
typedef struct RpPVSGlobalVars RpPVSGlobalVars;
struct RpPVSGlobalVars
{
    RpWorld            *World;

    RwInt32             worldOffset;
    RwInt32             sectorOffset;

    RwBool              collis;
    RwBool              bfc;

    RwInt32             progress_count;
    RwReal              diagonal;

    RwReal              gran;

    RwInt32             InSector;
    RwV3d               ViewPos;
    RpPVS             *CurrPVS;
};
extern RpPVSGlobalVars    rpPVSGlobals;
extern RwInt32             _rpPVSWorldGetNumWorldSectors(RpWorld * world);
extern RpPVSGlobalVars     rpPVSGlobals;
extern RpSector * PVSCalculateSectorBounds(RpSector * spSector, const RwBBox * box);
extern RpWorldSector * PVSCopyAndTagSectors(RpWorldSector * sector, void * data __RWUNUSED__);
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpPVSCache) == 0x2c);      /* an_codfh_rppvs.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpPVSGlobalVars, InSector) == 0x20);      /* an_codfh_rppvs.c */
/* TIER2: unvalidated authority=diffemu */
RwStream * PVSWorldReadChunk(RwStream * s, RwInt32 len __RWUNUSED__, RpWorld * wpWorld, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpPVSCache         *pvsCache = PVSCACHEFROMWORLD(wpWorld);

    RwStreamReadInt32(s, &pvsCache->processed,
                      sizeof(pvsCache->processed));
    RwStreamReadInt32(s, &pvsCache->hooked, sizeof(pvsCache->hooked));
    pvsCache->hooked = FALSE;

    rpPVSGlobals.World = wpWorld;
    rpPVSGlobals.bfc = TRUE;

    /* PVSInitializeSectors and PVSFindDiagonalSizeOfBBox, inlined as in MFL.exe */
    {
        RpPVSCache         *PVSCache =
            PVSCACHEFROMWORLD(rpPVSGlobals.World);
        RwBBox             *boundingBox = &rpPVSGlobals.World->boundingBox;
        RwV3d               diag;

        PVSCache->NumWorldSectors =
            _rpPVSWorldGetNumWorldSectors(rpPVSGlobals.World);

        diag.x = (boundingBox->sup.x - boundingBox->inf.x);
        diag.y = (boundingBox->sup.y - boundingBox->inf.y);
        diag.z = (boundingBox->sup.z - boundingBox->inf.z);
        rpPVSGlobals.diagonal = RwV3dLength(&diag);

        PVSCalculateSectorBounds((rpPVSGlobals.World)->rootSector,
                                        RpWorldGetBBox(rpPVSGlobals.World));

        RpWorldForAllWorldSectors(rpPVSGlobals.World,
                                  PVSCopyAndTagSectors, NULL);
    }

    return (s);
}
