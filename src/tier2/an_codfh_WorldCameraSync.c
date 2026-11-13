/* WorldCameraSync (MFL.exe 004f39c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * WorldSyncCamera with SectorsInFrustumAddSpace inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by
 * rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by
 * differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef union RwSplitBits RwSplitBits;
union RwSplitBits
{
    RwReal nReal;
    volatile RwInt32 nInt;
    volatile RwUInt32 nUInt;
};
#define GETCOORD(vect,y) (*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define RwRealloc(_p,_s,_h) ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s), (_h)))
enum RwMemoryHintFlag
{

    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;
#define rwSTANDARDHINTRENDERF2B 22
struct RwRaster
{
    RwRaster *parent;
    RwUInt8 *cpPixels;
    RwUInt8 *palette;
    RwInt32 width, height, depth;
    RwInt32 stride;
    RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType;
    RwUInt8 cFlags;
    RwUInt8 privateFlags;
    RwUInt8 cFormat;

    RwUInt8 *originalPixels;
    RwInt32 originalWidth;
    RwInt32 originalHeight;
    RwInt32 originalStride;
};
extern RwMatrix *
RwFrameGetLTM(RwFrame * frame);
#define RwCameraGetFrameMacro(_camera) ((RwFrame *)rwObjectGetParent((_camera)))
#define RwCameraGetFrame(_camera) RwCameraGetFrameMacro(_camera)
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
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, sizeof(RpPlaneSector) == 0x18);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpPlaneSector, leftSubTree) == 8);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpPlaneSector, leftValue) == 0x10);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpPlaneSector, rightSubTree) == 0xc);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_16, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_17, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_18, sizeof(RwSplitBits) == 4);      /* MFL.exe 0x4f3b67 / 0x4f3b6f (WorldCameraSync): fstp dword [esp+0x14] then mov edx,[esp+0x14] (nReal / nInt share one dword) */
RW_LAYOUT_ASSERT(rw37_row_19, RW_OFFSETOF(RpSector, type) == 0);      /* MFL.exe 0x4f410d / 0x4f4111 (WorldLightSync): mov ecx,[esi]; test ecx,ecx; jge = sect->type < 0 (world sector); the plane sector's type at the same offset 0x4f417f */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID 0x02
typedef struct RpWorldCameraExt RpWorldCameraExt;
struct RpWorldCameraExt
{
    
    RpWorldSector     **frustumSectors;
    RwInt32             space;
    RwInt32             position;

    RpWorld            *world;

    RwCameraBeginUpdateFunc oldBeginUpdate;
    RwCameraEndUpdateFunc oldEndUpdate;

    RwObjectHasFrameSyncFunction oldSync;
};
#define rwCAMERADISPLAYSECTORSINCREASE 50
#define WORLDCAMERAEXTFROMCAMERA(camera) ((RpWorldCameraExt *)(((RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldCameraExt, space) == 4 && RW_OFFSETOF(RpWorldCameraExt, position) == 8 && RW_OFFSETOF(RpWorldCameraExt, world) == 0xc && RW_OFFSETOF(RpWorldCameraExt, oldSync) == 0x18);      /* MFL.exe 0x4f39d8 call [esi+0x18] (oldSync), 0x4f39e6 world [esi+0xc], 0x4f3aee space [ebp+4], 0x4f3be2 position [ebp+8], 0x4f3b41 frustumSectors [ebp+0] */

