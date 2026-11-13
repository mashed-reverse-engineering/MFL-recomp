/* DefaultPathStroke2d (MFL.exe 00567890). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/stroke.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses, RwInt32FromRealMacro written as the RwFastRealToUInt32_r asm helper
 * (same instructions), Rt2dPathStroke (the public wrapper) in the same file. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
typedef struct RwSList RwSList;
extern void *_rwSListGetArray(RwSList *sList);
extern RwInt32 _rwSListGetNumEntries(const RwSList *sList);
#define rwSListGetArray(sList) _rwSListGetArray(sList)
#define rwSListGetNumEntries(sList) _rwSListGetNumEntries(sList)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
#define RwRGBARealAddMacro(o,a,b) MACRO_START { (o)->red = (((a)->red) + ( (b)->red)); (o)->green = (((a)->green) + ( (b)->green)); (o)->blue = (((a)->blue) + ( (b)->blue)); (o)->alpha = (((a)->alpha) + ( (b)->alpha)); } MACRO_STOP
#define RwRGBARealScaleMacro(o,a,scale) MACRO_START { (o)->red = (((a)->red) * ( scale)); (o)->green = (((a)->green) * ( scale)); (o)->blue = (((a)->blue) * ( scale)); (o)->alpha = (((a)->alpha) * ( scale)); } MACRO_STOP
#define RwRGBARealAdd(o,a,b) RwRGBARealAddMacro(o,a,b)
#define RwRGBARealScale(o,a,scale) RwRGBARealScaleMacro(o,a,scale)
#define RwV2dAddMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) + ( (b)->x)); (o)->y = (((a)->y) + ( (b)->y)); } MACRO_STOP
#define RwV2dSubMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) - ( (b)->x)); (o)->y = (((a)->y) - ( (b)->y)); } MACRO_STOP
#define RwV2dScaleMacro(o,i,s) MACRO_START { (o)->x = (((i)->x) * ( (s))); (o)->y = (((i)->y) * ( (s))); } MACRO_STOP
#define RwV2dAdd(o,a,b) RwV2dAddMacro(o, a, b)
#define RwV2dSub(o,a,b) RwV2dSubMacro(o, a, b)
#define RwV2dScale(o,i,s) RwV2dScaleMacro(o, i, s)
#define RwRenderStateSetMacro(_state,_value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateSet(_state, _value))
#define RwRenderStateSet(_state,_value) RwRenderStateSetMacro(_state, _value)
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
struct RxObjSpace3DVertex
{
    RwV3d objVertex;
    RwV3d objNormal;
    RwUInt32 color;
    RwReal u;
    RwReal v;
};
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
#define RxObjSpace3DVertexSetU(_vert,_imu) ((_vert)->u = (_imu))
#define RxObjSpace3DVertexSetV(_vert,_imv) ((_vert)->v = (_imv))
typedef RxObjSpace3DLitVertex RwIm3DVertex;
#define RwIm3DVertexSetU RxObjSpace3DVertexSetU
#define RwIm3DVertexSetV RxObjSpace3DVertexSetV
#define RwIm3DVertexSetPos(_vert,_imx,_imy,_imz) MACRO_START { (_vert)->objVertex.x = _imx; (_vert)->objVertex.y = _imy; (_vert)->objVertex.z = _imz; } MACRO_STOP
#define RwIm3DVertexSetRGBA(_vert,_r,_g,_b,_a) MACRO_START { ((_vert)->color = ((_a) << 24) | ((_r) << 16) | ((_g) << 8) | ((_b))); } MACRO_STOP
#define rwTEXTUREBASENAMELENGTH 32
#define rwTEXTUREFILTERMODEMASK 0x000000FF
#define rwTEXTUREADDRESSINGUMASK 0x00000F00
#define rwTEXTUREADDRESSINGVMASK 0x0000F000
typedef struct RwTexDictionary RwTexDictionary;
struct RwTexture
{
    RwRaster *raster;
    RwTexDictionary *dict;
    RwLLLink lInDictionary;

    RwChar name[rwTEXTUREBASENAMELENGTH];
    RwChar mask[rwTEXTUREBASENAMELENGTH];

    RwUInt32 filterAddressing;

    RwInt32 refCount;
};
#define RwTextureGetRasterMacro(_tex) ((_tex)->raster)
#define RwTextureGetFilterModeMacro(_tex) ((RwTextureFilterMode)((_tex)->filterAddressing & rwTEXTUREFILTERMODEMASK))
#define RwTextureGetAddressingMacro(_tex) (((((_tex)->filterAddressing & rwTEXTUREADDRESSINGUMASK) >> 8) == (((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) ? ((RwTextureAddressMode)(((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) : rwTEXTUREADDRESSNATEXTUREADDRESS)
#define RwTextureGetRaster(_tex) RwTextureGetRasterMacro(_tex)
#define RwTextureGetFilterMode(_tex) RwTextureGetFilterModeMacro(_tex)
#define RwTextureGetAddressing(_tex) RwTextureGetAddressingMacro(_tex)
enum RwIm3DTransformFlags
{
    rwIM3D_VERTEXUV = 1,
    rwIM3D_ALLOPAQUE = 2,
    rwIM3D_NOCLIP = 4,

    rwIM3D_VERTEXXYZ = 8,
    rwIM3D_VERTEXRGBA = 16,

    rwIM3DTRANSFORMFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwIm3DTransformFlags RwIm3DTransformFlags;
extern void *RwIm3DTransform(RwIm3DVertex *pVerts, RwUInt32 numVerts,
                              RwMatrix *ltm, RwUInt32 flags);
extern RwBool RwIm3DEnd(void);
extern RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType,
                                           RwImVertexIndex *indices,
                                           RwInt32 numIndices);
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
};
#define VERTEXCACHESIZE 256
typedef struct rt2dShadeParameters rt2dShadeParameters;
struct rt2dShadeParameters
{
    RwRGBAReal col;
    RwV2d uv;
};
typedef struct Rt2dBrush Rt2dBrush;
struct Rt2dBrush
{
    rt2dShadeParameters top;
    rt2dShadeParameters dtop;
    rt2dShadeParameters bottom;
    rt2dShadeParameters dbottom;
    RwRGBA colorCache;
    RwInt32 flag;
    RwTexture *texture;
    RpMaterial *material;
    RwReal halfwidth;
    RwInt32 refCount;
};
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
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(Rt2dBrush, dbottom) == 0x48);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(Rt2dBrush, colorCache) == 0x60);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(Rt2dBrush, flag) == 0x64);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(Rt2dBrush, texture) == 0x68);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_16, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_17, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_18, sizeof(rt2dShadeParameters) == 0x18);      /* Rt2dBrush: dbottom 0x48 (an_codfh_rt2dfont.c) = three of them */
RW_LAYOUT_ASSERT(rw37_row_19, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
RW_LAYOUT_ASSERT(rw37_row_20, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

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
extern Rt2dPath    *_rt2dScratchPath(void);
extern Rt2dPath    *_rt2dSubPathFlatten(Rt2dPath * flatpath,
                                        const Rt2dPath * path);
#define FIELDRGBA 1
#define FIELDUV 2
#define rtFONTMAXPAGES 4
#define rtFONTACHARINDEXSIZE 128
typedef Rt2dFont   *(*Rt2dFontShowCallBack)
    (Rt2dFont * font, const RwChar * string,
     RwReal height, RwV2d * anchor, Rt2dBrush * brush);
struct Rt2dFont
{
    RwBool              isOutline;
    RwReal              height, baseline;
    RwReal              intergap;

    RwUInt32            flag;

    RwTexture          *textures[rtFONTMAXPAGES];

    RwInt16             aCharIndex[rtFONTACHARINDEXSIZE];

    RwInt32             uCharOffset;
    RwInt32             uCharIndexCount;
    RwInt16            *uCharIndex;

    RwInt32             charCount;
    RwSList            *charMap;

    Rt2dFontShowCallBack fontshowCB;

    Rt2dFont           *nextBatch;
    void               *strCache;
};
struct _rt2dFontDictionaryNode
{
    RwChar *name;
    Rt2dFont *font;
    _rt2dFontDictionaryNode *next;
};
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
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(Rt2dFont, flag) == 0x10);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(Rt2dFont, aCharIndex) == 0x24);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(Rt2dFont, uCharOffset) == 0x124);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(Rt2dFont, charMap) == 0x134);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_4, RW_OFFSETOF(Rt2dFont, fontshowCB) == 0x138);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(Rt2dFont) == 0x144);      /* an_codfh_rt2dfont.c */
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
Rt2dPath * DefaultPathStroke2d(Rt2dPath * path, Rt2dBrush * brush);      /* recursive (subpaths) */
/* RwInt32FromRealMacro of the SDK (its int32fromreal): RenderWare's own inline-assembly real -> integer conversion, the same instructions
 * as RwFastRealToUInt32 (see recomp/cheat_allow.txt RwFastRealToUInt32_r); _r: a helper of this file (tier 2 is built /Ob0: called) */
