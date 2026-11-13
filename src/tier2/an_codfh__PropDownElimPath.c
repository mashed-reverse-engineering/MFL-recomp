/* _PropDownElimPath (MFL.exe 004f2650). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers _ReqSearch4Cluster / _ReqDeleteEntry and _IoSpecSearch4Cluster inlined by hand (MFL.exe has them inlined), the function
 * under its codfh name _PropDownElimPath (as the callers use it), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rxReq, reqentries) == 0xc && sizeof(RwReqEntry) == 0x24 && RW_OFFSETOF(RwReqEntry, inputs) == 8);      /* MFL.exe 0x4f2672 reqentries [ecx+0xc], 0x4f2687 stride 0x24, 0x4f2695 --inputs [edi+8], 0x4f26bd 9-dword entry copy */

extern void _PropDownElimPath(RxPipeline * pipeline, RxPipelineNode * node, RxClusterDefinition * clusterDef);      /* recursive: declared for the gate's shadow build */
/* TIER2: unvalidated authority=diffemu */
void _PropDownElimPath(RxPipeline * pipeline, RxPipelineNode * node, RxClusterDefinition * clusterDef)
{
    RwUInt32    i;
    RxIoSpec   *iospec = &node->nodeDef->io;
    rxReq      *req = node->topSortData->req;
    RwReqEntry *reqentry = (RwReqEntry *) NULL;

    /* _ReqSearch4Cluster(req, clusterDef), static in p2dep.c, inlined in MFL.exe */
    if (req->numused != 0)
    {
        RwReqEntry *entry = &(req->reqentries[0]);
        RwUInt32    n;

        for (n = 0;n < req->numused;++n)
        {
            if (entry->clusterDef == clusterDef)
            {
                reqentry = entry;
                break;
            }
            ++entry;
        }
    }

    if (reqentry == NULL)
    {
        return;
    }

    if (--reqentry->inputs != 0)
    {
        return;
    }

    /* _ReqDeleteEntry(req, reqentry), static, inlined: the last entry moves into the deleted one's place */
    req = node->topSortData->req;
    if (reqentry != &req->reqentries[req->numused - 1])
    {
        *reqentry = req->reqentries[req->numused - 1];
    }
    --req->numused;

    for (i = 0; i < node->numOutputs; ++i)
    {
        if (node->outputs[i] != (RwUInt32) ~ 0)
        {
            RxOutputSpec *outspec = &node->nodeDef->io.outputs[i];
            RwUInt32      n;
            RxClusterValid presinout;

            /* _IoSpecSearch4Cluster(iospec, clusterDef) (MFL.exe 0x4f2620), inlined here in MFL.exe */
            for (n = 0; n < iospec->numClustersOfInterest; ++n)
            {
                if (iospec->clustersOfInterest[n].clusterDef == clusterDef)
                {
                    break;
                }
            }
            if (n == iospec->numClustersOfInterest)
            {
                n = (RwUInt32) - 1;
            }

            presinout = (n == (RwUInt32) - 1) ?
                outspec->allOtherClusters : outspec->outputClusters[n];

            if (presinout == rxCLVALID_NOCHANGE)
            {
                _PropDownElimPath(pipeline,
                                  &pipeline->nodes[node->outputs[i]],
                                  clusterDef);
            }
        }
    }

    return;
}
