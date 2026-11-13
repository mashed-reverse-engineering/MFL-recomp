/* Flatten2d (MFL.exe 00563e70). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/path.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses, the forceinline helper SubPathLineto2dInline replaced by its macro
 * twin SubPathLineto2dMacro, the static helper Bisect2d written as a macro (MFL.exe inlines it). Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwSqrt(_result,_x) rwSqrtMacro(_result, _x)
#define rwInvSqrt(_recip,_x) rwInvSqrtMacro(_recip, _x)
typedef struct RwSList RwSList;
extern void *_rwSListGetNewEntry(RwSList *sList, RwUInt32 hint);
extern RwInt32 _rwSListGetNumEntries(const RwSList *sList);
#define rwSListGetNewEntry(sList,hint) _rwSListGetNewEntry(sList, hint)
#define rwSListGetNumEntries(sList) _rwSListGetNumEntries(sList)
#define rwSqrtMacro(_root,_input) ( *(_root) = _rwSqrt(_input) )
#define rwInvSqrtMacro(_recip,_input) ( *(_recip) = _rwInvSqrt(_input) )
#define rwSqrtInvSqrtMacro(_root,_recip,_input) MACRO_START { RwReal _tmp = _input; rwSqrt((_root), _tmp); rwInvSqrt((_recip), _tmp); } MACRO_STOP
#define RwV2dAssignMacro(_target,_source) ( *(_target) = *(_source) )
#define RwV2dAddMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) + ( (b)->x)); (o)->y = (((a)->y) + ( (b)->y)); } MACRO_STOP
#define RwV2dSubMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) - ( (b)->x)); (o)->y = (((a)->y) - ( (b)->y)); } MACRO_STOP
#define RwV2dScaleMacro(o,i,s) MACRO_START { (o)->x = (((i)->x) * ( (s))); (o)->y = (((i)->y) * ( (s))); } MACRO_STOP
#define RwV2dDotProductMacro(a,b) (( ((((a)->x) * ( (b)->x))) + ( (((a)->y) * ( (b)->y)))))
#define RwV2dNormalizeMacro(_result,_out,_in) MACRO_START { RwReal length2 = RwV2dDotProductMacro((_in), (_in)); RwReal recip; rwSqrtInvSqrtMacro(&(_result), &recip, length2); RwV2dScaleMacro((_out), (_in), recip); } MACRO_STOP
#define RwV2dAssign(o,a) RwV2dAssignMacro(o, a)
#define RwV2dAdd(o,a,b) RwV2dAddMacro(o, a, b)
#define RwV2dSub(o,a,b) RwV2dSubMacro(o, a, b)
#define RwV2dScale(o,i,s) RwV2dScaleMacro(o, i, s)
#define RwV2dDotProduct(a,b) RwV2dDotProductMacro(a,b)
extern RwReal _rwSqrt(const RwReal num);
extern RwReal _rwInvSqrt(const RwReal num);
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
typedef struct RwTexDictionary RwTexDictionary;
#define rwID_2DPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x90)
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
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
/* ---- end of the generated declarations ---- */

