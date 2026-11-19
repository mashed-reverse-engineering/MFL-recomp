/* RxPipelineLock (MFL.exe 004e6f40). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCriterionCoreID
{

    rwID_NACOREID = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    rwID_VECTORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),

    rwID_MATRIXMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),

    rwID_FRAMEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),

    rwID_STREAMMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),

    rwID_CAMERAMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),

    rwID_IMAGEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),

    rwID_RASTERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),

    rwID_TEXTUREMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),

    rwID_PIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),

    rwID_IMMEDIATEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),

    rwID_RESOURCESMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),

    rwID_DEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),

    rwID_COLORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),

    rwID_POLYPIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),

    rwID_ERRORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),

    rwID_METRICSMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),

    rwID_DRIVERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),

    rwID_CHUNKGROUPMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;
enum RwMemoryHintFlag
{

    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;
#define RXNODEMAXOUTPUTS 32
typedef struct rxReq rxReq;
struct RxPipelineNodeTopSortData
{
    RwUInt32 numIns;
    RwUInt32 numInsVisited;

    rxReq *req;

};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineNodeTopSortData) == 12);      /* MFL.exe 0x4e6d34-0x4e6d47 (LockPipelineExpandData): three words cleared, add eax,0xc per node */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_COREPLUGIN
#define CalcTopSortDataMemSize() ( sizeof(RxPipelineNodeTopSortData)*RXPIPELINEGLOBAL(maxNodesPerPipe) )
#define CalcNodeOutputsTopSortDataUnCompactedMemSize() (sizeof(RxPipelineNode )*RXPIPELINEGLOBAL(maxNodesPerPipe) + sizeof(RwUInt32)*RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe) + CalcTopSortDataMemSize() )
extern RwBool ReallocAndFixupSuperBlock(RxPipeline *pipeline, RwUInt32 newSize);
extern RwBool LockPipelineExpandData(RxPipeline *dstPipe, RxPipeline *srcPipe);
/* TIER2: validated 2026-11-14 authority=diffemu */
RxPipeline * RxPipelineLock(RxPipeline * pipeline)
{

    if (pipeline->locked == FALSE)
    {
        RwUInt32 lockedBlockSize;

        lockedBlockSize = CalcNodeOutputsTopSortDataUnCompactedMemSize();

        if (pipeline->nodes != NULL)
        {

            if (lockedBlockSize > pipeline->superBlockSize)
            {
                if (FALSE == ReallocAndFixupSuperBlock(
                                 pipeline, lockedBlockSize))
                {
                    return ((RxPipeline *)NULL);
                }
            }

            if (FALSE == LockPipelineExpandData(pipeline, pipeline))
            {
                return ((RxPipeline *)NULL);
            }
        }
        else
        {

            pipeline->superBlock = RwMalloc(lockedBlockSize,
                rwMEMHINTDUR_EVENT | rwMEMHINTFLAG_RESIZABLE | rwID_PIPEMODULE);
            if (NULL == pipeline->superBlock)
            {
                RWERROR((E_RW_NOMEM, lockedBlockSize));
                return ((RxPipeline *)NULL);
            }
            pipeline->superBlockSize = lockedBlockSize;

            pipeline->nodes = (RxPipelineNode *)pipeline->superBlock;
        }

        pipeline->locked = TRUE;
        if (pipeline->nodes != NULL)
        {
            RwUInt32            n;

            for (n = 0; n < pipeline->numNodes; ++n)
            {
                const RxNodeMethods *const nodeMethods =
                    &pipeline->nodes[n].nodeDef->nodeMethods;

                if (nodeMethods->pipelineNodeTerm != NULL)
                {
                    nodeMethods->pipelineNodeTerm(&pipeline->nodes[n]);
                }

                if (--(pipeline->nodes[n].nodeDef->InputPipesCnt) == 0)
                {
                    if (nodeMethods->nodeTerm != NULL)
                    {
                        nodeMethods->nodeTerm(pipeline->nodes[n].nodeDef);
                    }
                }

                pipeline->nodes[n].slotClusterRefs = (RxPipelineCluster **)NULL;
            }
        }
    }

    return (pipeline);
}
