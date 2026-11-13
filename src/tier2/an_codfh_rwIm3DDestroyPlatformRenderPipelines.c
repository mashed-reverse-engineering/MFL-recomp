/* rwIm3DDestroyPlatformRenderPipelines (MFL.exe 004f1ab0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/xbox/im3dpipe.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * the call of the D3D9 driver's _rwD3D9Im3DRenderClose added in front (MFL.exe's D3D9 build; the codfh file is the Xbox one), release build
 * macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern void
_rxPipelineDestroy(RxPipeline * Pipeline);
#define RxPipelineDestroy(_ppln) (_rxPipelineDestroy(_ppln), TRUE)
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

/* the D3D9 driver's Im3D close (MFL.exe 0x4f09d0): the codfh source of this function is the Xbox im3dpipe.c, the D3D9 one of MFL.exe
 * calls it first */
extern void _rwD3D9Im3DRenderClose(void);
/* TIER2: unvalidated authority=diffemu */
void rwIm3DDestroyPlatformRenderPipelines(rwIm3DRenderPipelines *globalPipes)
{
    _rwD3D9Im3DRenderClose();

    RwIm3DSetRenderPipeline(NULL, rwPRIMTYPETRILIST);
    RwIm3DSetRenderPipeline(NULL, rwPRIMTYPETRIFAN);
    RwIm3DSetRenderPipeline(NULL, rwPRIMTYPETRISTRIP);
    RwIm3DSetRenderPipeline(NULL, rwPRIMTYPELINELIST);
    RwIm3DSetRenderPipeline(NULL, rwPRIMTYPEPOLYLINE);

    if (NULL != globalPipes->triList)
    {
        RxPipelineDestroy(globalPipes->triList);
    }

    globalPipes->triList  = NULL;
    globalPipes->triFan   = NULL;
    globalPipes->triStrip = NULL;
    globalPipes->lineList = NULL;
    globalPipes->polyLine = NULL;

    return;
}
