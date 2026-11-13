/* PipelineNodeDestroy (MFL.exe 004deca0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2core.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define RXNODEMAXOUTPUTS 32
typedef struct rxReq rxReq;
struct RxPipelineNodeTopSortData
{
    RwUInt32 numIns;
    RwUInt32 numInsVisited;

    rxReq *req;

};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RxPipelineNodeTopSortData) == 12);      /* MFL.exe 0x4e6d34-0x4e6d47 (LockPipelineExpandData): three words cleared, add eax,0xc per node */
/* ---- end of the generated declarations ---- */

extern RxPipelineNode *
PipelineNodeDestroy(RxPipelineNode *node, RxPipeline *pipeline);
/* TIER2: unvalidated authority=diffemu */
RxPipelineNode * PipelineNodeDestroy(RxPipelineNode * node, RxPipeline *pipeline)
{

    if (FALSE == pipeline->locked)
    {

        if (node->nodeDef->nodeMethods.pipelineNodeTerm != NULL)
        {
            node->nodeDef->nodeMethods.pipelineNodeTerm(node);
        }

        node->nodeDef->InputPipesCnt--;
        
        if (0 == node->nodeDef->InputPipesCnt)
        {

            if (node->nodeDef->nodeMethods.nodeTerm != NULL)
            {
                node->nodeDef->nodeMethods.nodeTerm(node->nodeDef);
            }
            if (node->nodeDef->editable)
            {
                
                RwFree(node->nodeDef);
                node->nodeDef = (RxNodeDefinition *)NULL;
            }
        }

        if (node->initializationData != NULL)
        {
            
            RwFree(node->initializationData);
            node->initializationData = NULL;
            node->initializationDataSize = 0;
        }

        memset(node, 0, sizeof(RxPipelineNode));
    }
    else
    {
        RwInt32 nodeIndex;

        if (node->initializationData != NULL)
        {
            
            RwFree(node->initializationData);
            node->initializationData = NULL;
            node->initializationDataSize = 0;
        }

        if (0 == node->nodeDef->InputPipesCnt)
        {

            if (node->nodeDef->editable)
            {
                
                RwFree(node->nodeDef);
                node->nodeDef = (RxNodeDefinition *)NULL;
            }
        }

        nodeIndex = node - &(pipeline->nodes[0]);
        
        if ((RwUInt32)nodeIndex < (pipeline->numNodes - 1))
        {
            
            RxPipelineNodeTopSortData *topSortData, *nextTopSortData;
            RwUInt32 *output, *nextOutput;
            RwUInt32 i, j;

            output = (RwUInt32 *)
                &(pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            output += RXNODEMAXOUTPUTS*nodeIndex;
            nextOutput = output + RXNODEMAXOUTPUTS;
            for (i = nodeIndex;i < (pipeline->numNodes - 1);++i)
            {
                memcpy(output, nextOutput, sizeof(RwUInt32)*RXNODEMAXOUTPUTS);
                output = nextOutput;
                nextOutput += RXNODEMAXOUTPUTS;
            }

            output = (RwUInt32 *)
                &(pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            topSortData = (RxPipelineNodeTopSortData *)
                &(output[RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            nextTopSortData = topSortData + 1;
            for (i = nodeIndex;i < (pipeline->numNodes - 1);++i)
            {
                memcpy(topSortData,
                       nextTopSortData,
                       sizeof(RxPipelineNodeTopSortData));
                topSortData = nextTopSortData;
                ++nextTopSortData;
            }

            for (i = nodeIndex;i < (pipeline->numNodes - 1);++i)
            {
                memcpy(&(pipeline->nodes[i]),
                       &(pipeline->nodes[i + 1]),
                       sizeof(RxPipelineNode));
                pipeline->nodes[i].outputs -= RXNODEMAXOUTPUTS;
                --pipeline->nodes[i].topSortData;
            }

            for (i = 0;i < (pipeline->numNodes - 1);++i)
            {
                for (j = 0;j < pipeline->nodes[i].numOutputs;++j)
                {
                    if (pipeline->nodes[i].outputs[j] >= (RwUInt32)nodeIndex)
                    {
                        if (pipeline->nodes[i].outputs[j] == (RwUInt32)nodeIndex)
                        {
                            
                            pipeline->nodes[i].outputs[j] = (RwUInt32)-1;
                        }
                        else
                        {
                            
                            --pipeline->nodes[i].outputs[j];
                        }
                    }

                }
            }
        }
        else
        {
            
        }
    }

    --pipeline->numNodes;

    return (node);
}
