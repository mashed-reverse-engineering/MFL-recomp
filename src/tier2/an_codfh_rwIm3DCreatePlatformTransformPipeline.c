/* rwIm3DCreatePlatformTransformPipeline (MFL.exe 004f1a10).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/xbox/im3dpipe.c @e578672,
 * not byte-identical, changes: cut down to this
 * function and the private declarations it uses, the D3D9 instance node 0x4f13b0 in place of the Xbox one, release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RxPipeline *
RxPipelineCreate(void);
extern void
_rxPipelineDestroy(RxPipeline * Pipeline);
#define RxPipelineDestroy(_ppln) (_rxPipelineDestroy(_ppln), TRUE)
typedef RxPipeline RxLockedPipe;
extern RxLockedPipe *
RxPipelineLock(RxPipeline *pipeline);
extern RxPipeline *
RxLockedPipeUnlock(RxLockedPipe *pipeline);
extern RxLockedPipe *
RxLockedPipeAddFragment(RxLockedPipe *pipeline,
                        RwUInt32 *firstIndex,
                        RxNodeDefinition *nodeDef0,
                        ...);
extern RxPipeline *RwIm3DSetTransformPipeline(RxPipeline *pipeline);
/* ---- end of the generated declarations ---- */

/* D3D9 build: the instance node of the Im3D transform pipeline is FUN_004f13b0, which returns the node definition at 0x62eaf8 named
 * "ImmInstance.csl" (lane alias RxNodeDefinitionGetImmInstance); the codfh file is the Xbox one */
extern RxNodeDefinition *RxNodeDefinitionGetImmInstance(void);
#define RxNodeDefinitionGetXboxImmInstance RxNodeDefinitionGetImmInstance
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool rwIm3DCreatePlatformTransformPipeline(RxPipeline **globalPipe)
{
    RxPipeline  *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {
        RxPipeline  *lpipe;

        pipe->pluginId = rwID_COREPLUGIN;

        lpipe = RxPipelineLock(pipe);
        if (lpipe)
        {
            
            lpipe = RxLockedPipeAddFragment(lpipe,
                        NULL,
                        RxNodeDefinitionGetXboxImmInstance(),
                        NULL);  

            pipe = RxLockedPipeUnlock(lpipe);

            if (pipe != NULL)
            {
               *globalPipe = pipe;

                RwIm3DSetTransformPipeline(pipe);

                return (TRUE);
            }
        }

        RxPipelineDestroy(pipe);
    }

    return (FALSE);
}
