/* rt2dFontClose (MFL.exe 00565d30). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/font.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSList RwSList;
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
#define rwTEXTUREBASENAMELENGTH 32
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
extern RwBool RwTexDictionaryDestroy(RwTexDictionary * dict);
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
extern RwBool
Rt2dFontDestroy(Rt2dFont *font);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(Rt2dBrush, dbottom) == 0x48);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(Rt2dBrush, colorCache) == 0x60);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(Rt2dBrush, flag) == 0x64);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(Rt2dBrush, texture) == 0x68);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_7, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_8, sizeof(rt2dShadeParameters) == 0x18);      /* Rt2dBrush: dbottom 0x48 (an_codfh_rt2dfont.c) = three of them */
RW_LAYOUT_ASSERT(rw37_row_9, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct rt2dPathNode rt2dPathNode;
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(_rt2dFontDictionaryNode, font) == 4 && RW_OFFSETOF(_rt2dFontDictionaryNode, next) == 8);      /* MFL.exe 0x565d4e mov edx,[esi+0x4] (Rt2dFontDestroy argument); 0x565d62 mov edi,[esi+0x8] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(Rt2dGlobalVars, fontDictNodeFreeList) == 0x514);      /* MFL.exe 0x565d32 / 0x565d57: fontDictionary at 0x948078, fontDictNodeFreeList at 0x948074; Rt2dGlobals is at 0x947b60 */
RW_LAYOUT_ASSERT(rw37_own_2, sizeof(_rt2dTriIndex) == 8);      /* derived: currFontTexDictionary 0x520 and layerDepth 0x6e0 (an_codfh_rt2dfont.c) leave 8 bytes for triVertIndex */
RW_LAYOUT_ASSERT(rw37_own_3, sizeof(Rt2dStrBuffer) == 0x408);      /* derived: layerDepth 0x6e0 and fontReadFunc 0xc00 (an_codfh_rt2dfont.c) hold with six 16 byte cluster definitions and this size */
/* TIER2: unvalidated authority=diffemu */
void rt2dFontClose(void)
{
    _rt2dFontDictionaryNode *node;

    node = Rt2dGlobals.fontDictionary;
    while(node)
    {
        _rt2dFontDictionaryNode *next;
        RwFree(node->name);
        Rt2dFontDestroy(node->font);
        next = node->next;
        RwFreeListFree(Rt2dGlobals.fontDictNodeFreeList, node);
        node=next;
    }
    Rt2dGlobals.fontDictionary = (_rt2dFontDictionaryNode *)NULL;

    if (Rt2dGlobals.fontTexDictionary != NULL)
        RwTexDictionaryDestroy(Rt2dGlobals.fontTexDictionary);
    Rt2dGlobals.fontTexDictionary = NULL;
    Rt2dGlobals.currFontTexDictionary = NULL;

    RwFreeListDestroy(Rt2dGlobals.fontDictNodeFreeList);

    Rt2dGlobals.fontDictNodeFreeList = (RwFreeList *)NULL;

    RwFreeListDestroy(Rt2dGlobals.fontFreeList);

    Rt2dGlobals.fontFreeList = (RwFreeList *)NULL;

    if (Rt2dGlobals.uCharIndex != NULL)
        RwFree(Rt2dGlobals.uCharIndex);

    Rt2dGlobals.uCharIndex = NULL;

    if (Rt2dGlobals.fontPath != NULL)
        RwFree(Rt2dGlobals.fontPath);
    Rt2dGlobals.fontPath = NULL;
    Rt2dGlobals.fontPathSize = 0;

    return;
}
