/* RxLockedPipeAddFragment (MFL.exe 004e7100). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(ap,v) (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap,t) (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap) (ap = (va_list)0)
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
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_COREPLUGIN
extern RxPipelineNode *
PipelineNodeDestroy(RxPipelineNode *node, RxPipeline *pipeline);
#define ISNODELIVE(_node) ( NULL != ( (_node)->nodeDef) )
extern RwBool _NodeCreate(RxPipeline * pipeline, RxPipelineNode * node, RxNodeDefinition * nodespec);

/* RxLockedPipeAddPath(pipeline, RxPipelineNodeFindOutputByIndex(prev, 0), RxPipelineNodeFindInput(node)) of the SDK source, inlined as
 * MFL.exe has it (0x4e71a1-0x4e721d; the tier 2 build is /Ob0, so a static helper would stay a call): links output 0 of prev to node.
 * Returns FALSE when the path cannot be added. */
#define LinkNodeToPrevious(_pipeline, _prev, _node, _ok)                                         \
do {                                                                                              \
    RxPipelineNode *_in = ((_node) != NULL && ISNODELIVE(_node)) ? (_node) : (RxPipelineNode *)NULL; \
    RwUInt32 *_out = (ISNODELIVE(_prev) && 0 < (_prev)->numOutputs) ? &(_prev)->outputs[0] : (RwUInt32 *)NULL; \
    (_ok) = FALSE;                                                                                \
    if ((FALSE != (_pipeline)->locked) && (NULL != _out) && ((RwUInt32)-1 == *_out) &&           \
        (NULL != _in) && ISNODELIVE(_in))                                                         \
    {                                                                                             \
        RwUInt32 _index = _in - (_pipeline)->nodes;  /* PipelineNode2Index */                    \
        if ((&((_pipeline)->nodes[_index]) == _in) && (_index < (_pipeline)->numNodes) &&        \
            (_index != (RwUInt32)-1))                                                             \
        {                                                                                         \
            *_out = _index;                                                                       \
            (_ok) = TRUE;                                                                         \
        }                                                                                         \
    }                                                                                             \
} while (0)

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=static */
RxPipeline * RxLockedPipeAddFragment(RxPipeline *pipeline, RwUInt32 *firstIndex, RxNodeDefinition *nodeDef0, ...)
{

    if (pipeline != NULL && pipeline->locked != FALSE)
    {
        va_list             va;
        RxNodeDefinition   *nodeDef;
        RwUInt32            fragLength = 0;

        va_start(va, nodeDef0);
        for (nodeDef = nodeDef0; nodeDef != NULL;
             nodeDef = va_arg(va, RxNodeDefinition *))
        {
            ++fragLength;
        }
        va_end(va);

        if (fragLength != 0)
        {
            RwUInt32        oldnumnodes, n;
            RxPipelineNode *prevnode = (RxPipelineNode *)NULL;

            if ((pipeline->numNodes + fragLength) > RXPIPELINEGLOBAL(maxNodesPerPipe))
            {
                RWERROR((E_RX_PIPELINETOOMANYNODES));
                return ((RxPipeline *)NULL);
            }

            oldnumnodes = pipeline->numNodes;

            n = 0;
            va_start(va, nodeDef0);
            for (nodeDef = nodeDef0; nodeDef != NULL;
                 nodeDef = va_arg(va, RxNodeDefinition *))
            {
                RxPipelineNode *node = &pipeline->nodes[oldnumnodes + n];

                if (!_NodeCreate(pipeline, node, nodeDef))
                {
                    break;
                }
                if (prevnode != NULL)
                {
                    RwBool linked;

                    LinkNodeToPrevious(pipeline, prevnode, node, linked);
                    if (!linked)
                    {
                        PipelineNodeDestroy(node, pipeline);

                        break;
                    }
                }

                prevnode = node;

                ++n;
            }
            va_end(va);

            if (n == fragLength)
            {
                if (firstIndex != 0)
                {
                    *firstIndex = oldnumnodes;
                }
                return (pipeline);
            }

            /* "Failed to create and link all nodes in new fragment": the nodes made so far go again */
            while (n--)
            {
                PipelineNodeDestroy(
                    &(pipeline->nodes[(oldnumnodes - 1) + (n + 1)]), pipeline);
            }
        }
        /* else "No nodes specified to add!" (a debug message only) */
    }
    else
    {
        if (pipeline == NULL)
        {
            RWERROR((E_RW_NULLP));
        }
        else
        {
            RWERROR((E_RX_UNLOCKEDPIPE));
        }
    }

    return ((RxPipeline *)NULL);
}
