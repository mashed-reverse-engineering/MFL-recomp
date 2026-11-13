/* PipelineCalcNumUniqueClusters (MFL.exe 004e6480). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
struct RxPipelineRequiresCluster
{
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq rqdOrOpt;
    RwUInt32 slotIndex;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineRequiresCluster) == 0xc);      /* MFL.exe 0x4e65d1..0x4e65df (RxLockedPipeUnlock): (numNodes + 5) * numClusters * 4 = numClusters * (sizeof(RxPipelineCluster) 8 + 0xc) + numNodes * numClusters * 4 */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RxClusterRef) == 0xc);      /* MFL.exe 0x4e64b1..0x4e64c0 (PipelineCalcNumUniqueClusters): clustersOfInterest [ecx+0x24], clusterDef [edx], add edx,0xc per entry */
/* ---- end of the generated declarations ---- */

extern RwUInt32
PipelineCalcNumUniqueClusters(RxPipeline *pipe);
/* TIER2: unvalidated authority=diffemu */
RwUInt32 PipelineCalcNumUniqueClusters(RxPipeline *pipeline)
{
    RxClusterDefinition *lastAddress, *newAddress;
    RwUInt32             numUniqueClusters;
    RwUInt32             i, j;

    numUniqueClusters = 0;
    newAddress = (RxClusterDefinition *)NULL;
    while (1)
    {
        lastAddress = newAddress;
        newAddress  = (RxClusterDefinition *)0xFFFFFFFF;

        for (i = 0;i < pipeline->numNodes;++i)
        {
            RxNodeDefinition *nodeDef = pipeline->nodes[i].nodeDef;
            for (j = 0;j < nodeDef->io.numClustersOfInterest;++j)
            {
                RxClusterDefinition *address;
                address = nodeDef->io.clustersOfInterest[j].clusterDef;
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

    return (numUniqueClusters);
}
