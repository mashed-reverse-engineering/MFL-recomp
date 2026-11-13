/* RxLockedPipeUnlock (MFL.exe 004e64f0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * PipelineCalcNumUniqueClusters and the static helpers CalcNodesOutputsCompactedMemSize / CalcUnlockPersistentMemSize /
 * PipelineUnlockTopSort / PipelineTallyInputs inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by
 * rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by
 * differential emulation (tier 2). */
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
#define RXNODEMAXOUTPUTS 32
typedef struct rxReq rxReq;
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
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;
    RwUInt32 creationAttributes;
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
#define RXCHECKFORUSERTRAMPLING(_pipeline) 
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineNodeTopSortData) == 12);      /* MFL.exe 0x4e6d34-0x4e6d47 (LockPipelineExpandData): three words cleared, add eax,0xc per node */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RxPipelineCluster) == 8);      /* MFL.exe 0x4f3090 (MyEnumPipelineClustersCallBack): push 0x8 for StalacMiteAlloc */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RxPipelineCluster, creationAttributes) == 4);      /* MFL.exe 0x4f30ae (MyEnumPipelineClustersCallBack): mov [eax+0x4],ecx */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RxPipelineRequiresCluster) == 0xc);      /* MFL.exe 0x4e65d1..0x4e65df (RxLockedPipeUnlock): (numNodes + 5) * numClusters * 4 = numClusters * (sizeof(RxPipelineCluster) 8 + 0xc) + numNodes * numClusters * 4 */
RW_LAYOUT_ASSERT(rw37_row_4, sizeof(RxClusterRef) == 0xc);      /* MFL.exe 0x4e64b1..0x4e64c0 (PipelineCalcNumUniqueClusters): clustersOfInterest [ecx+0x24], clusterDef [edx], add edx,0xc per entry */
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
struct RwScopeTrace
{
        
    RwReqEntry          *head;
        
    RwScopeTrace        *continuation;
        
    RwScopeTrace        *next;

    RwScopeTrace        *parent;
};
extern RwUInt32
PipelineCalcNumUniqueClusters(RxPipeline *pipe);
extern RwUInt32     _rxChaseDependencies(RxPipeline *pipeline);
typedef struct P2MemoryLimits P2MemoryLimits;
struct P2MemoryLimits
{
    RwUInt8 *StalacTiteBase;
    RwUInt8 *StalacMiteBase;
};
typedef struct tagTopSortData TopSortData;
struct tagTopSortData
{
    RxPipeline         *pipeline;
    RwUInt32            nodesArraySlot;
};
#define ISNODELIVE(_node) ( NULL != ( (_node)->nodeDef) )
#define CalcTopSortDataMemSize() ( sizeof(RxPipelineNodeTopSortData)*RXPIPELINEGLOBAL(maxNodesPerPipe) )
#define CalcNodeOutputsTopSortDataUnCompactedMemSize() (sizeof(RxPipelineNode )*RXPIPELINEGLOBAL(maxNodesPerPipe) + sizeof(RwUInt32)*RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe) + CalcTopSortDataMemSize() )
#define CalcTempDepChaseMemSize(_pipeParam,_numClustersParam) ( _pipeParam->numNodes*sizeof(rxReq) + _pipeParam->numNodes*_numClustersParam*sizeof(RwReqEntry) + _pipeParam->numNodes*_numClustersParam*sizeof(RwScopeTrace) )
#define tramplingMagicValueSpace (0)
extern P2MemoryLimits gMemoryLimits;
extern RwUInt32 PipelineCalcNumUniqueClusters(RxPipeline *pipeline);
extern RwBool ReallocAndFixupSuperBlock(RxPipeline *pipeline, RwUInt32 newSize);
extern RwBool LockPipelineExpandData(RxPipeline *dstPipe, RxPipeline *srcPipe);
extern void PipelineTopSort(TopSortData *data, RwUInt32 nodeIndex);
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(TopSortData, nodesArraySlot) == 4 && sizeof(struct tagTopSortData) == 8);      /* MFL.exe 0x4e6664 / 0x4e6668: data.pipeline [esp+0x10], data.nodesArraySlot [esp+0x14] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(P2MemoryLimits, StalacMiteBase) == 4);      /* MFL.exe 0x4e664e / 0x4e6654: StalacMiteBase 0x7d4d20, StalacTiteBase 0x7d4d1c (gMemoryLimits) */
RW_LAYOUT_ASSERT(rw37_own_2, sizeof(rxReq) == 0x14 && sizeof(RwReqEntry) == 0x24 && sizeof(RwScopeTrace) == 0x10);      /* MFL.exe 0x4e65bb..0x4e65cf: per node and cluster (0x24 + 0x10) * numClusters + 0x14 (CalcTempDepChaseMemSize) */

