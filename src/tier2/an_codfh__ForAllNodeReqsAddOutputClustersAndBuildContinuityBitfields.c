/* _ForAllNodeReqsAddOutputClustersAndBuildContinuityBitfields (MFL.exe 004f2770).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, _ReqSearch4Cluster / _ReqAddEntry / _IoSpecSearch4Cluster inlined by hand (MFL.exe has them inlined),
 * the function under its codfh name (as rxChaseDependencies calls it), release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
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
#define _ReqNumEntries(_req) ( (_req)->numused )
#define _ReqGetEntry(_req,_n) ( ( ((RwUInt32) (_n)) < (_req)->numused ) ? &(_req)->reqentries[(_n)] : (RwReqEntry *) NULL )
/* p2dep.c helpers MFL.exe inlines into this function (static or not in the source), written out as statement macros:
 * _ReqSearch4Cluster, _ReqAddEntry (MFL.exe 0x4f2590) with rxCLREQ_DONTWANT and one input, _IoSpecSearch4Cluster (MFL.exe 0x4f2620) */
#define REQSEARCH4CLUSTER(_res, _req, _clusterDef)                      \
MACRO_START                                                             \
{                                                                       \
    rxReq                *_sq = (_req);                                 \
    RxClusterDefinition  *_sc = (_clusterDef);                          \
    RwReqEntry           *_e = &(_sq->reqentries[0]);                   \
    RwUInt32              _n;                                           \
                                                                        \
    (_res) = (RwReqEntry *) NULL;                                       \
    for (_n = 0; _n < _sq->numused; ++_n)                               \
    {                                                                   \
        if (_e->clusterDef == _sc)                                      \
        {                                                               \
            (_res) = _e;                                                \
            break;                                                      \
        }                                                               \
        ++_e;                                                           \
    }                                                                   \
}                                                                       \
MACRO_STOP
/* the fields _ReqAddEntry sets on a new entry (rxCLREQ_DONTWANT, one input) */
#define REQENTRYINIT(_e, _req, _clusterDef, _node)                      \
MACRO_START                                                             \
{                                                                       \
    (_e)->clusterDef = (_clusterDef);                                   \
    (_e)->required = rxCLREQ_DONTWANT;                                  \
    (_e)->inputs = 1;                                                   \
    (_e)->next = (RwReqEntry *)NULL;                                    \
    (_e)->req = (_req);                                                 \
    (_e)->st = (RwScopeTrace *)NULL;                                    \
    (_e)->outbf = 0U;                                                   \
    (_e)->assignedslot = (RwUInt32) - 1;                                \
    (_e)->originatingNode = (_node);                                    \
}                                                                       \
MACRO_STOP
#define REQADDENTRYDONTWANT(_res, _req, _clusterDef, _node)             \
MACRO_START                                                             \
{                                                                       \
    rxReq                *_aq = (_req);                                 \
    RxClusterDefinition  *_ac = (_clusterDef);                          \
    RwReqEntry           *_ae = &(_aq->reqentries[0]);                  \
    RwUInt32              _an;                                          \
    RwBool                _found = FALSE;                               \
                                                                        \
    /* _ReqSearch4Cluster.  MFL.exe also tests a FOUND entry for NULL (0x4f2893 -> 0x4f2808); the entry was read during \
     * the search, so the test is taken before that read here (the NULL entry is read at address 0, as MFL.exe does). */ \
    (_res) = (RwReqEntry *)NULL;                                        \
    for (_an = 0; _an < _aq->numused; ++_an)                            \
    {                                                                   \
        if (_ae != NULL)                                                \
        {                                                               \
            if (_ae->clusterDef == _ac)                                 \
            {                                                           \
                (_res) = _ae;                                           \
                _found = TRUE;                                          \
                break;                                                  \
            }                                                           \
        }                                                               \
        else if (_ae->clusterDef == _ac)                                \
        {                                                               \
            _found = TRUE;      /* found, but NULL: out of memory */    \
            break;                                                      \
        }                                                               \
        ++_ae;                                                          \
    }                                                                   \
    if (!_found)                                                        \
    {                                                                   \
        /* The new entry is NULL when reqentries is NULL and numused 0.  MFL.exe fills it in anyway, then takes the       \
         * out-of-memory return (0x4f2808 / 0x4f293e).  cl 13 drops a NULL test that comes after a store through the        \
         * pointer, so the test comes first and both branches fill the entry. */                                           \
        (_res) = &_aq->reqentries[_aq->numused];                        \
        ++_aq->numused;                                                 \
        if ((_res) != NULL)                                             \
        {                                                               \
            REQENTRYINIT((_res), _aq, _ac, (_node));                    \
        }                                                               \
        else                                                            \
        {                                                               \
            REQENTRYINIT((_res), _aq, _ac, (_node));                    \
            (_res) = (RwReqEntry *)NULL;                                \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP
#define IOSPECSEARCH4CLUSTER(_res, _iospec, _clusterDef)                \
MACRO_START                                                             \
{                                                                       \
    for ((_res) = 0; (_res) < (_iospec)->numClustersOfInterest; ++(_res)) \
    {                                                                   \
        if ((_iospec)->clustersOfInterest[(_res)].clusterDef == (_clusterDef)) \
        {                                                               \
            break;                                                      \
        }                                                               \
    }                                                                   \
    if ((_res) == (_iospec)->numClustersOfInterest)                     \
    {                                                                   \
        (_res) = (RwUInt32) - 1;                                        \
    }                                                                   \
}                                                                       \
MACRO_STOP
#define E_RX_DEP_SUCCESS 0
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, outbf) == 0x18 && RW_OFFSETOF(RwReqEntry, originatingNode) == 0x20);      /* MFL.exe 0x4f27bb reqentries [edx+0xc], 0x4f27cc stride 0x24, 0x4f2951 outbf [eax+0x18], 0x4f2805 originatingNode [eax+0x20] */

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=static */
RwUInt32 _ForAllNodeReqsAddOutputClustersAndBuildContinuityBitfields(RxPipeline * pipeline)
{
    RwUInt32            i, j, k;
    RxPipelineNode     *node;

    i = pipeline->numNodes;
    node = &pipeline->nodes[0];

    do
    {
        RxIoSpec   *iospec = &node->nodeDef->io;

        for (j = 0; j < iospec->numClustersOfInterest; ++j)
        {
            if (iospec->clustersOfInterest[j].forcePresent)
            {
                RwReqEntry *added;

                REQADDENTRYDONTWANT(added, node->topSortData->req,
                                    iospec->clustersOfInterest[j].clusterDef, node);
                if (!added)
                {
                    RWERROR((E_RX_DEP_OUTOFMEMORY));
                    return (E_RX_DEP_OUTOFMEMORY);
                }
            }
        }

        for (j = 0; j < node->numOutputs; ++j)
        {
            if (node->outputs[j] != (RwUInt32) ~ 0)
            {
                RxPipelineNode     *outnode =
                    &pipeline->nodes[node->outputs[j]];
                RxOutputSpec       *outspec =
                    &node->nodeDef->io.outputs[j];

                for (k = 0; k < _ReqNumEntries(outnode->topSortData->req); ++k)
                {
                    RwReqEntry    *reqentry =
                        _ReqGetEntry(outnode->topSortData->req, k);
                    RwUInt32       n;
                    RxClusterValid presinout;

                    IOSPECSEARCH4CLUSTER(n, iospec, reqentry->clusterDef);
                    presinout = (n == (RwUInt32) - 1) ?
                        outspec->allOtherClusters :
                        outspec->outputClusters[n];

                    if (presinout != rxCLVALID_INVALID)
                    {
                        RwReqEntry         *drivingreqe;

                        if (presinout != rxCLVALID_NOCHANGE)
                        {
                            REQADDENTRYDONTWANT(drivingreqe, node->topSortData->req,
                                                reqentry->clusterDef, node);
                            if (drivingreqe == NULL)
                            {
                                RWERROR((E_RX_DEP_OUTOFMEMORY));
                                return (E_RX_DEP_OUTOFMEMORY);
                            }
                        }
                        else
                        {
                            REQSEARCH4CLUSTER(drivingreqe, node->topSortData->req,
                                              reqentry->clusterDef);
                        }

                        if (drivingreqe != NULL)
                        {
                            drivingreqe->outbf |= (1U << j);
                        }
                    }
                }
            }
        }
    }
    while (node++, --i);

    return (E_RX_DEP_SUCCESS);
}
