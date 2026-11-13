/* _ForAllNodesWriteClusterAllocations (MFL.exe 004f2d10). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers _CountHeadNodeRqdsAndOpts / _EnumPipelineClusters (with its callback _MyEnumPipelineClustersCallBack) /
 * _WriteHeadNodeRqdsAndOpts2PipelineRequirements inlined by hand (MFL.exe has them inlined), the function under its codfh name (as
 * rxChaseDependencies calls it), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation
 * resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their
 * addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
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
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineNodeTopSortData) == 12);      /* MFL.exe 0x4e6d34-0x4e6d47 (LockPipelineExpandData): three words cleared, add eax,0xc per node */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RxPipelineCluster) == 8);      /* MFL.exe 0x4f3090 (MyEnumPipelineClustersCallBack): push 0x8 for StalacMiteAlloc */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RxPipelineCluster, creationAttributes) == 4);      /* MFL.exe 0x4f30ae (MyEnumPipelineClustersCallBack): mov [eax+0x4],ecx */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RxPipelineRequiresCluster) == 0xc);      /* MFL.exe 0x4e65d1..0x4e65df (RxLockedPipeUnlock): (numNodes + 5) * numClusters * 4 = numClusters * (sizeof(RxPipelineCluster) 8 + 0xc) + numNodes * numClusters * 4 */
RW_LAYOUT_ASSERT(rw37_row_4, sizeof(RxClusterRef) == 0xc);      /* MFL.exe 0x4e64b1..0x4e64c0 (PipelineCalcNumUniqueClusters): clustersOfInterest [ecx+0x24], clusterDef [edx], add edx,0xc per entry */
/* ---- end of the generated declarations ---- */

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
extern void
_rx_rxRadixExchangeSort(void *elements, 
                  RwUInt32 numElements, 
                  RwUInt32 elementSize,
                  RwUInt32 elementKeyOffset,
                  RwUInt32 keyLo, 
                  RwUInt32 keyHi);
extern void *
StalacTiteAlloc(RwUInt32 size);
extern void *
StalacMiteAlloc(RwUInt32 size);
#define _ReqNumEntries(_req) ( (_req)->numused )
#define _ReqGetEntry(_req,_n) ( ( ((RwUInt32) (_n)) < (_req)->numused ) ? &(_req)->reqentries[(_n)] : (RwReqEntry *) NULL )
#define E_RX_DEP_SUCCESS 0
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, required) == 4 && RW_OFFSETOF(RwReqEntry, outbf) == 0x18 && RW_OFFSETOF(RwReqEntry, assignedslot) == 0x1c);      /* MFL.exe 0x4f2d38 reqentries [esi+0xc], stride 0x24 (0x4f2d50), required [eax+4] (0x4f2d41), outbf [eax+0x18] (0x4f2f26), assignedslot [eax+0x1c] (0x4f2f16) */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(RwScopeTrace) == 0x10 && RW_OFFSETOF(RwScopeTrace, next) == 8);      /* MFL.exe 0x4f2d7b / 0x4f2d89 st->next [+8], st->head [+0] (0x4f2d71) */

