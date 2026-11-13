/* rxEmbeddedPacketBetweenNodes (MFL.exe 004f1cd0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2altmdl.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern void RxHeapFree(RxHeap * heap, void *block);
#define rxCLFLAGS_EXTERNAL ((RwUInt16) 0x0002U)
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;
    RwUInt32 creationAttributes;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineCluster) == 8);      /* MFL.exe 0x4f3090 (MyEnumPipelineClustersCallBack): push 0x8 for StalacMiteAlloc */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RxPipelineCluster, creationAttributes) == 4);      /* MFL.exe 0x4f30ae (MyEnumPipelineClustersCallBack): mov [eax+0x4],ecx */
/* ---- end of the generated declarations ---- */

#define CLUSTERZERO(cl) MACRO_START { RWASSERT( offsetof(RxCluster, data) == sizeof(RwUInt32) ); *(RwUInt32 *) &(cl)->flags = 0U; (cl)->data = NULL; (cl)->numAlloced = 0U; (cl)->numUsed = 0U; (cl)->clusterRef = (RxPipelineCluster *)NULL; } MACRO_STOP
#define CLUSTERTERMINATE(cl) MACRO_START { if ( (cl)->clusterRef != (RxPipelineCluster *)NULL ) { if ( (cl)->data != NULL && !((cl)->flags & rxCLFLAGS_EXTERNAL) ) { RxHeapFree(_rxHeapGlobal, (cl)->data); } CLUSTERZERO(cl); } } MACRO_STOP
#define EMBEDDEDPACKETBETWEENNODESMACRO(result,pipeline,nodeFrom,Index) MACRO_START { RwUInt32 outputIndex = nodeFrom->outputs[Index]; if (outputIndex != (RwUInt32) - 1) { RxPacket *packet; RxPipelineNode *nodeTo; RwUInt32 outputMask; nodeTo = &pipeline->nodes[outputIndex]; if (pipeline->embeddedPacketState > rxPKST_UNUSED) { outputMask = 1U << Index; packet = pipeline->embeddedPacket; if (!(packet->slotsContinue[0] & outputMask)) { RwUInt32 n = packet->numClusters; do { if (!(packet->slotsContinue[n] & outputMask)) { CLUSTERTERMINATE(&packet->clusters[n - 1]); } } while (--n); } packet->inputToClusterSlot = nodeTo->inputToClusterSlot; packet->slotsContinue = nodeTo->slotsContinue; packet->slotClusterRefs = nodeTo->slotClusterRefs; pipeline->embeddedPacketState = rxPKST_PENDING; } result = nodeTo; } else { result = (RxPipelineNode *)NULL; } } MACRO_STOP
extern RxHeap      *_rxHeapGlobal;
/* TIER2: unvalidated authority=diffemu */
RxPipelineNode * rxEmbeddedPacketBetweenNodes(RxPipeline * pipeline, RxPipelineNode * nodeFrom, RwUInt32 whichOutput)
{
    RxPipelineNode *result;

    EMBEDDEDPACKETBETWEENNODESMACRO(result, pipeline, nodeFrom, whichOutput);

    return (result);
}
