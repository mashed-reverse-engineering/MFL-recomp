/* NodeCreate (MFL.exe 004e7300). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
enum RwErrorCodePlugin_errcore
{
    E_RW_BADWINDOW,
    E_RW_CHUNKTYPEGET,
    E_RW_DEVICEERROR,
    E_RW_DEVICEOPEN,
    E_RW_DEVICESTART,
    E_RW_ENDOFSTREAM,
    E_RW_FRAMEDESTROYSTATIC,
    E_RW_FRAMEBUFFERMISMATCH,
    E_RW_FREELISTFREED,
    E_RW_FREELISTINVADDRESS,
    E_RW_FREELISTINVMEMBOUND,
    E_RW_FREELISTTRASH,
    E_RW_INSUFFICIENTRESOURCES,
    E_RW_INVSTREAMACCESSTYPE,
    E_RW_INVSTREAMTYPE,
    E_RW_NEGCLIPPLANE,
    E_RW_NOCAMERA,
    E_RW_NOFRAMEBUFFER,
    E_RW_NOPARENT,
    E_RW_RASTERRECT,
    E_RW_RASTERSTACK,
    E_RW_RASTERSTACKEMPTY,
    E_RW_READTEXMASK,
    E_RW_STREAMOPEN,
    E_RW_SYSTEMREQUEST,
    E_RW_ZEROLENGTH,
    E_RX_CANNOT_TRANSFER_BETWEEN_NODES,
    E_RX_CANNOT_TRANSFER_FROM_NODE_TO_PIPELINE,
    E_RX_CYCLICPIPELINE,
    E_RX_DEP_DEPENDENCIESMISMATCH,
    E_RX_DEP_DUPLICATECLUSTERDEFS,
    E_RX_DEP_NULLCLUSTERDEF,
    E_RX_DEP_OUTOFMEMORY,
    E_RX_EMPTYPIPELINE,
    E_RX_FRAGMENTEDPIPELINE,
    E_RX_IM3DNOTACTIVE,
    E_RX_INVALIDENTRYPOINT,
    E_RX_INVALIDPRIMTYPE,
    E_RX_INVALIDRESUMEPIPELINE,
    E_RX_LOCKEDPIPE,
    E_RX_NODETOOMANYCLUSTERSOFINTEREST,
    E_RX_NODETOOMANYOUTPUTS,
    E_RX_PIPELINETOOMANYNODES,
    E_RX_NODE_EXECUTION_FAILED,
    E_RX_NOVERTS,
    E_RX_PACKETPTRINVALID,
    E_RX_PACKETSCOPYFAILED,
    E_RX_RUNFROMNOTVALID,
    E_RX_RUNTOANDRUNFROM,
    E_RX_RUNTONOTVALID,
    E_RX_TOOMANYVERTS,
    E_RX_UNFINISHEDPIPELINE,
    E_RX_UNLOCKEDPIPE,
    E_RX_UNPROCESSED_PACKETS,
    E_RX_UNSATISFIED_REQUIREMENTS,
    E_RW_INSUFFICIENTRASTER,
    E_RW_INSUFFICIENTDIC,
    rwLASTERROR_errcore = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodePlugin_errcore RwErrorCodePlugin_errcore;
enum RwDebugType
{
    rwNADEBUGTYPE = 0,
    rwDEBUGASSERT,
    rwDEBUGERROR,
    rwDEBUGMESSAGE,
    rwDEBUGTRACE,
    rwDEBUGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwDebugType RwDebugType;
#define RwDebugSendMessage(type,funcName,message) 
extern RwInt32 _rxPipelineMaxNodes;
#define RXNODEMAXCLUSTERSOFINTEREST 32
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
RwBool NodeCreate(RxPipeline * pipeline, RxPipelineNode * node, RxNodeDefinition * nodespec)
{
    RxPipelineNodeTopSortData *topSortData;
    RwUInt32           *outputs;
    RwBool              result = TRUE;
    RwUInt32            n;

    n = nodespec->io.numOutputs;

    memset(node, 0x00U, sizeof(RxPipelineNode));

    if (n > RXNODEMAXOUTPUTS)
    {
        RWERROR((E_RX_NODETOOMANYOUTPUTS));
        result = FALSE;
    }

    if (nodespec->io.numClustersOfInterest > RXNODEMAXCLUSTERSOFINTEREST)
    {
        RWERROR((E_RX_NODETOOMANYCLUSTERSOFINTEREST));
        result = FALSE;
    }

    if (RXPIPELINEGLOBAL(maxNodesPerPipe) !=
        (RwUInt32) _rxPipelineMaxNodes)
    {
        RwDebugSendMessage(rwDEBUGMESSAGE,
                           RWSTRING("_NodeCreate"),
                           RWSTRING("_rxPipelineMaxNodes should only be changed BEFORE initializing RenderWare!"));
    }

    if (n >= RXPIPELINEGLOBAL(maxNodesPerPipe))
    {
        RWERROR((E_RX_PIPELINETOOMANYNODES));
        result = FALSE;
    }

    if (FALSE != result)
    {

        outputs = (RwUInt32 *)
            & (pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
        outputs += pipeline->numNodes * RXNODEMAXOUTPUTS;

        node->outputs = outputs;
        node->numOutputs = n;

        for (n = 0; n < node->numOutputs; ++n)
        {
            *outputs = (RwUInt32) - 1;
            ++outputs;
        }

        outputs = (RwUInt32 *)
            & (pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
        topSortData = (RxPipelineNodeTopSortData *)
            & (outputs[RXPIPELINEGLOBAL(maxNodesPerPipe) * RXNODEMAXOUTPUTS]);
        topSortData += pipeline->numNodes;

        topSortData->numIns = 0;
        topSortData->numInsVisited = 0;
        topSortData->req = (rxReq *) NULL;
        node->topSortData = topSortData;

        node->initializationData = NULL;
        node->initializationDataSize = 0;

        node->nodeDef = nodespec; 

        ++pipeline->numNodes;
    }

    return (result);
}
