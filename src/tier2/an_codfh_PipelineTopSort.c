/* PipelineTopSort (MFL.exe 004e6d60). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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

typedef struct tagTopSortData TopSortData;
struct tagTopSortData
{
    RxPipeline         *pipeline;
    RwUInt32            nodesArraySlot;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(TopSortData, nodesArraySlot) == 4 && sizeof(struct tagTopSortData) == 8);      /* MFL.exe 0x4e6d6f data->nodesArraySlot [ebx+4], 0x4e6d7e data->pipeline [ebx] */
extern void PipelineTopSort(TopSortData *data, RwUInt32 nodeIndex);      /* recursive: declared for the gate's shadow build (shadowgen2 renames the definition) */
/* TIER2: unvalidated authority=diffemu */
void PipelineTopSort(TopSortData *data, RwUInt32 nodeIndex)
{
    RxPipelineNode *curNode;
    RwUInt32 i = data->nodesArraySlot;
    RwUInt32 j = nodeIndex;

    if (i != j)
    {
        RwUInt32                   tmpOutput;
        RwUInt32                  *outputsI,     *outputsJ;
        RxPipelineNodeTopSortData  tempTopSortData;
        RxPipelineNodeTopSortData *topSortDataI, *topSortDataJ;
        RxPipelineNode             tmpNode;
        RwUInt32                   k, l;

        outputsI = data->pipeline->nodes[i].outputs;
        outputsJ = data->pipeline->nodes[j].outputs;
        for (k = 0;k < RXNODEMAXOUTPUTS;k++)
        {
            tmpOutput   = outputsI[k];
            outputsI[k] = outputsJ[k];
            outputsJ[k] = tmpOutput;
        }
        
        data->pipeline->nodes[i].outputs = outputsJ;
        data->pipeline->nodes[j].outputs = outputsI;

        topSortDataI = data->pipeline->nodes[i].topSortData;
        topSortDataJ = data->pipeline->nodes[j].topSortData;
        tempTopSortData = *topSortDataI;
       *topSortDataI    = *topSortDataJ;
       *topSortDataJ    =  tempTopSortData;
        
        data->pipeline->nodes[i].topSortData = topSortDataJ;
        data->pipeline->nodes[j].topSortData = topSortDataI;

        tmpNode = data->pipeline->nodes[i];
        data->pipeline->nodes[i] = data->pipeline->nodes[j];
        data->pipeline->nodes[j] = tmpNode;
        
        for (k = 0;k < data->pipeline->numNodes;++k)
        {
            RxPipelineNode *node = &(data->pipeline->nodes[k]);
            for (l = 0;l < node->numOutputs;++l)
            {
                if (node->outputs[l] == i)
                {
                    node->outputs[l] = j;
                }
                else
                if (node->outputs[l] == j)
                {
                    node->outputs[l] = i;
                }
            }
        }
    }

    curNode = &(data->pipeline->nodes[data->nodesArraySlot]);
    ++data->nodesArraySlot;

    if (curNode->numOutputs != 0)
    {
        for (i = 0;i < curNode->numOutputs;++i)
        {
            RwUInt32 outIndex = curNode->outputs[i];

            if (outIndex != (RwUInt32)-1)
            {
                RxPipelineNode *outNode = &(data->pipeline->nodes[outIndex]);

                ++outNode->topSortData->numInsVisited;
                if (outNode->topSortData->numIns ==
                    outNode->topSortData->numInsVisited)
                {
                    PipelineTopSort(data, outIndex);
                }
            }
        }
    }

    return;
}