/* TIER2: unvalidated authority=diffemu */
RwObjectHasFrame * WorldCameraSync(RwObjectHasFrame * object)
{
    RpWorldCameraExt   *cameraExt = WORLDCAMERAEXTFROMCAMERA(object);

    if (cameraExt->oldSync(object))
    {
        RwCamera           *camera = (RwCamera *) object;
        RpWorld            *world = cameraExt->world;

        if (world)
        {
            RwStandardFunc const HintRenderF2BFunc =
                RWSRCGLOBAL(stdFunc[rwSTANDARDHINTRENDERF2B]);

            /* WorldSyncCamera(world, camera) with SectorsInFrustumAddSpace, static in baworobj.c, inlined in MFL.exe
             * (0x4f3a08..0x4f3be2): the world sectors inside the camera frustum, in render order, into cameraExt->frustumSectors */
            {
                RpSector           *spaStack[rpWORLDMAXBSPDEPTH];
                RpSector           *spSect;
                RwV3d               vViewPoint;
                RwV3d               inf, sup;
                const RwFrustumPlane *plpPlanes;
                RwBool              goBackToFront;
                RwInt32             nStack = 0;
                RwInt32             position = 0;

                vViewPoint = RwFrameGetLTM(RwCameraGetFrame(camera))->pos;
                inf = camera->frustumBoundBox.inf;
                sup = camera->frustumBoundBox.sup;
                plpPlanes = camera->frustumPlanes;
                goBackToFront = (world->renderOrder == rpWORLDRENDERBACK2FRONT);

                spSect = cameraExt->world->rootSector;
                do
                {
                    const RwBool isAtomic = (spSect->type < 0);

                    if (isAtomic)
                    {
                        RwBool              outside;
                        RwInt32             i;
                        RpWorldSector     * worldSector = (RpWorldSector *) spSect;
                        const RwV3d * const base = (RwV3d *)&worldSector->boundingBox;

                        for (i = 0; i < 6; ++i)
                        {
                            RwV3d               vCorner;
                            RwSplitBits         sbSide;

                            vCorner.x = base[plpPlanes[i].closestX].x;
                            vCorner.y = base[plpPlanes[i].closestY].y;
                            vCorner.z = base[plpPlanes[i].closestZ].z;

                            sbSide.nReal =
                                RwV3dDotProduct(&vCorner, &plpPlanes[i].plane.normal) -
                                plpPlanes[i].plane.distance;

                            outside = (sbSide.nInt > 0);

                            if (outside)
                            {
                                break;
                            }
                        }

                        if (!outside)
                        {
                            if (position >= cameraExt->space)
                            {
                                /* SectorsInFrustumAddSpace(cameraExt, rwCAMERADISPLAYSECTORSINCREASE) */
                                RpWorldSector     **newFrustumSectors;
                                RwInt32             memSize;

                                memSize = (cameraExt->space + rwCAMERADISPLAYSECTORSINCREASE) * sizeof(RpWorldSector *);
                                if (cameraExt->frustumSectors)
                                {
                                    newFrustumSectors = (RpWorldSector **)
                                        RwRealloc(cameraExt->frustumSectors, memSize,
                                        rwMEMHINTDUR_EVENT | rwID_WORLDMODULE |
                                        rwMEMHINTFLAG_RESIZABLE);
                                }
                                else
                                {
                                    newFrustumSectors = (RpWorldSector **) RwMalloc(memSize,
                                        rwMEMHINTDUR_EVENT | rwID_WORLDMODULE |
                                        rwMEMHINTFLAG_RESIZABLE);
                                }

                                if (!newFrustumSectors)
                                {
                                    RWERROR((E_RW_NOMEM, memSize));
                                    break;      /* WorldSyncCamera stops here (position stored below) */
                                }
                                cameraExt->frustumSectors = newFrustumSectors;
                                cameraExt->space += rwCAMERADISPLAYSECTORSINCREASE;
                            }

                            cameraExt->frustumSectors[position++] = worldSector;
                        }

                        spSect = spaStack[nStack--];
                    }
                    else
                    {
                        RpPlaneSector      *pspPlane = (RpPlaneSector *) spSect;
                        RwSplitBits         sbLeft;
                        RwSplitBits         sbRight;

                        sbLeft.nReal = GETCOORD(inf, spSect->type) - pspPlane->leftValue;
                        sbRight.nReal = pspPlane->rightValue - GETCOORD(sup, spSect->type);

                        if ((sbLeft.nInt < 0) && (sbRight.nInt < 0))
                        {
                            const RwBool  viewPointIsHigher =
                                GETCOORD(vViewPoint, pspPlane->type) > pspPlane->value;

                            if (  (goBackToFront && viewPointIsHigher) ||
                                 !(goBackToFront || viewPointIsHigher) )
                            {
                                spSect = pspPlane->leftSubTree;
                                spaStack[++nStack] = pspPlane->rightSubTree;
                            }
                            else
                            {
                                spSect = pspPlane->rightSubTree;
                                spaStack[++nStack] = pspPlane->leftSubTree;
                            }
                        }
                        else
                        {
                            spSect = (sbLeft.nInt < 0)?
                                (pspPlane->leftSubTree) : (pspPlane->rightSubTree);
                        }
                    }
                }
                while (nStack >= 0);

                cameraExt->position = position;
            }

            HintRenderF2BFunc(NULL, NULL,
                              (world->renderOrder ==
                               rpWORLDRENDERFRONT2BACK));
        }

        return (object);
    }
    return ((RwObjectHasFrame *)NULL);
}