/* TIER2: unvalidated authority=diffemu */
RxPipeline * RxLockedPipeUnlock(RxPipeline * pipeline)
{

    if ((NULL  != pipeline) &&
        (FALSE != pipeline->locked))
    {
        if (pipeline->numNodes != 0)
        {
            RxPipelineNodeTopSortData *newTopSortData;
            RxPipelineNodeTopSortData *topSortData;
            RwUInt32  numUniqueClusters;
            RwUInt32  unlockStartBlockSize;
            RwUInt32  unlockEndBlockSize;
            RwUInt32  topSortBlockSize;
            RwUInt32  depChaseBlockSize;
            RwUInt32  totalOutputs = 0;
            RwUInt32 *newOutputs;
            RwUInt32 *outputs;
            RwBool    error = FALSE;
            RwUInt32  doneNodes = 0;
            RwInt32   i;

            if (!( (pipeline->entryPoint < pipeline->numNodes) &&
                   (ISNODELIVE(&pipeline->nodes[pipeline->entryPoint])) )  )
            {
                RWERROR((E_RX_INVALIDENTRYPOINT));
                return ((RxPipeline *)NULL);
            }

            /* PipelineCalcNumUniqueClusters(pipeline) (MFL.exe 0x4e6480), inlined here in MFL.exe: count the distinct cluster
             * definitions of interest by stepping through them in address order */
            {
                RxClusterDefinition *lastAddress, *newAddress;
                RwUInt32             j, k;

                numUniqueClusters = 0;
                newAddress = (RxClusterDefinition *)NULL;
                while (1)
                {
                    lastAddress = newAddress;
                    newAddress  = (RxClusterDefinition *)0xFFFFFFFF;

                    for (j = 0;j < pipeline->numNodes;++j)
                    {
                        RxNodeDefinition *nodeDef = pipeline->nodes[j].nodeDef;
                        for (k = 0;k < nodeDef->io.numClustersOfInterest;++k)
                        {
                            RxClusterDefinition *address;
                            address = nodeDef->io.clustersOfInterest[k].clusterDef;
                            if ((address > lastAddress) &&
                                (address < newAddress ) )
                            {
                                newAddress = address;
                            }
                        }
                    }

                    if (newAddress == (RxClusterDefinition *)0xFFFFFFFF) break;
                    ++numUniqueClusters;
                }
            }

            topSortBlockSize = CalcNodeOutputsTopSortDataUnCompactedMemSize();

            depChaseBlockSize  = CalcTopSortDataMemSize();
            /* CalcNodesOutputsCompactedMemSize(pipeline), static, inlined (MFL.exe: node 0's output count for every node, as in the
             * source) */
            depChaseBlockSize += pipeline->numNodes*sizeof(RxPipelineNode)
                               + pipeline->numNodes*pipeline->nodes[0].numOutputs*sizeof(RwUInt32);
            depChaseBlockSize += CalcTempDepChaseMemSize(
                                     pipeline, numUniqueClusters);
            /* CalcUnlockPersistentMemSize(pipeline, numUniqueClusters), static, inlined */
            depChaseBlockSize += numUniqueClusters*sizeof(RxPipelineCluster)
                               + numUniqueClusters*sizeof(RxPipelineRequiresCluster)
                               + pipeline->numNodes*numUniqueClusters*sizeof(RxPipelineCluster *)
                               + pipeline->numNodes*(numUniqueClusters + 1)*sizeof(RwUInt32);
            for (i = 0;(RwUInt32)i < pipeline->numNodes;++i)
            {
                RxPipelineNode *node = &(pipeline->nodes[i]);

                if (node->nodeDef->pipelineNodePrivateDataSize != 0)
                {
                    depChaseBlockSize += tramplingMagicValueSpace +
                                         node->nodeDef->pipelineNodePrivateDataSize;
                }
                depChaseBlockSize += sizeof(RwUInt32)*node->nodeDef->io.numClustersOfInterest;
            }
            depChaseBlockSize += sizeof(RxPacket) + sizeof(RxCluster)*(numUniqueClusters - 1);

            unlockStartBlockSize = topSortBlockSize;
            if (depChaseBlockSize > unlockStartBlockSize)
            {
                unlockStartBlockSize = depChaseBlockSize;
            }

            if (unlockStartBlockSize > pipeline->superBlockSize)
            {
                if (FALSE == ReallocAndFixupSuperBlock(
                                 pipeline, unlockStartBlockSize))
                {
                    return ((RxPipeline *)NULL);
                }
            }

            gMemoryLimits.StalacTiteBase = ((RwUInt8 *)pipeline->superBlock) + unlockStartBlockSize;

            gMemoryLimits.StalacMiteBase = (RwUInt8 *) NULL;

            /* PipelineUnlockTopSort(pipeline) with PipelineTallyInputs, static, inlined in MFL.exe */
            {
                TopSortData         data;
                RxPipelineNode     *nodes;

                data.pipeline = pipeline;
                data.nodesArraySlot = 0;

                nodes = &pipeline->nodes[0];
                for (i = 0; (RwUInt32)i < pipeline->numNodes; ++i)
                {
                    if (ISNODELIVE(nodes))
                    {
                        nodes->topSortData->numInsVisited = 0;
                        nodes->topSortData->numIns = 0;
                    }
                    ++nodes;
                }

                nodes = &pipeline->nodes[0];
                for (i = 0; (RwUInt32)i < pipeline->numNodes; ++i)
                {
                    if (ISNODELIVE(nodes))
                    {
                        if (nodes->numOutputs != 0)
                        {
                            int                 j = nodes->numOutputs;
                            RwUInt32           *outs = &nodes->outputs[0];

                            do
                            {
                                if (*(RwInt32 *) outs != -1)
                                {
                                    pipeline->nodes[*outs].topSortData->numIns++;
                                }
                            }
                            while (outs++, --j);
                        }
                    }
                    ++nodes;
                }

                if (pipeline->nodes[pipeline->entryPoint].topSortData->numIns != 0)
                {
                    RWERROR((E_RX_INVALIDENTRYPOINT));
                    return ((RxPipeline *)NULL);
                }

                for (i = 0; (RwUInt32)i < pipeline->numNodes; ++i)
                {
                    if (((RwUInt32)i != pipeline->entryPoint) &&
                        (pipeline->nodes[i].topSortData->numIns == 0))
                    {
                        RWERROR((E_RX_FRAGMENTEDPIPELINE));
                        return ((RxPipeline *)NULL);
                    }
                }

                PipelineTopSort(&data, pipeline->entryPoint);

                for (i = 0; (RwUInt32)i < pipeline->numNodes; ++i)
                {
                    if (pipeline->nodes[i].topSortData->numIns !=
                        pipeline->nodes[i].topSortData->numInsVisited)
                    {
                        RWERROR((E_RX_CYCLICPIPELINE));
                        return ((RxPipeline *)NULL);
                    }
                }

                pipeline->entryPoint = 0;
            }

            outputs = (RwUInt32 *)&(pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            topSortData = (RxPipelineNodeTopSortData *)&
                (outputs[RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            topSortData += pipeline->numNodes - 1;
            newTopSortData = (RxPipelineNodeTopSortData *)
                (((RwUInt8 *)pipeline->superBlock) + unlockStartBlockSize);
            --newTopSortData;

            for (i = (pipeline->numNodes - 1);i >= 0;--i)
            {
                memcpy(newTopSortData,
                       topSortData,
                       sizeof(RxPipelineNodeTopSortData));
                pipeline->nodes[i].topSortData = newTopSortData;

                --topSortData;
                --newTopSortData;
            }

            newOutputs = (RwUInt32 *)&(pipeline->nodes[pipeline->numNodes]);
            for (i = 0;(RwUInt32)i < pipeline->numNodes;i++)
            {
                if (0 == pipeline->nodes[i].numOutputs)
                {
                    pipeline->nodes[i].outputs = (RwUInt32 *)NULL;
                }
                else
                {
                    memcpy(newOutputs,
                           outputs,
                           pipeline->nodes[i].numOutputs*sizeof(RwUInt32));
                    pipeline->nodes[i].outputs = newOutputs;
                }

                outputs      += RXNODEMAXOUTPUTS;
                newOutputs   += pipeline->nodes[i].numOutputs;
                totalOutputs += pipeline->nodes[i].numOutputs;
            }

            gMemoryLimits.StalacMiteBase  = (RwUInt8 *)&(outputs[totalOutputs]);
            gMemoryLimits.StalacTiteBase  = (RwUInt8 *)topSortData;

            if (_rxChaseDependencies(pipeline) != 0)
            {
                return ((RxPipeline *)NULL);
            }

            unlockEndBlockSize = gMemoryLimits.StalacMiteBase -
                                 (RwUInt8 *)pipeline->superBlock;
            if (FALSE == ReallocAndFixupSuperBlock(
                             pipeline, unlockEndBlockSize))
            {
                goto fail;
            }

            for (i = 0;(RwUInt32)i < pipeline->numNodes;++i)
            {
                pipeline->nodes[i].topSortData = (RxPipelineNodeTopSortData *)NULL;
            }

            RXCHECKFORUSERTRAMPLING(pipeline);

            for (i = (pipeline->numNodes - 1);i >= 0;--i)
            {
                RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                RxPipelineNode   *node    = &(pipeline->nodes[i]);

                if (nodeDef->InputPipesCnt++ == 0)
                {
                    if (NULL != nodeDef->nodeMethods.nodeInit)
                    {
                        if (nodeDef->nodeMethods.nodeInit(nodeDef) == FALSE)
                        {
                            doneNodes = (pipeline->numNodes - 1) - i;
                            error = TRUE;
                            break;
                        }
                    }
                }

                if (nodeDef->nodeMethods.pipelineNodeInit != NULL)
                {
                    if (nodeDef->nodeMethods.pipelineNodeInit(node) == FALSE)
                    {
                        --nodeDef->InputPipesCnt;
                        if (0 == nodeDef->InputPipesCnt)
                        {
                            if (NULL != nodeDef->nodeMethods.nodeTerm)
                            {
                                nodeDef->nodeMethods.nodeTerm(nodeDef);
                            }
                        }
                        doneNodes = (pipeline->numNodes - 1) - i;
                        error = TRUE;
                        break;
                    }
                }
            }

            if (FALSE == error)
            {
                for (i = (pipeline->numNodes - 1);i >= 0;--i)
                {
                    RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                    RxPipelineNode   *node    = &(pipeline->nodes[i]);

                    if (nodeDef->nodeMethods.pipelineNodeConfig != NULL)
                    {
                        if (nodeDef->nodeMethods.pipelineNodeConfig(
                                node, pipeline) == FALSE)
                        {
                            doneNodes = pipeline->numNodes;
                            error = TRUE;
                            break;
                        }
                    }
                }
            }

            if (FALSE != error)
            {
                for (i = (pipeline->numNodes - 1) - (doneNodes - 1);
                     (RwUInt32)i < pipeline->numNodes;
                     ++i)
                {
                    RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                    RxPipelineNode   *node    = &(pipeline->nodes[i]);

                    if (nodeDef->nodeMethods.pipelineNodeTerm != NULL)
                    {
                        nodeDef->nodeMethods.pipelineNodeTerm(node);
                    }

                    nodeDef->InputPipesCnt--;
                    if (0 == nodeDef->InputPipesCnt)
                    {
                        if (NULL != nodeDef->nodeMethods.nodeTerm)
                        {
                            nodeDef->nodeMethods.nodeTerm(nodeDef);
                        }
                    }
                }

                goto fail;
            }

            RXCHECKFORUSERTRAMPLING(pipeline);
        }

        pipeline->locked = FALSE;

        return (pipeline);
    }

    if (pipeline == NULL)
    {
        RWERROR((E_RW_NULLP));
    }
    else
    {
        RWERROR((E_RX_UNLOCKEDPIPE));
    }

    return ((RxPipeline *)NULL);

  fail:

    LockPipelineExpandData(pipeline, pipeline);

    return ((RxPipeline *)NULL);
}
