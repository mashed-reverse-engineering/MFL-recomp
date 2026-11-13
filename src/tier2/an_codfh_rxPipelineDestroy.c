/* rxPipelineDestroy (MFL.exe 004deff0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2core.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

extern RxPipelineNode *
PipelineNodeDestroy(RxPipelineNode *node, RxPipeline *pipeline);
extern RxPipelineNode * PipelineNodeDestroy(RxPipelineNode * node, RxPipeline *pipeline);
/* TIER2: unvalidated authority=diffemu */
void rxPipelineDestroy(RxPipeline * Pipeline)
{

    if (Pipeline)
    {

        RwUInt32            numNodes, i;
        RxPipelineNode     *Node = Pipeline->nodes;

        numNodes = Pipeline->numNodes;
        for (i = 0; i < numNodes; ++i)
        {
            
            PipelineNodeDestroy(Node, Pipeline);
            Node++;
        }
        Pipeline->nodes = (RxPipelineNode *)NULL;

        if (Pipeline->superBlock != NULL)
        {
            
            RwFree(Pipeline->superBlock);
            Pipeline->superBlock = (RwUInt32 *)NULL;
            Pipeline->superBlockSize = 0;
        }

        RwFreeListFree(RXPIPELINEGLOBAL(pipesFreeList), Pipeline);
        Pipeline = (RxPipeline *)NULL; 
    }

    return;
}
