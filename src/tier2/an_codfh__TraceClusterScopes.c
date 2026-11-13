/* _TraceClusterScopes (MFL.exe 004f2a50). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers _ReqSearch4Cluster / _ScopeTraceCreate / _ScopeTraceAddEntry / _ScopeTraceMerge inlined by hand (MFL.exe has them
 * inlined), the function under its codfh name (as rxChaseDependencies calls it), release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
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
typedef struct rxReq rxReq;
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
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RxPipelineRequiresCluster) == 0xc);      /* MFL.exe 0x4e65d1..0x4e65df (RxLockedPipeUnlock): (numNodes + 5) * numClusters * 4 = numClusters * (sizeof(RxPipelineCluster) 8 + 0xc) + numNodes * numClusters * 4 */
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
extern void *
StalacTiteAlloc(RwUInt32 size);
#define _ReqNumEntries(_req) ( (_req)->numused )
#define _ReqGetEntry(_req,_n) ( ( ((RwUInt32) (_n)) < (_req)->numused ) ? &(_req)->reqentries[(_n)] : (RwReqEntry *) NULL )
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, st) == 0xc && RW_OFFSETOF(RwReqEntry, next) == 0x10 && RW_OFFSETOF(RwReqEntry, outbf) == 0x18);      /* MFL.exe 0x4f2a84 reqentries [eax+0xc], 0x4f2ad0 stride 0x24, 0x4f2a8d st [esi+0xc], 0x4f2ac8 next [esi+0x10], 0x4f2b72 outbf [eax+0x18] */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(RwScopeTrace) == 0x10 && RW_OFFSETOF(RwScopeTrace, continuation) == 4 && RW_OFFSETOF(RwScopeTrace, next) == 8 && RW_OFFSETOF(RwScopeTrace, parent) == 0xc);      /* MFL.exe 0x4f2a92 StalacTiteAlloc(0x10); 0x4f2aa4..0x4f2ab1 continuation +4, head +0, parent +0xc, next +8 */
#define E_RX_DEP_SUCCESS 0

/* the scope trace helpers of p2dep.c (static), inlined in MFL.exe: _ReqSearch4Cluster, _ScopeTraceCreate, _ScopeTraceAddEntry,
 * _ScopeTraceMerge */
#define REQSEARCH4CLUSTER(_res, _req, _clusterDef)                      \
MACRO_START                                                             \
{                                                                       \
    RwReqEntry *_e = &((_req)->reqentries[0]);                          \
    RwUInt32    _n;                                                     \
                                                                        \
    (_res) = (RwReqEntry *) NULL;                                       \
    for (_n = 0; _n < (_req)->numused; ++_n)                            \
    {                                                                   \
        if (_e->clusterDef == (_clusterDef))                            \
        {                                                               \
            (_res) = _e;                                                \
            break;                                                      \
        }                                                               \
        ++_e;                                                           \
    }                                                                   \
}                                                                       \
MACRO_STOP
#define SCOPETRACEADDENTRY(_scopetrace, _reqentry)                      \
MACRO_START                                                             \
{                                                                       \
    (_reqentry)->next = (_scopetrace)->head;                            \
    (_scopetrace)->head = (_reqentry);                                  \
}                                                                       \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
RwUInt32 _TraceClusterScopes(RxPipeline * pipeline, RwScopeTrace ** stheadref)
{
    RwUInt32            i, j, k;
    RxPipelineNode     *node;

    i = pipeline->numNodes;
    node = &pipeline->nodes[0];

    do
    {
        for (k = 0; k < _ReqNumEntries(node->topSortData->req); ++k)
        {
            RwReqEntry *reqentry = _ReqGetEntry(node->topSortData->req, k);

            if (reqentry->st == NULL)
            {
                /* _ScopeTraceCreate(stheadref) */
                RwScopeTrace *scopetrace = (RwScopeTrace *)StalacTiteAlloc(sizeof(RwScopeTrace));

                if (scopetrace != NULL)
                {
                    scopetrace->continuation = (RwScopeTrace *)NULL;
                    scopetrace->head = (RwReqEntry *)NULL;
                    scopetrace->parent = (RwScopeTrace *)NULL;

                    scopetrace->next = *stheadref;
                    *stheadref = scopetrace;
                }

                if ((reqentry->st = scopetrace) == NULL)
                {
                    RWERROR((E_RX_DEP_OUTOFMEMORY));
                    return (E_RX_DEP_OUTOFMEMORY);
                }

                SCOPETRACEADDENTRY(reqentry->st, reqentry);
            }
        }

        for (j = 0; j < node->numOutputs; ++j)
        {
            if (node->outputs[j] != (RwUInt32) ~ 0)
            {
                RxPipelineNode     *outnode =
                    &pipeline->nodes[node->outputs[j]];

                for (k = 0; k < _ReqNumEntries(outnode->topSortData->req); ++k)
                {
                    RwReqEntry *reqentry =
                        _ReqGetEntry(outnode->topSortData->req, k);

                    if (reqentry->required != rxCLREQ_DONTWANT)
                    {
                        RwReqEntry *drivingreqe;

                        REQSEARCH4CLUSTER(drivingreqe, node->topSortData->req,
                                          reqentry->clusterDef);

                        if (drivingreqe != NULL
                            && (drivingreqe->outbf & (1U << j)) != 0U)
                        {
                            if (reqentry->st == NULL)
                            {
                                SCOPETRACEADDENTRY(drivingreqe->st, reqentry);

                                reqentry->st = drivingreqe->st;
                            }
                            else
                            {
                                /* _ScopeTraceMerge(stheadref, reqentry->st, drivingreqe->st): join the two traces' roots */
                                RwScopeTrace  *proot = reqentry->st, *qroot = drivingreqe->st;
                                RwScopeTrace **headref = stheadref;

                                while (proot->parent != NULL)
                                {
                                    proot = proot->parent;
                                }
                                while (qroot->parent != NULL)
                                {
                                    qroot = qroot->parent;
                                }

                                if (proot != qroot)
                                {
                                    RwScopeTrace       *prootfinalpage = proot;

                                    while (prootfinalpage->continuation != NULL)
                                    {
                                        prootfinalpage = prootfinalpage->continuation;
                                    }

                                    prootfinalpage->continuation = qroot;

                                    qroot->parent = proot;

                                    while (*headref != qroot)
                                    {
                                        headref = &(*headref)->next;
                                    }
                                    *headref = (*headref)->next;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    while (node++, --i);

    return (E_RX_DEP_SUCCESS);
}
