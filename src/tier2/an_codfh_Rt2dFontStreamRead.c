/* Rt2dFontStreamRead (MFL.exe 00564e40). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/font.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses, the static helpers rt2dFontOutlineStreamRead /
 * rt2dFontBitmapStreamRead written inline (MFL.exe inlines them; their returns became gotos). Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
typedef signed char RwInt8;
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
typedef struct RwSList RwSList;
extern void *_rwSListGetArray(RwSList *sList);
extern void *_rwSListGetNewEntries(RwSList *sList, RwInt32 entry,
                                          RwUInt32 hint);
#define rwSListGetArray(sList) _rwSListGetArray(sList)
#define rwSListGetNewEntries(sList,entry,hint) _rwSListGetNewEntries(sList, entry, hint)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;
    RwReal green;
    RwReal blue;
    RwReal alpha;
};
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RwStream *RwStreamReadReal(RwStream *stream, RwReal *reals,
                                  RwUInt32 numBytes);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
extern RwStream *RwStreamReadInt16(RwStream *stream, RwInt16 *ints,
                                   RwUInt32 numBytes);
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
struct RwRaster
{
    RwRaster *parent;
    RwUInt8 *cpPixels;
    RwUInt8 *palette;
    RwInt32 width, height, depth;
    RwInt32 stride;
    RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType;
    RwUInt8 cFlags;
    RwUInt8 privateFlags;
    RwUInt8 cFormat;

    RwUInt8 *originalPixels;
    RwInt32 originalWidth;
    RwInt32 originalHeight;
    RwInt32 originalStride;
};
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
#define RwTextureGetRasterMacro(_tex) ((_tex)->raster)
#define RwTextureGetRaster(_tex) RwTextureGetRasterMacro(_tex)
extern RwTexture *RwTextureRead(const RwChar * name,
                                  const RwChar * maskName);
extern RwTexDictionary *RwTexDictionaryGetCurrent(void);
extern RwTexDictionary *RwTexDictionarySetCurrent(RwTexDictionary * dict);
#define rwID_2DPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x90)
#define rwID_2DPATH MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa1)
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
extern Rt2dPath *
Rt2dPathStreamRead(RwStream *stream);
extern RwBool
Rt2dFontDestroy(Rt2dFont *font);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(Rt2dBBox, h) == 0xc);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(Rt2dBrush, dbottom) == 0x48);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(Rt2dBrush, colorCache) == 0x60);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(Rt2dBrush, flag) == 0x64);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(Rt2dBrush, texture) == 0x68);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_row_9, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_11, sizeof(rt2dShadeParameters) == 0x18);      /* Rt2dBrush: dbottom 0x48 (an_codfh_rt2dfont.c) = three of them */
RW_LAYOUT_ASSERT(rw37_row_12, sizeof(RwRGBAReal) == 0x10);      /* RpLight: color 0x18, minusCosAngle 0x28 (an_codfh_RpLightStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct rt2dPathNode rt2dPathNode;
#define rtFONTVERSION 1
#define rtFONTMAXPAGES 4
#define rtFONTACHARINDEXSIZE 128
typedef Rt2dFont   *(*Rt2dFontShowCallBack)
    (Rt2dFont * font, const RwChar * string,
     RwReal height, RwV2d * anchor, Rt2dBrush * brush);
typedef struct rt2dCharRect rt2dCharRect;
struct rt2dCharRect
{
    RwReal              width;
    RwTexture          *tex;
    RwV2d               uv[2];
    Rt2dPath           *path;
    RwInt8              charpage;
};
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
#define RT2D_STREAM_PATH_BEGIN 5
#define _rt2dMarkerDecode(_code,_ver,_marker) MACRO_START { (_ver) = (((_code) & 0xFF000000) >> 24); (_marker) = ((_code) & 0x00FFFFFF); } MACRO_STOP
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
extern Rt2dFont * rt2dFontShowOutline(Rt2dFont * font, const RwChar * string, RwReal height, RwV2d * anchor, Rt2dBrush * brush);
extern Rt2dFont * rt2dFontShowBitmap(Rt2dFont * font, const RwChar * string, RwReal height, RwV2d * anchor, Rt2dBrush * brush);
extern Rt2dFont * FontCreate2d( void );
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
RW_LAYOUT_ASSERT(rw37_priv_16, sizeof(rt2dCharRect) == 0x20);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_17, RW_OFFSETOF(rt2dCharRect, path) == 0x18);      /* an_codfh_rt2dfont.c */
RW_LAYOUT_ASSERT(rw37_priv_18, RW_OFFSETOF(rt2dCharRect, charpage) == 0x1c);      /* an_codfh_rt2dfont.c */
/* TIER2: validated 2026-11-14 authority=diffemu */
Rt2dFont * Rt2dFontStreamRead(RwStream * stream)
{
    Rt2dFont           *font;
    RwInt32            i, marker, ver;
    rt2dCharRect       *charMap;
    RwTexDictionary    *texDict;

    font = FontCreate2d();
    if (!font)
    {
        return ((Rt2dFont *)NULL);
    }

    if (RwStreamReadInt32(stream, &i, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);

    _rt2dMarkerDecode(i, ver, marker);

    if (ver < rtFONTVERSION)
    {

        return (NULL);
    }

    if (RwStreamReadInt32(stream, (RwInt32 *) &font->isOutline, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);

    if (RwStreamReadReal(stream, &font->height, sizeof(RwReal)) == NULL)
        return ((Rt2dFont *)NULL);
    if (RwStreamReadReal(stream, &font->baseline, sizeof(RwReal)) == NULL)
        return ((Rt2dFont *)NULL);
    if (RwStreamReadReal(stream, &font->intergap, sizeof(RwReal)) == NULL)
        return ((Rt2dFont *)NULL);

    if (RwStreamReadInt32(stream, (RwInt32 *) &font->flag, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);

    if (RwStreamReadInt32(stream, (RwInt32 *) &font->uCharOffset, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);
    if (RwStreamReadInt32(stream, (RwInt32 *) &font->charCount, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);
    if (RwStreamReadInt32(stream, (RwInt32 *) &font->uCharIndexCount, sizeof(RwInt32)) == NULL)
        return ((Rt2dFont *)NULL);

    if (font->uCharIndexCount > 0)
    {
        font->uCharIndex = (RwInt16 *) RwMalloc(font->uCharIndexCount * sizeof(RwInt16),
                                                rwMEMHINTDUR_EVENT | rwID_2DPLUGIN);

        memset(font->uCharIndex, 0, font->uCharIndexCount * sizeof(RwInt16));

        if (font->uCharIndex == NULL)
        {
            Rt2dFontDestroy(font);

            return (NULL);
        }

        if (RwStreamReadInt16(stream,
            (RwInt16 *) font->uCharIndex, font->uCharIndexCount * sizeof(RwInt16)) == NULL)
            return ((Rt2dFont *)NULL);
    }

    if (RwStreamReadInt16(stream,
        (RwInt16 *) font->aCharIndex, rtFONTACHARINDEXSIZE * sizeof(RwInt16)) == NULL)
        return ((Rt2dFont *)NULL);

    rwSListGetNewEntries(font->charMap, font->charCount,
        rwMEMHINTDUR_EVENT | rwID_2DPLUGIN);

    charMap = (rt2dCharRect *) rwSListGetArray(font->charMap);

    memset(charMap, 0, (font->charCount * sizeof(rt2dCharRect)));

    if (font->isOutline)
    {
        /* rt2dFontOutlineStreamRead, inlined (MFL.exe inlines it) */
        Rt2dPath *pathHead;

        charMap = (rt2dCharRect *) rwSListGetArray(font->charMap);

        for (i = 0; i < font->charCount; i++)
        {
            pathHead = (Rt2dPath *) NULL;

            if (RwStreamReadInt32(stream, (RwInt32 *) &marker, sizeof(RwInt32)) == NULL)
                goto outline_failed;

            if (RwStreamReadReal(stream, (RwReal *) &charMap[i].width, sizeof(RwReal)) == NULL)
                goto outline_failed;

            if (RwStreamReadInt32(stream, (RwInt32 *) &marker, sizeof(RwInt32))== NULL)
                goto outline_failed;

            if (marker == RT2D_STREAM_PATH_BEGIN)
            {
                if (!RwStreamFindChunk(stream, rwID_2DPATH, NULL, NULL))
                    goto outline_failed;

                pathHead = Rt2dPathStreamRead(stream);

                if (RwStreamReadInt32(stream, (RwInt32 *) &marker, sizeof(RwInt32)) == NULL)
                    goto outline_failed;
            }

            if (RwStreamReadInt32(stream, (RwInt32 *) &marker, sizeof(RwInt32)) == NULL)
                goto outline_failed;

            charMap[i].path = pathHead;
        }

        font->fontshowCB = rt2dFontShowOutline;
        return (font);

    outline_failed:
        Rt2dFontDestroy(font);
        return ((Rt2dFont *) NULL);
    }
    else
    {
        /* rt2dFontBitmapStreamRead, inlined (MFL.exe inlines it) */
        RwInt32 count, size;
        RwChar str[rwTEXTUREBASENAMELENGTH + 1];

        texDict = RwTexDictionaryGetCurrent();
        RwTexDictionarySetCurrent(Rt2dGlobals.currFontTexDictionary);

        charMap = (rt2dCharRect *) rwSListGetArray(font->charMap);

        for (i = 0; i < font->charCount; i++)
        {
            if (RwStreamReadReal(stream, (RwReal *) &charMap[i].uv, (4 * sizeof(RwReal))) == NULL)
                goto bitmap_failed;

            if (RwStreamReadReal(stream, (RwReal *) &charMap[i].width, sizeof(RwReal)) == NULL)
                goto bitmap_failed;

            if (RwStreamRead(stream, (RwChar *) &charMap[i].charpage, sizeof(RwChar)) != sizeof(RwChar))
                goto bitmap_failed;

            charMap[i].path = NULL;
        }

        if (RwStreamReadInt32(stream, (RwInt32 *) &count, sizeof(RwInt32)) == NULL)
            goto bitmap_failed;

        size = rwTEXTUREBASENAMELENGTH * sizeof(RwChar);

        for (i = 0; i < count; i++)
        {
            if (RwStreamRead(stream, str, (RwUInt32) size) != (RwUInt32) size)
                goto bitmap_failed;

            if ((font->textures[i] = RwTextureRead(str, NULL)) == NULL)
                goto bitmap_failed;
        }

        for (i = 0; i < font->charCount; i++)
        {
            charMap[i].tex = font->textures[(RwUInt32) charMap[i].charpage];

            if (RwTextureGetRaster(charMap[i].tex) == NULL)
                goto bitmap_failed;
        }

        font->nextBatch = (Rt2dFont *)NULL;
        font->strCache = NULL;

        font->fontshowCB = rt2dFontShowBitmap;
        goto bitmap_done;

    bitmap_failed:
        Rt2dFontDestroy(font);
        font = (Rt2dFont *)NULL;

    bitmap_done:
        RwTexDictionarySetCurrent(texDict);
    }

    return (font);
}