extern void __cdecl terminate();
typedef unsigned char Rt2dFontChar;
enum rt2dLinetype
{
    rt2dMOVE = 0,
    rt2dLINE = 1,
    rt2dCURVE = 2,
    rt2DLINETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rt2dLinetype rt2dLinetype;
typedef struct rt2dPathNode rt2dPathNode;
struct rt2dPathNode
{
    rt2dLinetype        type;
    RwV2d               pos, normal;
    RwReal              dist;
};
struct Rt2dPath
{
    void                *segments;
    RwInt32             numSegment;
    RwBool              closed;
    RwBool              flat;
    RwReal              inset;
    Rt2dPath           *next;
    Rt2dPath           *curr;
};
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
typedef RwV2d       RtCubicBezier[4];
#define Rt2dIsFlatMacro(result,_control) MACRO_START { RwV2d delta, dir; RwReal Sqdist, dot; RwV2dSub(&delta, &(_control)[3], &(_control)[0]); Sqdist = RwV2dDotProduct(&delta, &delta); RwV2dSub(&dir, &(_control)[1], &(_control)[0]); dot = RwV2dDotProduct(&dir, &delta); result = (RwV2dDotProduct(&dir, &dir) - Rt2dGlobals.sqTolerance) * Sqdist < (dot * dot); if (result) { RwV2dSub(&dir, &(_control)[2], &(_control)[0]); dot = RwV2dDotProduct(&dir, &delta); result = (RwV2dDotProduct(&dir, &dir) - Rt2dGlobals.sqTolerance) * Sqdist < (dot * dot); } } MACRO_STOP
#define SubPathLineto2dMacro(_path,_pt) MACRO_START { RwV2d dir; RwReal l; RwInt32 vcount = rwSListGetNumEntries((RwSList *)((_path)->segments)); rt2dPathNode *_pnode = (rt2dPathNode *) rwSListGetNewEntry((RwSList *)(((_path))->segments), rwID_2DPLUGIN | rwMEMHINTDUR_EVENT); _pnode->type = rt2dLINE; _pnode->pos.x = ((_pt))->x; _pnode->pos.y = ((_pt))->y; dir.x = ((_pt))->y - (_pnode-1)->pos.y; dir.y = (_pnode-1)->pos.x - ((_pt))->x; RwV2dNormalizeMacro(l, &(_pnode)->normal, &dir); _pnode->dist = (_pnode-1)->dist + l; if (vcount > 1) { RwV2dAdd(&dir, &(_pnode-1)->normal, &(_pnode)->normal); l = 1.0f / RwV2dDotProduct(&(_pnode-1)->normal, &dir); RwV2dScale(&(_pnode-1)->normal, &dir, l); } else { (_pnode-1)->normal = (_pnode)->normal; } } MACRO_STOP
#define SubPathLineto2d(_path,_pt) SubPathLineto2dMacro(_path, _pt)
void Flatten2d(Rt2dPath * const flat, const RtCubicBezier control, int depth);      /* recursive */
/* Bisect2d (a static function of path.c that MFL.exe inlines; as a macro so the x87 temporaries stay where the exe keeps them) */
#define Bisect2dMacro(control, left, right)                     \
MACRO_START                                                     \
{                                                               \
    RwV2d               vTemp, vTemp2;                          \
                                                                \
    RwV2dAdd(&vTemp2, &control[1], &control[2]);                \
    RwV2dScale(&vTemp2, &vTemp2, (RwReal) (0.5));               \
                                                                \
    RwV2dAdd(&vTemp, &control[0], &control[1]);                 \
    RwV2dScale(&left[1], &vTemp, (RwReal) (0.5));               \
                                                                \
    RwV2dAdd(&vTemp, &control[3], &control[2]);                 \
    RwV2dScale(&right[2], &vTemp, (RwReal) (0.5));              \
                                                                \
    RwV2dAdd(&vTemp, &left[1], &vTemp2);                        \
    RwV2dScale(&left[2], &vTemp, (RwReal) (0.5));               \
                                                                \
    RwV2dAdd(&vTemp, &right[2], &vTemp2);                       \
    RwV2dScale(&right[1], &vTemp, (RwReal) (0.5));              \
                                                                \
    RwV2dAdd(&vTemp, &left[2], &right[1]);                      \
    RwV2dScale(&left[3], &vTemp, (RwReal) (0.5));               \
                                                                \
    RwV2dAssign(&left[0], &control[0]);                         \
    RwV2dAssign(&right[0], &left[3]);                           \
    RwV2dAssign(&right[3], &control[3]);                        \
}                                                               \
MACRO_STOP
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
void Flatten2d(Rt2dPath * const flat, const RtCubicBezier control, int depth)
{
    RwBool              terminate;

    terminate = (--depth < 0);
    if (terminate)
    {
        SubPathLineto2d(flat, &control[3]);
    }
    else
    {
        Rt2dIsFlatMacro(terminate, control);
        if (terminate)
        {
            SubPathLineto2d(flat, &control[3]);
        }
        else
        {
            RtCubicBezier       left, right;

            Bisect2dMacro(control, left, right);

            Flatten2d(flat, left, depth);
            Flatten2d(flat, right, depth);
        }
    }

    return;
}
