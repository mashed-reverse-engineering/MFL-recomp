/* rt2dGetTolerance (MFL.exe 00562e60). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/gstate.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses, the function named _rt2dGetTolerance as MFL.exe's symbol, the
 * static helper _rt2dCTMGetInverse written inline (MFL.exe inlines it). Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwV3d *RwV3dTransformVector(RwV3d * vectorOut,
                                   const RwV3d * vectorIn,
                                   const RwMatrix * matrix);
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
typedef struct RwTexDictionary RwTexDictionary;
#define VERTEXCACHESIZE 256
typedef struct Rt2dBrush Rt2dBrush;
typedef struct Rt2dPath Rt2dPath;
typedef struct Rt2dFont Rt2dFont;
typedef struct _rt2dFontDictionaryNode _rt2dFontDictionaryNode;
typedef struct Rt2dBBox Rt2dBBox;
struct Rt2dBBox
{
    RwReal x;
    RwReal y;
    RwReal w;
    RwReal h;
};
typedef Rt2dFont*(* Rt2dFontCallBackRead)(const RwChar *name);
extern RwMatrix *
_rt2dCTMGet(void);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
/* ---- end of the generated declarations ---- */

typedef struct rt2dPathNode rt2dPathNode;
typedef unsigned char Rt2dFontChar;
typedef struct _rt2dTriIndex _rt2dTriIndex;
struct _rt2dTriIndex
{
    RwInt32             currIdx;
    RwInt32             *idx;
};
#define MAXVINDEX (VERTEXCACHESIZE/4 * 6)
#define RT2DMAXCOLOR 64
#define RT2DMAXCTM 32
#define RT2DSTRBUFFER 1024
typedef struct Rt2dStrBuffer Rt2dStrBuffer;
struct Rt2dStrBuffer
{
    RwInt32             space;
    Rt2dFontChar       *str;
    Rt2dFontChar        buffer[RT2DSTRBUFFER];
};
typedef struct Rt2dGlobalVars Rt2dGlobalVars;
struct Rt2dGlobalVars
{
    RwImVertexIndex     topo[VERTEXCACHESIZE];
    RwImVertexIndex     fonttopo[MAXVINDEX];

    RwFreeList         *brushFreeList;
    RwIm3DVertex       *brushVertexCache;

    RwFreeList         *fontFreeList;
    RwChar             *fontPath;
    RwUInt32           fontPathSize;
    RwFreeList         *fontDictNodeFreeList;
    _rt2dFontDictionaryNode *fontDictionary;
    RwTexDictionary    *fontTexDictionary;
    RwTexDictionary    *currFontTexDictionary;

    RwFreeList         *pathFreeList;
    Rt2dPath           *scratch;

    RwFreeList         *objectFreeList;

    RwFreeList         *fillFreeList;

    RwFreeList         *triVertFreeList;
    RwFreeList         *triPolyFreeList;
    _rt2dTriIndex       triVertIndex;

    RwMatrix            metricMtx;
    RwMatrix           *ctm[RT2DMAXCTM];
    RwInt32             mtos;

    RwV2d               vw;
    RwV2d               vp;

    RwMatrix            CTM2d, iCTM2d;
    RwMatrix            CTM;
    RwBool              CTMValid;
    RwUInt32            TransformFlags;

    RwReal              layerDepth;
    RwReal              tolerance;
    RwReal              sqTolerance;
    RwBool              sqToleranceValid;
    RwInt32             flatDepth;

    RwInt32             extensionOffset;

    RwReal              deviceX;
    RwReal              deviceY;
    RwReal              deviceW;
    RwReal              deviceH;

    RwBool              pixelSpace;

    RwCamera           *viewCamera;

    Rt2dBrush          *brush;
    Rt2dFont           *font;
    Rt2dPath           *path;
    Rt2dFontChar       *str;

    rt2dPathNode       *pnode0;
    rt2dPathNode       *pnode1;

