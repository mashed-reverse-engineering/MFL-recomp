/* rxChaseDependencies (MFL.exe 004f21b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper _PropagateDependenciesAndKillDeadPaths inlined by hand (MFL.exe has it inlined), an empty if dropped, release build macros
 * expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
enum RxClusterValid
{
    rxCLVALID_NOCHANGE = 0,

    rxCLVALID_VALID = 1,

    rxCLVALID_INVALID = 2,

    rxCLUSTERVALIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef struct rxReq rxReq;
typedef enum RxClusterValid RxClusterValid;
struct RxOutputSpec
{
     RwChar *name;
     RxClusterValid *outputClusters;
     RxClusterValid allOtherClusters;
};
enum RxClusterForcePresent
{
    rxCLALLOWABSENT = FALSE,

    rxCLFORCEPRESENT = TRUE,

    rxCLUSTERFORCEPRESENTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxClusterForcePresent RxClusterForcePresent;
struct RxClusterRef
{
    RxClusterDefinition *clusterDef;
    RxClusterForcePresent forcePresent;
    RwUInt32 reserved;
};
struct RxPipelineNodeTopSortData
{
    RwUInt32 numIns;
    RwUInt32 numInsVisited;

    rxReq *req;

};
struct RxPipelineRequiresCluster
{
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq rqdOrOpt;
    RwUInt32 slotIndex;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineNodeTopSortData) == 12);      /* MFL.exe 0x4e6d34-0x4e6d47 (LockPipelineExpandData): three words cleared, add eax,0xc per node */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RxOutputSpec) == 0xc && RW_OFFSETOF(RxOutputSpec, outputClusters) == 4 && RW_OFFSETOF(RxOutputSpec, allOtherClusters) == 8);      /* MFL.exe 0x4f2291..0x4f22dc (rxChaseDependencies): outspec = io.outputs + j * 0xc ([esp+0x20] += 0xc), outputClusters [ebx+4], allOtherClusters [ebx+8] */
RW_LAYOUT_ASSERT(rw37_row_2, sizeof(RxPipelineRequiresCluster) == 0xc);      /* MFL.exe 0x4e65d1..0x4e65df (RxLockedPipeUnlock): (numNodes + 5) * numClusters * 4 = numClusters * (sizeof(RxPipelineCluster) 8 + 0xc) + numNodes * numClusters * 4 */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RxClusterRef) == 0xc);      /* MFL.exe 0x4e64b1..0x4e64c0 (PipelineCalcNumUniqueClusters): clustersOfInterest [ecx+0x24], clusterDef [edx], add edx,0xc per entry */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_COREPLUGIN
typedef struct RwReqEntry RwReqEntry;
typedef struct RwScopeTrace RwScopeTrace;
struct rxReq
{
    RwUInt32             numused;
    RwUInt32             numalloced;
    RwUInt32             unavailslots;
        
    RwReqEntry          *reqentries;
        
    RxPipelineNode      *node;
};
struct RwReqEntry
{
        
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq required;
    RwUInt32             inputs;

    RwScopeTrace        *st;

    RwReqEntry          *next;
        
    rxReq               *req;
    RwUInt32             outbf;
    RwUInt32             assignedslot;

    RxPipelineNode      *originatingNode;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, required) == 4 && RW_OFFSETOF(RwReqEntry, originatingNode) == 0x20);      /* MFL.exe 0x4f22a0 req->numused [eax], 0x4f22b2 reqentries [eax+0xc], 0x4f23ad entry stride 0x24, 0x4f22eb required [esi+4], 0x4f22e7 originatingNode [esi+0x20] */