/* TIER2: unvalidated authority=diffemu */
RwUInt32 _ForAllNodesWriteClusterAllocations(RxPipeline *pipeline, RwScopeTrace *stHead)
{
    RwUInt32            numPipelineRequiresClusters;
    RwUInt32            numPipelineClusters;
    RxPipelineCluster  *pipelineClusters;
    RxPipelineNode     *node;
    RwUInt8            *titeBase, *miteBase;
    RwUInt32            slotsContinueAND;
    RwUInt32            i, j, k;
    RwScopeTrace       *st, *stdedupe;

    /* _CountHeadNodeRqdsAndOpts(pipeline), static in p2dep.c, inlined in MFL.exe */
    node = &pipeline->nodes[0];
    numPipelineRequiresClusters = 0;
    for (i = 0; i < _ReqNumEntries(node->topSortData->req); ++i)
    {
        RwReqEntry *reqentry = _ReqGetEntry(node->topSortData->req, i);

        if (reqentry->required == rxCLREQ_REQUIRED ||
            reqentry->required == rxCLREQ_OPTIONAL    )
        {
            ++numPipelineRequiresClusters;
        }
    }

    /* _EnumPipelineClusters(stHead, NULL, NULL), static, inlined: the number of distinct cluster definitions over the scope traces */
    numPipelineClusters = 0;
    for (st = stHead; st != NULL; st = st->next)
    {
        for (stdedupe = stHead; stdedupe != st; stdedupe = stdedupe->next)
        {
            if (stdedupe->head->clusterDef == st->head->clusterDef)
            {
                break;
            }
        }
        if (stdedupe == st)
        {
            ++numPipelineClusters;
        }
    }

    titeBase = (RwUInt8 *)StalacTiteAlloc(0);

    miteBase = (RwUInt8 *)StalacMiteAlloc(0);

    i = titeBase - miteBase;

    memset(miteBase, 0, i);

    pipeline->embeddedPacket = (RxPacket *)
        StalacMiteAlloc(
            sizeof(RxPacket) +

            (pipeline->packetNumClusterSlots - 1)*sizeof(RxCluster));

    pipeline->embeddedPacket->numClusters =
        (RwUInt16)pipeline->packetNumClusterSlots;
    pipeline->embeddedPacket->pipeline = pipeline;
    pipeline->embeddedPacketState = rxPKST_PACKETLESS;

    /* _EnumPipelineClusters(stHead, _MyEnumPipelineClustersCallBack, &pipelineClusters) with the callback, inlined: one pipeline
     * cluster record per distinct cluster definition, in a row from the stalagmite */
    pipelineClusters = (RxPipelineCluster *) NULL;
    for (st = stHead; st != NULL; st = st->next)
    {
        for (stdedupe = stHead; stdedupe != st; stdedupe = stdedupe->next)
        {
            if (stdedupe->head->clusterDef == st->head->clusterDef)
            {
                break;
            }
        }
        if (stdedupe == st)
        {
            RxClusterDefinition *clusterdef = st->head->clusterDef;
            RxPipelineCluster   *pipelineCluster;

            pipelineCluster = (RxPipelineCluster *)
                StalacMiteAlloc(sizeof(RxPipelineCluster));

            if (NULL == pipelineClusters) pipelineClusters = pipelineCluster;

            pipelineCluster->clusterRef = clusterdef;
            pipelineCluster->creationAttributes = clusterdef->defaultAttributes;
        }
    }

    for (i = 0;i < pipeline->numNodes;++i)
    {
        node = &(pipeline->nodes[i]);

        if (0 < pipeline->packetNumClusterSlots)
        {
            node->slotClusterRefs = (RxPipelineCluster **)StalacMiteAlloc(
                pipeline->packetNumClusterSlots*sizeof(RxPipelineCluster *));
        }

        node->slotsContinue = (RwUInt32 *)StalacMiteAlloc(
            (1 + pipeline->packetNumClusterSlots)*sizeof(RwUInt32));

        if (0 < node->nodeDef->io.numClustersOfInterest)
        {
            node->inputToClusterSlot = (RwUInt32 *)StalacMiteAlloc(
                node->nodeDef->io.numClustersOfInterest*sizeof(RwUInt32));
        }

        if (node->nodeDef->pipelineNodePrivateDataSize != 0)
        {
            j = (node->nodeDef->pipelineNodePrivateDataSize + 3) & ~3;
            node->privateData = (void *)StalacMiteAlloc(j);
        }

        slotsContinueAND = ~0;
        for (j = 0; j < _ReqNumEntries(node->topSortData->req); ++j)
        {
            RxPipelineCluster *pipelineCluster = (RxPipelineCluster *)NULL;
            RwReqEntry        *reqEntry;

            reqEntry = _ReqGetEntry(node->topSortData->req, j);

            for (k = 0;k < numPipelineClusters;++k)
            {
                if (pipelineClusters[k].clusterRef == reqEntry->clusterDef)
                {
                    pipelineCluster = &(pipelineClusters[k]);
                    break;
                }
            }

            node->slotClusterRefs[reqEntry->assignedslot] =
                pipelineCluster;
            node->slotsContinue[1 + reqEntry->assignedslot] =
                reqEntry->outbf;

            slotsContinueAND &= reqEntry->outbf;
        }

        node->slotsContinue[0] = slotsContinueAND;

        for (j = 0; j < node->nodeDef->io.numClustersOfInterest; ++j)
        {
            node->inputToClusterSlot[j] = (RwUInt32)-1;
            for (k = 0; k < pipeline->packetNumClusterSlots; ++k)
            {
                if ((node->slotClusterRefs[k] != NULL) &&
                    (node->slotClusterRefs[k]->clusterRef ==
                     node->nodeDef->io.clustersOfInterest[j].clusterDef))
                {
                    node->inputToClusterSlot[j] = k;
                    break;
                }
            }
        }
    }

    if (0 < numPipelineRequiresClusters)
    {
        pipeline->inputRequirements = (RxPipelineRequiresCluster *)
            StalacMiteAlloc(sizeof(RxPipelineRequiresCluster)*
                            numPipelineRequiresClusters);
    }

    /* _WriteHeadNodeRqdsAndOpts2PipelineRequirements(pipeline), static, inlined: the head node's required / optional clusters,
     * sorted by cluster definition address */
    {
        RwUInt32        count = 0;

        node = &pipeline->nodes[0];
        for (i = 0; i < _ReqNumEntries(node->topSortData->req); ++i)
        {
            RwReqEntry *reqentry = _ReqGetEntry(node->topSortData->req, i);

            if ((reqentry->required == rxCLREQ_REQUIRED) ||
                (reqentry->required == rxCLREQ_OPTIONAL)    )
            {
                RxPipelineRequiresCluster *reqcl =
                    &(pipeline->inputRequirements[count++]);

                reqcl->clusterDef = reqentry->clusterDef;
                reqcl->rqdOrOpt   = reqentry->required;
                reqcl->slotIndex  = reqentry->assignedslot;
            }
        }

        pipeline->numInputRequirements = count;

        _rx_rxRadixExchangeSort(pipeline->inputRequirements,
                          pipeline->numInputRequirements,
                          sizeof(RxPipelineRequiresCluster),
                          offsetof(RxPipelineRequiresCluster, clusterDef),
                          0x00000000U,
                          0xFFFFFFFFU  );
    }

    return (E_RX_DEP_SUCCESS);
}
