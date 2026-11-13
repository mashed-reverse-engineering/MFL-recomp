/* PVSWorldSectorNodeFn (MFL.exe 005490b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvs.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
#define RxPipelineNodeParamGetData(_param) ( (_param)->dataParam )
typedef struct RxPipelineNode RxPipelineNodeInstance;
typedef struct RxExecutionContext RxExecutionContext;
struct RxPipelineNodeParam
{
    void *dataParam;
    RxHeap *heap;
};
struct RxExecutionContext
{
    RxPipeline *pipeline;
    RxPipelineNode *currentNode;
    RwInt32 exitCode;
    RwUInt32 pad;
    RxPipelineNodeParam params;

};
extern void
_rxPacketDestroy(RxPacket * Packet);
#define RxPacketDispatch(_packet,_output,_self) rxPacketDispatchMacro(_packet, _output, _self)
extern RxPipelineNode *
_rxEmbeddedPacketBetweenNodes(RxPipeline *pipeline,
                             RxPipelineNode *nodeFrom,
                             RwUInt32 whichOutput);
extern RxExecutionContext _rxExecCtxGlobal;
#define rxPacketDispatchMacro(_packet,_output,_self) MACRO_START { RxPipeline *curPipeline = _rxExecCtxGlobal.pipeline; if ( FALSE != _rxExecCtxGlobal.exitCode ) { RxPipelineNode *nextNode = _rxEmbeddedPacketBetweenNodes(curPipeline, _self, (_output)); if ( nextNode != NULL ) { RwUInt32 exitCode = nextNode->nodeDef->nodeMethods.nodeBody( nextNode, &(_rxExecCtxGlobal.params)); if (FALSE == exitCode) _rxExecCtxGlobal.exitCode = exitCode; } } if ( curPipeline->embeddedPacketState > rxPKST_UNUSED ) { curPipeline->embeddedPacketState = rxPKST_INUSE; _rxPacketDestroy(curPipeline->embeddedPacket); } } MACRO_STOP
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
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
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_19, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_20, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RxExecutionContext, exitCode) == 8);      /* MFL.exe 0x549120 / 0x549125 (PVSWorldSectorNodeFn): _rxExecCtxGlobal.pipeline at 0x947120, exitCode at 0x947128 */
RW_LAYOUT_ASSERT(rw37_row_22, RW_OFFSETOF(RxExecutionContext, params) == 0x10);      /* MFL.exe 0x549145 (PVSWorldSectorNodeFn): push 0x947130 = &_rxExecCtxGlobal.params */
RW_LAYOUT_ASSERT(rw37_row_23, RW_OFFSETOF(RxPipelineNodeParam, dataParam) == 0);      /* MFL.exe 0x5490c2 (PVSWorldSectorNodeFn): mov esi,[eax] = params->dataParam (first member; heap behind it is not used by any row) */
RW_LAYOUT_ASSERT(rw37_row_24, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef RwUInt8     RpPVSVisMap;
#define PVSFROMWORLDSECTOR(sector) ((RpPVS *)(((char *)(sector))+rpPVSGlobals.sectorOffset))
#define PVSCACHEFROMWORLD(world) ((RpPVSCache *)(((char *)(world))+rpPVSGlobals.worldOffset))
#define PVSVISMAPGETSECTOR(_vismap,_id) ((_vismap)[(_id) >> 3] & (1 << ((_id) & 7)))
typedef             RwBool(*RpPVSProgressCallBack) (RwInt32 msg,
                                                     RwReal value);
enum _rpPVSPartitionId
{
    rpNAPVSPARTITIONID = 0,
    rpPVSFRONT,
    rpPVSBACK,
    rpPVSSPLIT,
    rpPVSCOPLANAR,
    rpPVSPARTITIONIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum _rpPVSPartitionId _rpPVSPartitionId;
typedef struct _rpPVSPolyList _rpPVSPolyList;
typedef struct _rpPVSPolyList *_rpPVSPolyListPtr;
typedef struct RpPVS RpPVS;
struct RpPVS
{
    RwInt32             sectorID;
    RwInt32             vismaplength;
    RwInt32             sampleKey;

    RpPVSVisMap        *vismap;

    _rpPVSPolyListPtr         sectailpoly; 
    _rpPVSPartitionId         potential;

    RwUInt32            numpols;
    RwBBox              sbox;
    RwBBox              gbox;
    RwReal              diagonal;
    RwV3d               centre;
    RwInt32             axessig[3];
};
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
extern RpPVSGlobalVars     rpPVSGlobals;
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpPVSCache) == 0x2c);      /* an_codfh_rppvs.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpPVSGlobalVars, InSector) == 0x20);      /* an_codfh_rppvs.c */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpPVS, vismap) == 0xc);      /* MFL.exe 0x5490d9: mov edx,[eax+esi+0xc] (pvs->vismap != NULL), sectorID at [eax+esi] (0x5490e8) */
/* TIER2: unvalidated authority=diffemu */
RwInt32 PVSWorldSectorNodeFn(RxPipelineNodeInstance * self __RWUNUSED__, const RxPipelineNodeParam * params)
{
    RwBool              visible;
    RpPVSCache         *pvsCache;
    RpPVS              *pvs;
    RpWorldSector      *worldSector;
    RpWorld            *world;

    worldSector = (RpWorldSector *) RxPipelineNodeParamGetData(params);

    world = rpPVSGlobals.World;

    pvs = PVSFROMWORLDSECTOR(worldSector);
    
    pvsCache = PVSCACHEFROMWORLD(world);

    visible = TRUE;

    if ((worldSector->numTriangles > 0) && (NULL != pvs->vismap))
    {
        RwInt32             sectorID = pvs->sectorID;

        pvsCache->ptotal += worldSector->numTriangles;

        if (PVSVISMAPGETSECTOR(pvs->vismap, sectorID))
        {
            visible = TRUE;
        }
        else
        {
            visible = FALSE;
        }
    }

    if (visible)
    {
        pvsCache->paccept += worldSector->numTriangles;

        RxPacketDispatch(NULL, 0, self);
    }

    return (TRUE);
}