extern RwUInt32
PipelineCalcNumUniqueClusters(RxPipeline *pipe);
#define _ReqNumEntries(_req) ( (_req)->numused )
#define _ReqGetEntry(_req,_n) ( ( ((RwUInt32) (_n)) < (_req)->numused ) ? &(_req)->reqentries[(_n)] : (RwReqEntry *) NULL )
extern rxReq * _ReqCreate(RxPipelineNode *node, RwUInt32 numClusters);
extern RwReqEntry * _ReqAddEntry(rxReq *req, RxClusterDefinition *clusterDef, RxClusterValidityReq required, RwUInt32 inputs, RxPipelineNode *originatingNode);
extern RwUInt32 _IoSpecSearch4Cluster(RxIoSpec * iospec, RxClusterDefinition * clusterDef);
extern void _PropDownElimPath(RxPipeline * pipeline, RxPipelineNode * node, RxClusterDefinition * clusterDef);
#define REQSDUMP(pipeline) 
#define ALLOCSDUMP(pipeline) 
#define SCOPETRACESDUMP(head) 
#define E_RX_DEP_SUCCESS 0
extern RwUInt32 _ForAllNodeReqsAddOutputClustersAndBuildContinuityBitfields(RxPipeline * pipeline);
extern RwUInt32 _TraceClusterScopes(RxPipeline * pipeline, RwScopeTrace ** stheadref);
extern RwUInt32 _AssignClusterSlots(RxPipeline * pipeline, RwScopeTrace ** stheadref);
extern RwUInt32 _ForAllNodesWriteClusterAllocations(RxPipeline *pipeline, RwScopeTrace *stHead);
/* TIER2: unvalidated authority=diffemu */
RwUInt32 rxChaseDependencies(RxPipeline * pipeline)
{
    RwScopeTrace       *sthead = (RwScopeTrace *)NULL;
    RwUInt32            result;

    /* _PropagateDependenciesAndKillDeadPaths(pipeline), static in p2dep.c, inlined in MFL.exe (0x4f21b3..0x4f244d): from the last node
     * back to the first, each node's requirements = what its outputs' nodes need (passed through, or checked against its output spec)
     * plus its own input requirements */
    {
        RwUInt32        numUniqueClusters;
        RwUInt32        i, j, k;
        RxPipelineNode *node;

        i = pipeline->numNodes;
        node = &pipeline->nodes[pipeline->numNodes - 1];

        do
        {
            RxIoSpec *iospec = &node->nodeDef->io;

            for (j = 0;j < node->nodeDef->io.numClustersOfInterest;++j)
            {
                RxClusterDefinition *cluster =
                    node->nodeDef->io.clustersOfInterest[j].clusterDef;
                if (cluster == NULL)
                {
                    RWERROR((E_RX_DEP_NULLCLUSTERDEF, node->nodeDef->name, j));
                    result = E_RX_DEP_NULLCLUSTERDEF;
                    goto Exit;
                }

                for (k = j + 1;k < node->nodeDef->io.numClustersOfInterest;++k)
                {
                    RxClusterDefinition *cluster2 =
                        node->nodeDef->io.clustersOfInterest[k].clusterDef;
                    if (cluster == cluster2)
                    {
                        RWERROR((E_RX_DEP_DUPLICATECLUSTERDEFS,
                                 node->nodeDef->name, cluster2->name));
                        result = E_RX_DEP_DUPLICATECLUSTERDEFS;
                        goto Exit;
                    }
                }
            }

            numUniqueClusters = PipelineCalcNumUniqueClusters(pipeline);
            if ((node->topSortData->req = _ReqCreate(node, numUniqueClusters)) == NULL)
            {
                RWERROR((E_RX_DEP_OUTOFMEMORY));
                result = E_RX_DEP_OUTOFMEMORY;
                goto Exit;
            }

            for (j = 0; j < node->numOutputs; ++j)
            {
                if (node->outputs[j] != (RwUInt32) -1)
                {
                    RxPipelineNode *outnode = &pipeline->nodes[node->outputs[j]];
                    RxOutputSpec   *outspec = &node->nodeDef->io.outputs[j];

                    for (k = 0; k < _ReqNumEntries(outnode->topSortData->req); ++k)
                    {
                        RwReqEntry *reqentry = _ReqGetEntry(outnode->topSortData->req, k);

                        RwUInt32 n = _IoSpecSearch4Cluster(
                                         iospec, reqentry->clusterDef);
                        RxClusterValid  presinout = (n == (RwUInt32)-1) ?
                                                    outspec->allOtherClusters :
                                                    outspec->outputClusters[n];

                        if (presinout == rxCLVALID_NOCHANGE)
                        {
                            if (!_ReqAddEntry(node->topSortData->req,
                                              reqentry->clusterDef,
                                              reqentry->required,
                                              node->topSortData->numIns,
                                              reqentry->originatingNode))
                            {
                                RWERROR((E_RX_DEP_OUTOFMEMORY));
                                result = E_RX_DEP_OUTOFMEMORY;
                                goto Exit;
                            }
                        }
                        else
                        {
                            if (reqentry->required == rxCLREQ_REQUIRED)
                            {
                                if (presinout != rxCLVALID_VALID)
                                {
                                    RWERROR((E_RX_DEP_DEPENDENCIESMISMATCH,
                                             reqentry->clusterDef->name,
                                             reqentry->originatingNode->nodeDef->name,
                                             node->nodeDef->name,
                                             j,
                                             outspec->name));
                                    result = E_RX_DEP_DEPENDENCIESMISMATCH;
                                    goto Exit;
                                }
                            }
                            else
                            {
                                if (presinout == rxCLVALID_INVALID)
                                {
                                    _PropDownElimPath(pipeline, outnode,
                                                      reqentry->clusterDef);
                                }
                            }
                        }
                    }
                }
            }

            for (j = 0; j < iospec->numClustersOfInterest; ++j)
            {
                if (iospec->inputRequirements[j] != rxCLREQ_DONTWANT)
                {
                    if (!_ReqAddEntry(node->topSortData->req,
                                      iospec->
                                      clustersOfInterest[j].clusterDef,
                                      iospec->inputRequirements[j],
                                      node->topSortData->numIns, node))
                    {
                        RWERROR((E_RX_DEP_OUTOFMEMORY));
                        result = E_RX_DEP_OUTOFMEMORY;
                        goto Exit;
                    }
                }
            }
        }
        while (node--, --i);
    }

    result =
        _ForAllNodeReqsAddOutputClustersAndBuildContinuityBitfields(pipeline);

    if (result != E_RX_DEP_SUCCESS)
    {
        goto Exit;
    }

    REQSDUMP(pipeline);

    result = _TraceClusterScopes(pipeline, &sthead);

    if (result != E_RX_DEP_SUCCESS)
    {
        goto Exit;
    }

    result = _AssignClusterSlots(pipeline, &sthead);

    if (result != E_RX_DEP_SUCCESS)
    {
        goto Exit;
    }

    SCOPETRACESDUMP(sthead);

    result = _ForAllNodesWriteClusterAllocations(pipeline, sthead);

    if (result != E_RX_DEP_SUCCESS)
    {
        goto Exit;
    }

    ALLOCSDUMP(pipeline);

 Exit:

    return (result);
}