    RwReal              pathInset;
    RwReal              oobaseu;
    RwReal              fontbaseu;

    Rt2dBBox            bbox;
    RwV2d               delta;

    RwInt32             numVerts, numBatches;

    RwCameraBeginUpdateFunc cameraBeginUpdate;
    RwCameraEndUpdateFunc cameraEndUpdate;

    RxClusterDefinition RxClPS2FillPos;
    RxClusterDefinition RxClPS2FillNrm;

    RxClusterDefinition RxClPS2FontUV1;
    RxClusterDefinition RxClPS2FontUV2;

    RxClusterDefinition RxClPS2StrokePos;
    RxClusterDefinition RxClPS2StrokeNrm;

    RxPipeline         *default_pipe, *use_pipe;

    RxPipeline         *default_stroke_pipe, *use_stroke_pipe;
    RxPipeline         *default_font_pipe, *use_font_pipe;
    RxPipeline         *default_fill_pipe, *use_fill_pipe;
    RxPipeline         *default_sgfont_pipe, *use_sgfont_pipe;

    RwInt32             fill_batch_sz, font_batch_sz, stroke_batch_sz,
                        sgfont_batch_sz;

    Rt2dStrBuffer       strBuffer;

    Rt2dFont           *fontBatch;

    Rt2dFontCallBackRead
                        fontReadFunc;

    /* no `unknown` word in MFL.exe, see agents/prelude_w5/private_patches.py */

    RwRGBA              colorCache[RT2DMAXCOLOR];
    RwUInt32           *triIdxCache;
    RwUInt32            triIdxCacheSize;

    RwChar              filename[80];
    RwInt32            *uCharIndex;
    RwInt32             uCharIndexSize;
};
extern Rt2dGlobalVars Rt2dGlobals;
RW_LAYOUT_ASSERT(rw37_priv_6, RW_OFFSETOF(Rt2dGlobalVars, fonttopo) == 0x200);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_7, RW_OFFSETOF(Rt2dGlobalVars, fontFreeList) == 0x508);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_8, RW_OFFSETOF(Rt2dGlobalVars, fontPathSize) == 0x510);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_9, RW_OFFSETOF(Rt2dGlobalVars, fontDictionary) == 0x518);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_10, RW_OFFSETOF(Rt2dGlobalVars, currFontTexDictionary) == 0x520);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_11, RW_OFFSETOF(Rt2dGlobalVars, brushVertexCache) == 0x504);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_12, RW_OFFSETOF(Rt2dGlobalVars, layerDepth) == 0x6e0);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_13, RW_OFFSETOF(Rt2dGlobalVars, fontReadFunc) == 0xc00);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_14, RW_OFFSETOF(Rt2dGlobalVars, uCharIndex) == 0xd5c);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_15, RW_OFFSETOF(Rt2dGlobalVars, uCharIndexSize) == 0xd60);      /* an_codfh_rt2dfont.c */
/* TIER2: unvalidated authority=diffemu */
RwReal _rt2dGetTolerance(void)
{

    if (!Rt2dGlobals.sqToleranceValid)
    {
        RwMatrix           *mat;
        RwV3d               p;

        _rt2dCTMGet();      /* brings the CTM and its inverse up to date; MFL.exe ignores the result and uses the inverse directly */
        mat = &Rt2dGlobals.iCTM2d;
        p.x = (Rt2dGlobals.vw.x / Rt2dGlobals.vp.x) *
            Rt2dGlobals.layerDepth * Rt2dGlobals.tolerance;

        p.y = 0.0f;
        p.z = 0.0f;
        RwV3dTransformVector(&p, &p, mat);
        Rt2dGlobals.sqTolerance = RwV3dDotProduct(&p, &p);
        Rt2dGlobals.sqToleranceValid = TRUE;
    }

    return (Rt2dGlobals.sqTolerance);
}
