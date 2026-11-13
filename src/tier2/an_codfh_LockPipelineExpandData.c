/* LockPipelineExpandData (MFL.exe 004e6b90). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
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
/* TIER2: unvalidated authority=diffemu */
RwBool LockPipelineExpandData(RxPipeline *dstPipe, RxPipeline *srcPipe)
{
    RxPipelineNodeTopSortData *topSortArray;
    RwUInt32   *outputs;
    RwInt32     i;

    if (dstPipe != srcPipe)
    {

        for (i = srcPipe->numNodes - 1;i >= 0;--i)
        {
            memcpy(&(dstPipe->nodes[i]),
                   &(srcPipe->nodes[i]),
                   sizeof(RxPipelineNode));

            dstPipe->nodes[i].slotClusterRefs = (RxPipelineCluster **)NULL;
            dstPipe->nodes[i].slotsContinue = (RwUInt32 *)NULL;
            dstPipe->nodes[i].privateData = NULL;
            dstPipe->nodes[i].inputToClusterSlot = (RwUInt32 *)NULL;

            if (0 < dstPipe->nodes[i].initializationDataSize)
            {
                dstPipe->nodes[i].initializationData =
                    RwMalloc(dstPipe->nodes[i].initializationDataSize,
                         rwMEMHINTDUR_EVENT | rwID_PIPEMODULE);
                if (NULL == dstPipe->nodes[i].initializationData)
                {
                    RWERROR((E_RW_NOMEM,
                        dstPipe->nodes[i].initializationDataSize));
                    return (FALSE);
                }
                memcpy(dstPipe->nodes[i].initializationData,
                       srcPipe->nodes[i].initializationData,
                       dstPipe->nodes[i].initializationDataSize);
            }
        }
        
        dstPipe->numNodes = srcPipe->numNodes;
    }

    outputs = (RwUInt32 *)
        &(dstPipe->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);

    for (i = srcPipe->numNodes - 1;i >= 0;--i)
    {

        dstPipe->nodes[i].outputs = &(outputs[i*RXNODEMAXOUTPUTS]);
        if (NULL != srcPipe->nodes[i].outputs)
        {
            memcpy(dstPipe->nodes[i].outputs,
                   srcPipe->nodes[i].outputs,
                   RXNODEMAXOUTPUTS*sizeof(RwUInt32));
        }
        else
        {
            
        }
    }

    topSortArray = (RxPipelineNodeTopSortData *)
        &(outputs[RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe)]);
    for (i = 0;(RwUInt32)i < srcPipe->numNodes;++i)
    {
        topSortArray[i].numIns = 0;
        topSortArray[i].numInsVisited = 0;
        topSortArray[i].req = (rxReq *)NULL;
        dstPipe->nodes[i].topSortData = &(topSortArray[i]);
    }

    return (TRUE);
}
