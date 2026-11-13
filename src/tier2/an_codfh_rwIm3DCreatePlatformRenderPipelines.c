/* rwIm3DCreatePlatformRenderPipelines (MFL.exe 004f1b10). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/xbox/im3dpipe.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * the Xbox submit node replaced by the D3D9 one and the D3D9 tail call _rwD3D9Im3DRenderOpen added (MFL.exe is the D3D9 build), release
 * build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build,
 * the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then
 * hand-checked; compared with MFL.exe by differential emulation (tier 2). */
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
typedef struct rwIm3DRenderPipelines rwIm3DRenderPipelines;
struct rwIm3DRenderPipelines
{

    RxPipeline *triList;
    RxPipeline *triFan;
    RxPipeline *triStrip;
    RxPipeline *lineList;
    RxPipeline *polyLine;
    RxPipeline *pointList;
};
extern RxPipeline *RwIm3DSetRenderPipeline( RxPipeline *pipeline,
                                              RwPrimitiveType primType);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(rwIm3DRenderPipelines, polyLine) == 0x10);      /* MFL.exe 0x4f1af7..0x4f1b02 (rwIm3DDestroyPlatformRenderPipelines): [esi] .. [esi+0x10] cleared, triList first */
/* ---- end of the generated declarations ---- */

extern RxNodeDefinition *RxNodeDefinitionGetD3D9SubmitNoLight(void);      /* MFL.exe 0x4f13c0: returns the node definition at 0x62eb58, "nodeD3D9SubmitNoLight.csl" (the codfh source has the Xbox one) */
extern RwBool _rwD3D9Im3DRenderOpen(void);

/* TIER2: unvalidated authority=diffemu */
RwBool rwIm3DCreatePlatformRenderPipelines(rwIm3DRenderPipelines *globalPipes)
{
    RxPipeline  *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {
        RxLockedPipe    *lpipe;

        pipe->pluginId = rwID_COREPLUGIN;

        lpipe = RxPipelineLock(pipe);
        if (lpipe)
        {
            lpipe = RxLockedPipeAddFragment(lpipe,
                NULL,
                RxNodeDefinitionGetD3D9SubmitNoLight(),
                NULL);

            pipe = RxLockedPipeUnlock(lpipe);

            if (NULL != pipe)
            {
                globalPipes->triList  = pipe;
                globalPipes->triFan   = pipe;
                globalPipes->triStrip = pipe;
                globalPipes->lineList = pipe;
                globalPipes->polyLine = pipe;

                RwIm3DSetRenderPipeline(pipe, rwPRIMTYPETRILIST);
                RwIm3DSetRenderPipeline(pipe, rwPRIMTYPETRIFAN);
                RwIm3DSetRenderPipeline(pipe, rwPRIMTYPETRISTRIP);
                RwIm3DSetRenderPipeline(pipe, rwPRIMTYPELINELIST);
                RwIm3DSetRenderPipeline(pipe, rwPRIMTYPEPOLYLINE);

                /* D3D9 build (MFL.exe 0x4f1b95): the result is the D3D9 Im3D render open */
                return (_rwD3D9Im3DRenderOpen());
            }
        }

        RxPipelineDestroy(pipe);
    }

    return (FALSE);
}
