/* MyEnumPipelineClustersCallBack (MFL.exe 004f3090). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2dep.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;
    RwUInt32 creationAttributes;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineCluster) == 8);      /* MFL.exe 0x4f3090 (MyEnumPipelineClustersCallBack): push 0x8 for StalacMiteAlloc */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RxPipelineCluster, creationAttributes) == 4);      /* MFL.exe 0x4f30ae (MyEnumPipelineClustersCallBack): mov [eax+0x4],ecx */
/* ---- end of the generated declarations ---- */

extern void *
StalacMiteAlloc(RwUInt32 size);
/* TIER2: unvalidated authority=diffemu */
void MyEnumPipelineClustersCallBack(RxClusterDefinition *clusterdef, RwUInt32 __RWUNUSED__ pipelineclusterindex, void *data)
{
    RxPipelineCluster **array = (RxPipelineCluster **)data;
    RxPipelineCluster  *pipelineCluster;

    pipelineCluster = (RxPipelineCluster *)
        StalacMiteAlloc(sizeof(RxPipelineCluster));

    if (NULL == *array) *array = pipelineCluster;

    pipelineCluster->clusterRef = clusterdef;
    pipelineCluster->creationAttributes = clusterdef->defaultAttributes;

    return;
}