static RwUInt32 RwFastRealToUInt32_r(RwReal x)
{
#if defined(NOASM)
    return (x >= -2147483648.0f && x < 2147483648.0f) ? (RwUInt32)(RwInt32)x : 0x80000000u;
#else
    RwUInt16 savemode;
    RwUInt16 workmode;
    RwInt32 res;

    __asm
    {
        fnstcw savemode
        fld dword ptr [x]
        mov ax, savemode
        or ah, 0ch
        mov workmode, ax
        fldcw workmode
        fistp dword ptr [res]
        fldcw savemode
    }
    return (RwUInt32)res;
#endif
}
/* TIER2: unvalidated authority=diffemu */
Rt2dPath * DefaultPathStroke2d(Rt2dPath * path, Rt2dBrush * brush)
{
    Rt2dPath           *flat;
    rt2dPathNode       *pnode;
    rt2dPathNode       *firstnode;
    RwInt32             vcount, i, vindex;
    RwIm3DVertex       *vdst;
    RwReal              oobaseu, halfwidth, layerDepth, inset;

    if (path)
    {
        RwV2d               pos;
        rt2dShadeParameters sp;
        RwV2d               avgnormal;
        RwMatrix           *ctm = _rt2dCTMGet();
        RwInt32             red;
        RwInt32             green;
        RwInt32             blue;
        RwInt32             alpha;

        layerDepth = Rt2dGlobals.layerDepth;
        halfwidth = brush->halfwidth;
        inset = path->inset;

        flat = path;
        if (!path->flat)
        {
            flat = _rt2dScratchPath();
            _rt2dSubPathFlatten(flat, path);
        }

        firstnode = pnode = _rt2dPathGetSegmentArray(flat);

        vcount = _rt2dPathGetNumSegment(flat);

        if (brush->texture)
        {
            const RwTextureFilterMode filterMode =
                RwTextureGetFilterMode(brush->texture);
            const RwTextureAddressMode addressingMode =
                RwTextureGetAddressing(brush->texture);

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
                             (void *)
                             RwTextureGetRaster(brush->texture));
            RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
                             (void *) filterMode);
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,
                             (void *) addressingMode);
        }
        else
        {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
        }

        oobaseu = 1.0f / pnode[vcount - 1].dist;
        vdst = Rt2dGlobals.brushVertexCache;
        vindex = 0;
        for (i = 0; i < vcount; i++, pnode++)
        {
            
            if (vindex >= VERTEXCACHESIZE- 2)
            {
                
                if (RwIm3DTransform
                    (Rt2dGlobals.brushVertexCache, vindex, ctm,
                    (brush->texture ? rwIM3D_VERTEXUV : 0) |
                    Rt2dGlobals.TransformFlags))
                {
                    RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRISTRIP,
                                                 Rt2dGlobals.topo,
                                                 vindex);
                    RwIm3DEnd();

                }

                vdst = Rt2dGlobals.brushVertexCache;
                vindex = 0;

                i--;
                pnode--;
            }

            RwV2dScale(&avgnormal, &pnode->normal, (halfwidth + inset));

            RwV2dSub(&pos, &pnode->pos, &avgnormal);
            RwIm3DVertexSetPos(vdst, pos.x, pos.y, layerDepth);

            if (brush->flag & FIELDRGBA)   
            {
                RwRGBARealScale(&sp.col, &brush->dtop.col,
                                pnode->dist * oobaseu);
                RwRGBARealAdd(&sp.col, &brush->top.col, &sp.col);

                red   = (RwInt32)RwFastRealToUInt32_r(sp.col.red);
                green = (RwInt32)RwFastRealToUInt32_r(sp.col.green);
                blue  = (RwInt32)RwFastRealToUInt32_r(sp.col.blue);
                alpha = (RwInt32)RwFastRealToUInt32_r(sp.col.alpha);

                RwIm3DVertexSetRGBA(vdst,
                                    (RwUInt8) red,
                                    (RwUInt8) green,
                                    (RwUInt8) blue, (RwUInt8) alpha);
            }
            else  
            {
                RwIm3DVertexSetRGBA(vdst,
                                    brush->colorCache.red,
                                    brush->colorCache.green,
                                    brush->colorCache.blue,
                                    brush->colorCache.alpha);
            }

            if (brush->flag & FIELDUV)
            {
                /* RwV2dScale + RwV2dAdd; u in one expression: MFL.exe adds the unrounded product for u, the stored one for v */
                RwReal t = pnode->dist * oobaseu;

                sp.uv.y = brush->dtop.uv.y * t;
                sp.uv.x = brush->top.uv.x + brush->dtop.uv.x * t;
                sp.uv.y = brush->top.uv.y + sp.uv.y;
                RwIm3DVertexSetU(vdst, sp.uv.x);
                RwIm3DVertexSetV(vdst, sp.uv.y);
            }

            vdst++;

            RwV2dScale(&avgnormal, &pnode->normal, (halfwidth - inset));

            RwV2dAdd(&pos, &pnode->pos, &avgnormal);
            RwIm3DVertexSetPos(vdst, pos.x, pos.y, layerDepth);

            if (brush->flag & FIELDRGBA)  
            {
                RwRGBARealScale(&sp.col, &brush->dbottom.col,
                                pnode->dist * oobaseu);
                RwRGBARealAdd(&sp.col, &brush->bottom.col, &sp.col);

                red   = (RwInt32)RwFastRealToUInt32_r(sp.col.red);
                green = (RwInt32)RwFastRealToUInt32_r(sp.col.green);
                blue  = (RwInt32)RwFastRealToUInt32_r(sp.col.blue);
                alpha = (RwInt32)RwFastRealToUInt32_r(sp.col.alpha);

                RwIm3DVertexSetRGBA(vdst,
                                    (RwUInt8) red,
                                    (RwUInt8) green,
                                    (RwUInt8) blue, (RwUInt8) alpha);
            }
            else 
            {
                RwIm3DVertexSetRGBA(vdst,
                                    brush->colorCache.red,
                                    brush->colorCache.green,
                                    brush->colorCache.blue,
                                    brush->colorCache.alpha);
            }

            if (brush->flag & FIELDUV)
            {
                RwReal t = pnode->dist * oobaseu;      /* as above */

                sp.uv.y = brush->dbottom.uv.y * t;
                sp.uv.x = brush->bottom.uv.x + brush->dbottom.uv.x * t;
                sp.uv.y = brush->bottom.uv.y + sp.uv.y;
                RwIm3DVertexSetU(vdst, sp.uv.x);
                RwIm3DVertexSetV(vdst, sp.uv.y);
            }

            vdst++;

            vindex += 2;
        }

        if (vindex > 0)
        {
            
            if (RwIm3DTransform
                (Rt2dGlobals.brushVertexCache, vindex, ctm,
                 (brush->texture ? rwIM3D_VERTEXUV : 0) |
                 Rt2dGlobals.TransformFlags))
            {
                RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRISTRIP,
                                             Rt2dGlobals.topo, vindex);
                RwIm3DEnd();

            }
        }

        DefaultPathStroke2d(path->next, brush);
    }

    return (path);
}

/* stroke.c: the public entry (MFL.exe 00567880, a jump to DefaultPathStroke2d) */
Rt2dPath * Rt2dPathStroke(Rt2dPath * path, Rt2dBrush * brush)
{
    return (DefaultPathStroke2d(path, brush));
}
