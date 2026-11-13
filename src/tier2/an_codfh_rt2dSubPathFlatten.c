/* rt2dSubPathFlatten (MFL.exe 00563bf0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/path.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses, the function named _rt2dSubPathFlatten as MFL.exe's symbol.
 * Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwSqrt(_result,_x) rwSqrtMacro(_result, _x)
#define rwInvSqrt(_recip,_x) rwInvSqrtMacro(_recip, _x)
typedef struct RwSList RwSList;
extern void *_rwSListGetArray(RwSList *sList);
extern void *_rwSListGetNewEntry(RwSList *sList, RwUInt32 hint);
extern RwInt32 _rwSListGetNumEntries(const RwSList *sList);
#define rwSListGetArray(sList) _rwSListGetArray(sList)
#define rwSListGetNewEntry(sList,hint) _rwSListGetNewEntry(sList, hint)
#define rwSListGetNumEntries(sList) _rwSListGetNumEntries(sList)
#define rwSqrtMacro(_root,_input) ( *(_root) = _rwSqrt(_input) )
#define rwInvSqrtMacro(_recip,_input) ( *(_recip) = _rwInvSqrt(_input) )
#define rwSqrtInvSqrtMacro(_root,_recip,_input) MACRO_START { RwReal _tmp = _input; rwSqrt((_root), _tmp); rwInvSqrt((_recip), _tmp); } MACRO_STOP
#define RwV2dAddMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) + ( (b)->x)); (o)->y = (((a)->y) + ( (b)->y)); } MACRO_STOP
#define RwV2dScaleMacro(o,i,s) MACRO_START { (o)->x = (((i)->x) * ( (s))); (o)->y = (((i)->y) * ( (s))); } MACRO_STOP
#define RwV2dDotProductMacro(a,b) (( ((((a)->x) * ( (b)->x))) + ( (((a)->y) * ( (b)->y)))))
#define RwV2dNormalizeMacro(_result,_out,_in) MACRO_START { RwReal length2 = RwV2dDotProductMacro((_in), (_in)); RwReal recip; rwSqrtInvSqrtMacro(&(_result), &recip, length2); RwV2dScaleMacro((_out), (_in), recip); } MACRO_STOP
#define RwV2dAdd(o,a,b) RwV2dAddMacro(o, a, b)
#define RwV2dScale(o,i,s) RwV2dScaleMacro(o, i, s)
#define RwV2dDotProduct(a,b) RwV2dDotProductMacro(a,b)
extern RwReal _rwSqrt(const RwReal num);
extern RwReal _rwInvSqrt(const RwReal num);
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
typedef struct RwTexDictionary RwTexDictionary;
#define rwID_2DPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x90)
#define rwID_2DPATH MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa1)
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
extern Rt2dPath *
Rt2dPathClose(Rt2dPath *path);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
/* ---- end of the generated declarations ---- */

typedef unsigned char Rt2dFontChar;
#define _rt2dPathIsLocked(path) (path->numSegment == -1)
#define _rt2dPathGetNumSegment(path) (_rt2dPathIsLocked(path) ? rwSListGetNumEntries((RwSList *)(path)->segments) : path->numSegment)
#define _rt2dPathGetSegmentArray(path) ((rt2dPathNode *)(_rt2dPathIsLocked(path) ? rwSListGetArray((RwSList *)(path)->segments) : path->segments))
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
extern RwReal       _rt2dGetTolerance(void);
typedef RwV2d       RtCubicBezier[4];
#define SubPathLineto2dMacro(_path,_pt) MACRO_START { RwV2d dir; RwReal l; RwInt32 vcount = rwSListGetNumEntries((RwSList *)((_path)->segments)); rt2dPathNode *_pnode = (rt2dPathNode *) rwSListGetNewEntry((RwSList *)(((_path))->segments), rwID_2DPLUGIN | rwMEMHINTDUR_EVENT); _pnode->type = rt2dLINE; _pnode->pos.x = ((_pt))->x; _pnode->pos.y = ((_pt))->y; dir.x = ((_pt))->y - (_pnode-1)->pos.y; dir.y = (_pnode-1)->pos.x - ((_pt))->x; RwV2dNormalizeMacro(l, &(_pnode)->normal, &dir); _pnode->dist = (_pnode-1)->dist + l; if (vcount > 1) { RwV2dAdd(&dir, &(_pnode-1)->normal, &(_pnode)->normal); l = 1.0f / RwV2dDotProduct(&(_pnode-1)->normal, &dir); RwV2dScale(&(_pnode-1)->normal, &dir, l); } else { (_pnode-1)->normal = (_pnode)->normal; } } MACRO_STOP
#define SubPathLineto2d(_path,_pt) SubPathLineto2dMacro(_path, _pt)
extern void Flatten2d(Rt2dPath * const flat, const RtCubicBezier control, int depth);
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
Rt2dPath * _rt2dSubPathFlatten(Rt2dPath * flat, const Rt2dPath * path)
{
    rt2dPathNode       *pnode, *qnode;
    RwInt32             vcount, i;

    Rt2dGlobals.sqTolerance = _rt2dGetTolerance();

    pnode = _rt2dPathGetSegmentArray(path);
    vcount = _rt2dPathGetNumSegment(path);

    for (i = 0; i < vcount; i++, pnode++)
    {
        if (pnode->type == rt2dCURVE)
        {
            RwV2d               control[4];

            control[0] = (pnode - 1)->pos;
            control[1] = (pnode + 0)->pos;
            control[2] = (pnode + 1)->pos;
            control[3] = (pnode + 2)->pos;
            Flatten2d(flat, control, Rt2dGlobals.flatDepth);
            i += 2;
            pnode += 2;
        }
        else if (pnode->type == rt2dLINE)
        {
            SubPathLineto2d(flat, &pnode->pos);
        }
        else
        {
            qnode = (rt2dPathNode *)
                rwSListGetNewEntry((RwSList *)(flat->segments),
                           rwID_2DPATH | rwMEMHINTDUR_EVENT);
            *qnode = *pnode;
        }
    }

    if (path->closed)
    {
        Rt2dPathClose(flat);
    }

    return (flat);
}
