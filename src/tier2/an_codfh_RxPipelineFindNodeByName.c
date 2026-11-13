/* RxPipelineFindNodeByName (MFL.exe 004e7070). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwstrcmp RWSRCGLOBAL(stringFuncs).vecStrcmp
/* ---- end of the generated declarations ---- */

#define ISNODELIVE(_node) ( NULL != ( (_node)->nodeDef) )
/* TIER2: unvalidated authority=diffemu */
RxPipelineNode * RxPipelineFindNodeByName(RxPipeline * pipeline, const RwChar * name, RxPipelineNode * start, RwInt32 * nodeIndex)
{
    RwBool              check;

    check = (pipeline != NULL) && (name != NULL)
        && (pipeline->numNodes);

    if (check != 0)
    {
        RxPipelineNode     *node = &pipeline->nodes[0];
        RwInt32             n = pipeline->numNodes;

        if (start)
        {
            while ((node != start) && (n > 0))
            {
                ++node;
                --n;
            }
            ++node;
            --n;
        }

        while (n > 0)
        {
            if (ISNODELIVE(node))
            {
                if (!rwstrcmp(node->nodeDef->name, name))
                {
                    if (nodeIndex != NULL)
                        *nodeIndex = pipeline->numNodes - n;
                    return (node);
                }
            }
            ++node;
            --n;
        }
    }

    if (nodeIndex != NULL)
        *nodeIndex = -1;
    return ((RxPipelineNode *)NULL);
}
