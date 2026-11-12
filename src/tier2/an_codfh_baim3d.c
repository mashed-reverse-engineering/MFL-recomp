/* baim3d module (MFL.exe 0x004dd0c0 / 0x004dd100): RenderWare Graphics 3.7 (Criterion Software), Im3D module open / close: the
 * platform transform / render pipelines (rwIm3DOpen / rwIm3DClose = codfh _rwIm3DOpen / _rwIm3DClose; exe names).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/baim3d.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build, no RwDebugSendMessage), RWIMMEDIGLOBAL /
 * RWPLUGINOFFSET expanded to the byte offset into *RwEngineInstance, memset as a word loop over the 0x74-byte globals, the
 * _rwIm3DClose call on failure written out as the exe inlined it, rwImmediGlobals declared here only up to the two platform
 * pipeline fields it touches (codfh baim3d.h layout, size 0x74) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct rwIm3DRenderPipelines {
    void *triList;
    void *triFan;
    void *triStrip;
    void *lineList;
    void *polyLine;
    void *pointList;
} rwIm3DRenderPipelines;
typedef struct rwImmediGlobals {
    void *im3DTransformPipeline;        /* 0x00 */
    rwIm3DRenderPipelines im3DRenderPipelines;      /* 0x04 */
    void *platformIm3DTransformPipeline;      /* 0x1c */
    rwIm3DRenderPipelines platformIm3DRenderPipelines;      /* 0x20 */
    void *pool[15];                     /* 0x38 curPool and the vertex pool state */
} rwImmediGlobals;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
RW_LAYOUT_ASSERT(rw_assert_im3d_platform_transform, RW_OFFSETOF(rwImmediGlobals, platformIm3DTransformPipeline) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_im3d_platform_render, RW_OFFSETOF(rwImmediGlobals, platformIm3DRenderPipelines) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_im3d_pointlist, RW_OFFSETOF(rwImmediGlobals, im3DRenderPipelines.pointList) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_im3d_size, sizeof(rwImmediGlobals) == 0x74);
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

/*007d2128*/ extern void *RwEngineInstance;
/*00947360*/ extern RwModuleInfo _rwIm3DModule;
/*007d26fc*/ extern rwImmediGlobals *_rwIm3DGlobals;
/*004f1a10*/ extern RwBool _rwIm3DCreatePlatformTransformPipeline(void *pipeline);
/*004f1a80*/ extern void _rwIm3DDestroyPlatformTransformPipeline(void *pipeline);
/*004f1b10*/ extern RwBool _rwIm3DCreatePlatformRenderPipelines(void *pipelines);
/*004f1ab0*/ extern void _rwIm3DDestroyPlatformRenderPipelines(void *pipelines);
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
/* RwPrimitiveType (rwPRIMTYPE*: LINELIST 1, POLYLINE 2, TRILIST 3, TRISTRIP 4, TRIFAN 5, POINTLIST 6) comes from mfl_tier2.h */
#define rwID_COREPLUGIN 0x01
#define E_RX_INVALIDPRIMTYPE 0x25
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
#define RWIMMEDIGLOBAL(var) (((rwImmediGlobals *)((RwUInt8 *)RwEngineInstance + _rwIm3DModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwIm3DClose(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Clean up target-specific RwIm3D pipelines */
    _rwIm3DDestroyPlatformRenderPipelines(&RWIMMEDIGLOBAL(platformIm3DRenderPipelines));
    _rwIm3DDestroyPlatformTransformPipeline(&RWIMMEDIGLOBAL(platformIm3DTransformPipeline));

    --_rwIm3DModule.numInstances;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwIm3DOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    RwUInt32 *word;
    RwInt32 i;

    /* Cache the globals offset (same for all instances) */
    _rwIm3DModule.globalsOffset = offset;

    /* One more module instance */
    ++_rwIm3DModule.numInstances;

    /* Temporary, for debugging */
    _rwIm3DGlobals = (rwImmediGlobals *)&RWIMMEDIGLOBAL(im3DTransformPipeline);

    /* Pipes and curPool initialised to NULL/0 */
    word = (RwUInt32 *)_rwIm3DGlobals;
    for (i = 0; i < (RwInt32)(sizeof(rwImmediGlobals) / sizeof(RwUInt32)); i++) word[i] = 0;

    /* Allow creation of platform-specific pipes to override the generic ones as defaults */
    if (_rwIm3DCreatePlatformTransformPipeline(&RWIMMEDIGLOBAL(platformIm3DTransformPipeline)) &&
        _rwIm3DCreatePlatformRenderPipelines(&RWIMMEDIGLOBAL(platformIm3DRenderPipelines)))
    {
        return instance;
    }

    /* This should clean up safely (_rwIm3DClose) */
    _rwIm3DDestroyPlatformRenderPipelines(&RWIMMEDIGLOBAL(platformIm3DRenderPipelines));
    _rwIm3DDestroyPlatformTransformPipeline(&RWIMMEDIGLOBAL(platformIm3DTransformPipeline));
    --_rwIm3DModule.numInstances;

    /* Oops... */
    return NULL;
}

/* the default pipeline of a primitive type: the platform one (or NULL) */
#define SET_DEFAULT(slot) \
    { \
        if (RWIMMEDIGLOBAL(platformIm3DRenderPipelines).slot != NULL) \
            RWIMMEDIGLOBAL(im3DRenderPipelines).slot = RWIMMEDIGLOBAL(platformIm3DRenderPipelines).slot; \
        else \
            RWIMMEDIGLOBAL(im3DRenderPipelines).slot = NULL; \
        return RWIMMEDIGLOBAL(im3DRenderPipelines).slot; \
    }

/* TIER2: validated 2026-11-10 authority=diffemu */
void *RwIm3DSetRenderPipeline(void *pipeline, RwInt32 primType)
{
    RwError error;

    if (pipeline != NULL)
    {
        /* Set the appropriate RwIm3D pipeline */
        switch (primType)
        {
            case rwPRIMTYPETRILIST: RWIMMEDIGLOBAL(im3DRenderPipelines).triList = pipeline; return pipeline;
            case rwPRIMTYPETRIFAN: RWIMMEDIGLOBAL(im3DRenderPipelines).triFan = pipeline; return pipeline;
            case rwPRIMTYPETRISTRIP: RWIMMEDIGLOBAL(im3DRenderPipelines).triStrip = pipeline; return pipeline;
            case rwPRIMTYPELINELIST: RWIMMEDIGLOBAL(im3DRenderPipelines).lineList = pipeline; return pipeline;
            case rwPRIMTYPEPOLYLINE: RWIMMEDIGLOBAL(im3DRenderPipelines).polyLine = pipeline; return pipeline;
            case rwPRIMTYPEPOINTLIST: RWIMMEDIGLOBAL(im3DRenderPipelines).pointList = pipeline; return pipeline;
            default: break;
        }
    }
    else
    {
        /* Set the appropriate RwIm3D pipeline to a default pipeline */
        switch (primType)
        {
            case rwPRIMTYPETRILIST: SET_DEFAULT(triList);
            case rwPRIMTYPETRIFAN: SET_DEFAULT(triFan);
            case rwPRIMTYPETRISTRIP: SET_DEFAULT(triStrip);
            case rwPRIMTYPELINELIST: SET_DEFAULT(lineList);
            case rwPRIMTYPEPOLYLINE: SET_DEFAULT(polyLine);
            case rwPRIMTYPEPOINTLIST: SET_DEFAULT(pointList);
            default: break;
        }
    }
    error.pluginID = rwID_COREPLUGIN;
    error.errorCode = _rwerror(E_RX_INVALIDPRIMTYPE, primType);
    RwErrorSet(&error);
    return NULL;
}
