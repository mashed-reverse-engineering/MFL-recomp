/* rt2dFontShowBitmap (MFL.exe): RenderWare Graphics 3.7 (Criterion Software), rt2d font module.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/font.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), CODFH_WIN32 trace code dropped, the Im3D / RGBA real macros written out.
 * Exe deltas (from the disassembly of 0x005653f0): the vertex colours are converted with RwFastRealToUInt32 (plain truncation, no + 0.5), and the
 * pen advances as (x + intergap) + width.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwFreeList RwFreeList;
typedef struct Rt2dFont Rt2dFont;
typedef struct Rt2dBrush Rt2dBrush;
struct Rt2dFont {
    RwBool isOutline;                   /* 0x00 */
    RwReal height;                      /* 0x04 */
    RwReal baseline;                    /* 0x08 */
    RwReal intergap;                    /* 0x0c */
    RwUInt32 flag;                      /* 0x10 */
    RwTexture *textures[4];             /* 0x14 */
    short aCharIndex[128];              /* 0x24 */
    RwInt32 uCharOffset;                /* 0x124 */
    RwInt32 uCharIndexCount;            /* 0x128 */
    short *uCharIndex;                  /* 0x12c */
    RwInt32 charCount;                  /* 0x130 */
    void *charMap;                      /* 0x134 */
    Rt2dFont *(*fontshowCB)(Rt2dFont *font, const RwChar *string, RwReal height, RwV2d *anchor, Rt2dBrush *brush);      /* 0x138 */
    Rt2dFont *nextBatch;                /* 0x13c */
    void *strCache;                     /* 0x140 */
};
typedef struct Rt2dGlobalVars {
    RwUInt8 pad000[0x200];              /* topo */
    short fonttopo[384];                /* 0x200 */
    void *brushFreeList;                /* 0x500 */
    RwIm3DVertex *brushVertexCache;     /* 0x504 */
    RwFreeList *fontFreeList;           /* 0x508 */
    RwChar *fontPath;                   /* 0x50c */
    RwUInt32 fontPathSize;              /* 0x510 */
    RwFreeList *fontDictNodeFreeList;   /* 0x514 */
    void *fontDictionary;               /* 0x518 */
    RwTexDictionary *fontTexDictionary; /* 0x51c */
    RwTexDictionary *currFontTexDictionary;      /* 0x520 */
    RwUInt8 pad524[0x6e0 - 0x524];
    RwReal layerDepth;                  /* 0x6e0 */
    RwUInt8 pad6e4[0xc00 - 0x6e4];
    void *fontReadFunc;                 /* 0xc00 */
    RwUInt8 padc04[0xd5c - 0xc04];
    RwInt32 *uCharIndex;                /* 0xd5c */
    RwInt32 uCharIndexSize;             /* 0xd60 */
} Rt2dGlobalVars;
RW_LAYOUT_ASSERT(rw_assert_font_flag, RW_OFFSETOF(Rt2dFont, flag) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_font_aCharIndex, RW_OFFSETOF(Rt2dFont, aCharIndex) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_font_uCharOffset, RW_OFFSETOF(Rt2dFont, uCharOffset) == 0x124);
RW_LAYOUT_ASSERT(rw_assert_font_charMap, RW_OFFSETOF(Rt2dFont, charMap) == 0x134);
RW_LAYOUT_ASSERT(rw_assert_font_fontshowCB, RW_OFFSETOF(Rt2dFont, fontshowCB) == 0x138);
RW_LAYOUT_ASSERT(rw_assert_font_size, sizeof(Rt2dFont) == 0x144);
RW_LAYOUT_ASSERT(rw_assert_g_fonttopo, RW_OFFSETOF(Rt2dGlobalVars, fonttopo) == 0x200);
RW_LAYOUT_ASSERT(rw_assert_g_fontFreeList, RW_OFFSETOF(Rt2dGlobalVars, fontFreeList) == 0x508);
RW_LAYOUT_ASSERT(rw_assert_g_fontPathSize, RW_OFFSETOF(Rt2dGlobalVars, fontPathSize) == 0x510);
RW_LAYOUT_ASSERT(rw_assert_g_fontDictionary, RW_OFFSETOF(Rt2dGlobalVars, fontDictionary) == 0x518);
RW_LAYOUT_ASSERT(rw_assert_g_currFontTexDictionary, RW_OFFSETOF(Rt2dGlobalVars, currFontTexDictionary) == 0x520);
RW_LAYOUT_ASSERT(rw_assert_g_brushVertexCache, RW_OFFSETOF(Rt2dGlobalVars, brushVertexCache) == 0x504);
RW_LAYOUT_ASSERT(rw_assert_g_layerDepth, RW_OFFSETOF(Rt2dGlobalVars, layerDepth) == 0x6e0);
RW_LAYOUT_ASSERT(rw_assert_g_fontReadFunc, RW_OFFSETOF(Rt2dGlobalVars, fontReadFunc) == 0xc00);
RW_LAYOUT_ASSERT(rw_assert_g_uCharIndex, RW_OFFSETOF(Rt2dGlobalVars, uCharIndex) == 0xd5c);
RW_LAYOUT_ASSERT(rw_assert_g_uCharIndexSize, RW_OFFSETOF(Rt2dGlobalVars, uCharIndexSize) == 0xd60);

#define rt2dMAXVINDEX 384      /* VERTEXCACHESIZE (256) / 4 * 6 */
#define RW_GLOBALS_RWMALLOC 0x108      /* RwGlobals.memoryFuncs (rw37.h RwGlobals37) */
#define RW_GLOBALS_RWREALLOC 0x110
#define RW_GLOBALS_STRCPY 0xcc      /* RwGlobals.stringFuncs */
#define RW_GLOBALS_STRLEN 0xf4
#define rwMEMHINT_2DFONT_GLOBAL 0x40199      /* rwMEMHINTDUR_GLOBAL | rwID_2DFONT */
#define rwMEMHINT_2DPLUGIN_GLOBAL 0x40190      /* rwMEMHINTDUR_GLOBAL | rwID_2DPLUGIN */
#define rwMEMHINT_2DPLUGIN_GLOBAL_RESIZABLE 0x1040190      /* | rwMEMHINTFLAG_RESIZABLE */

typedef void *(*RwMallocFn)(size_t size, RwUInt32 hint);
typedef void *(*RwReallocFn)(void *mem, size_t size, RwUInt32 hint);
typedef size_t (*RwStrlenFn)(const RwChar *s);
typedef RwChar *(*RwStrcpyFn)(RwChar *dst, const RwChar *src);

/*007d2128*/ extern void *RwEngineInstance;
/*00947b60*/ extern Rt2dGlobalVars Rt2dGlobals;
/*0063a438*/ extern RwInt32 _rt2dFontFreeListBlockSize;
/*0063a43c*/ extern RwInt32 _rt2dFontFreeListPreallocBlocks;
/*0063a440*/ extern RwInt32 _rt2dFontDictNodeFreeListBlockSize;
/*0063a444*/ extern RwInt32 _rt2dFontDictNodeFreeListPreallocBlocks;
/*0063a448*/ extern RwChar s_fonts__0063a448[];      /* "fonts\\" */
#define rt2dFontDefaultPath s_fonts__0063a448
/*007da904*/ extern RwUInt8 _rt2dFontFreeList[0x24];
/*007da928*/ extern RwUInt8 _rt2dFontDictNodeFreeList[0x24];
/*004e92a0*/ extern RwFreeList *RwFreeListCreateAndPreallocateSpace(RwInt32 size, RwInt32 blockSize, RwInt32 alignment, RwInt32 numBlocksToPrealloc, void *freeList, RwUInt32 hint);
/*004d5a20*/ extern RwTexDictionary *RwTexDictionaryCreate(void);
/*005663c0*/ extern Rt2dFont *FontDefaultRead(const RwChar *name);
#define RwMalloc(size, hint) ((*(RwMallocFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWMALLOC))((size), (hint)))
#define RwRealloc(mem, size, hint) ((*(RwReallocFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWREALLOC))((mem), (size), (hint)))
#define rwstrlen(s) ((*(RwStrlenFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRLEN))(s))
#define rwstrcpy(d, s) ((*(RwStrcpyFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRCPY))((d), (s)))

typedef struct Rt2dBBox {
    RwReal x, y, w, h;
} Rt2dBBox;
typedef struct rt2dCharRect {
    RwReal width;                       /* 0x00 */
    RwTexture *tex;                     /* 0x04 */
    RwV2d uv[2];                        /* 0x08 */
    void *path;                         /* 0x18 */
    char charpage;                      /* 0x1c */
} rt2dCharRect;
RW_LAYOUT_ASSERT(rw_assert_bbox_h, RW_OFFSETOF(Rt2dBBox, h) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_charrect_size, sizeof(rt2dCharRect) == 0x20);

#define RW_GLOBALS_RWFREE 0x10c
#define RW_GLOBALS_STRCAT 0xd4
#define RW_GLOBALS_STRCMP 0xe8
#define rwMEMHINT_2DPLUGIN_FUNCTION 0x10190      /* rwMEMHINTDUR_FUNCTION | rwID_2DPLUGIN */
typedef void (*RwFreeFn)(void *mem);
typedef RwChar *(*RwStrcatFn)(RwChar *dst, const RwChar *src);
typedef int (*RwStrcmpFn)(const RwChar *a, const RwChar *b);
#define RwFree(mem) ((*(RwFreeFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWFREE))(mem))
#define rwstrcat(d, s) ((*(RwStrcatFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRCAT))((d), (s)))
#define rwstrcmp(a, b) ((*(RwStrcmpFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_STRCMP))((a), (b)))

/*005d6010*/ extern void *_rwSListGetArray(void *sList);
/*00565c80*/ extern Rt2dFont *FontCreate2d(void);
/*005662e0*/ extern RwBool Rt2dFontDestroy(Rt2dFont *font);

/*005653f0*/ extern Rt2dFont *rt2dFontShowBitmap(Rt2dFont *font, const RwChar *string, RwReal height, RwV2d *anchor, Rt2dBrush *brush);
typedef struct RwTextureHead {
    void *raster;                       /* 0x00 RwTexture.raster */
    RwUInt8 pad04[0x4c];                /* dict, link, name, mask */
    RwUInt32 filterAddressing;          /* 0x50 */
} RwTextureHead;
RW_LAYOUT_ASSERT(rw_assert_tex_filter, RW_OFFSETOF(RwTextureHead, filterAddressing) == 0x50);
typedef struct rt2dShadeParameters {
    RwRGBAReal col;
    RwV2d uv;
} rt2dShadeParameters;
struct Rt2dBrush {
    rt2dShadeParameters top;            /* 0x00 */
    rt2dShadeParameters dtop;           /* 0x18 */
    rt2dShadeParameters bottom;         /* 0x30 */
    rt2dShadeParameters dbottom;        /* 0x48 */
    RwRGBA colorCache;                  /* 0x60 */
    RwInt32 flag;                       /* 0x64 */
    RwTexture *texture;                 /* 0x68 */
};
RW_LAYOUT_ASSERT(rw_assert_brush_dbottom, RW_OFFSETOF(Rt2dBrush, dbottom) == 0x48);
RW_LAYOUT_ASSERT(rw_assert_brush_colorCache, RW_OFFSETOF(Rt2dBrush, colorCache) == 0x60);
RW_LAYOUT_ASSERT(rw_assert_brush_flag, RW_OFFSETOF(Rt2dBrush, flag) == 0x64);
RW_LAYOUT_ASSERT(rw_assert_brush_texture, RW_OFFSETOF(Rt2dBrush, texture) == 0x68);
RW_LAYOUT_ASSERT(rw_assert_vertex_size, sizeof(RwIm3DVertex) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_charrect_path, RW_OFFSETOF(rt2dCharRect, path) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_charrect_charpage, RW_OFFSETOF(rt2dCharRect, charpage) == 0x1c);

/* FontStrGetChar / FontCharGetCode of font.h */
#define FontStrGetChar(_c, _s, _m, _i) \
    (_c) = ((RwUInt32)((_s)[0] | (((_s)[(_i)] & (_m)) << 8)))
#define FontCharGetCode(_code, _c, _font) \
    if ((_c) >= 128) \
    { \
        (_code) = (_c) - (_font)->uCharOffset; \
        if (((_code) >= 0) && ((_code) < (_font)->uCharIndexCount)) \
            (_code) = (RwInt32)(_font)->uCharIndex[(_code)]; \
        else \
            (_code) = -1; \
    } \
    else \
        (_code) = (RwInt32)(_font)->aCharIndex[(_c)]

extern void *memset(void *dest, int c, size_t count);
#define rwMEMHINT_2DPLUGIN_EVENT 0x30190      /* rwMEMHINTDUR_EVENT | rwID_2DPLUGIN */
#define rwID_2DPATH 0x1a1


/* rt2dFontShowBitmap helpers: exe addresses of the rt2d CTM / font width / Im3D calls */
/*005632e0*/ extern void Rt2dCTMPush(void);
/*00563340*/ extern void Rt2dCTMPop(void);
/*00563370*/ extern void Rt2dCTMScale(RwReal x, RwReal y);
/*005633c0*/ extern void Rt2dCTMTranslate(RwReal x, RwReal y);
/*00563410*/ extern RwMatrix *_rt2dCTMGet(void);
/*00565f80*/ extern RwReal Rt2dFontGetStringWidth(Rt2dFont *font, const RwChar *string, RwReal height);
#define rwIM3D_VERTEXUV_ 1
#define rwIM3D_NOCLIP_ 4
#define FIELDRGBA 1
#define FIELDUV 2
#define rtFONTFLAG_UNICODE 2
#define MAXVINDEX 384
#define RwIm3DVertexSetColor(v, r, g, b, a) ((v)->color = ((RwUInt32)(a) << 24) | ((RwUInt32)(r) << 16) | ((RwUInt32)(g) << 8) | (RwUInt32)(b))

/* RwFastRealToUInt32 (RenderWare's inline real -> integer conversion): the exe has exactly these eight instructions inlined 16 times.
 * A 32-bit fistp under chop rounding: NaN and values outside the int range give 0x80000000 (an (int) cast goes through _ftol, a 64-bit
 * fistp whose low half is kept), the float argument is read from memory (so every colour sum is rounded to float first), and the
 * control word is saved, set and reloaded around it.  The NOASM branch is the portable form of the same result. */
#if !defined(_MSC_VER) || !defined(_M_IX86)
#define NOASM
#endif
#define RwFastRealToUInt32 RwFastRealToUInt32_r      /* _r: a helper of this file (tier 2 is built /Ob0, so it is called, not inlined) */
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

/* Vertex colour = base + delta * scale, truncated to bytes (the rt2d shade macros of the source, written out) */
#define FONT_SHADE(vtx, base, delta, scale) \
    do \
    { \
        sc = (scale); \
        sp.col.red = (delta).col.red * sc; \
        sp.col.green = (delta).col.green * sc; \
        sp.col.blue = (delta).col.blue * sc; \
        sp.col.alpha = (delta).col.alpha * sc; \
        sp.col.red = (base).col.red + sp.col.red; \
        sp.col.green = (base).col.green + sp.col.green; \
        sp.col.blue = (base).col.blue + sp.col.blue; \
        sp.col.alpha = (base).col.alpha + sp.col.alpha; \
        red = RwFastRealToUInt32(sp.col.red); \
        green = RwFastRealToUInt32(sp.col.green); \
        blue = RwFastRealToUInt32(sp.col.blue); \
        alpha = RwFastRealToUInt32(sp.col.alpha); \
        RwIm3DVertexSetColor(vtx, (RwUInt8)red, (RwUInt8)green, (RwUInt8)blue, (RwUInt8)alpha); \
    } while (0)

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
Rt2dFont *rt2dFontShowBitmap(Rt2dFont *font, const RwChar *string, RwReal height, RwV2d *anchor, Rt2dBrush *brush)
{
    RwIm3DVertex *vdst;
    RwReal x, y;
    RwReal baseu;
    RwReal oobaseu;
    RwReal layerDepth;
    RwTexture *currTex;
    RwInt32 vindex = 0, character, charCode, charStride, uCharOffset;
    const RwUInt8 *tmpstring;
    RwUInt8 charMask;
    rt2dCharRect *charMap;

    layerDepth = Rt2dGlobals.layerDepth;

    if (font)
    {
        RwMatrix *ctm;

        charMap = (rt2dCharRect *)_rwSListGetArray(font->charMap);

        if (font->flag & rtFONTFLAG_UNICODE)
        {
            charStride = 2;
            charMask = (RwUInt8)0xFF;
            uCharOffset = 1;
        }
        else
        {
            charStride = 1;
            charMask = (RwUInt8)0x0;
            uCharOffset = 0;
        }

        tmpstring = (const RwUInt8 *)string;

        Rt2dCTMPush();
        Rt2dCTMTranslate(anchor->x, anchor->y);
        Rt2dCTMScale(height, height);
        Rt2dCTMTranslate(((RwReal)0), -font->baseline);

        ctm = _rt2dCTMGet();

        baseu = Rt2dFontGetStringWidth(font, (const RwChar *)string, ((RwReal)1));
        oobaseu = ((RwReal)1) / baseu;
        baseu = ((RwReal)0);

        x = ((RwReal)0);
        y = ((RwReal)0);
        currTex = (RwTexture *)NULL;
        vdst = Rt2dGlobals.brushVertexCache;
        vindex = 0;

        character = (RwUInt32)(tmpstring[0] | ((tmpstring[uCharOffset] & charMask) << 8));

        while (character > 0)
        {
            rt2dCharRect *cr;
            rt2dShadeParameters sp;
            RwUInt32 red, green, blue, alpha;
            RwReal sc;

            FontCharGetCode(charCode, character, font);

            tmpstring += charStride;

            if (charCode >= 0)
            {
                cr = &charMap[charCode];

                if ((cr->tex != currTex) || (vindex > MAXVINDEX - 6))
                {
                    if (vindex)
                    {
                        if (RwIm3DTransform(Rt2dGlobals.brushVertexCache, vdst - Rt2dGlobals.brushVertexCache, ctm, rwIM3D_VERTEXUV_ | rwIM3D_NOCLIP_))
                        {
                            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, (RwImVertexIndex *)Rt2dGlobals.fonttopo, vindex);
                            RwIm3DEnd();
                        }
                    }

                    if (cr->tex != currTex)
                    {
                        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)(mfl_uptr)(((RwTextureHead *)cr->tex)->filterAddressing & 0xff));
                        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, ((RwTextureHead *)cr->tex)->raster);
                        currTex = cr->tex;
                    }

                    vdst = Rt2dGlobals.brushVertexCache;
                    vindex = 0;

                    if (brush->texture && (brush->flag & FIELDUV))
                    {
                        brush->flag &= ~FIELDUV;
                    }
                }

                vdst->objVertex.x = x; vdst->objVertex.y = y; vdst->objVertex.z = layerDepth;
                vdst->u = cr->uv[0].x;
                vdst->v = cr->uv[1].y;
                if (brush->flag & FIELDRGBA)
                {
                    FONT_SHADE(vdst, brush->bottom, brush->dbottom, baseu * oobaseu);
                }
                else
                {
                    RwIm3DVertexSetColor(vdst, (RwUInt8)brush->colorCache.red, (RwUInt8)brush->colorCache.green, (RwUInt8)brush->colorCache.blue, (RwUInt8)brush->colorCache.alpha);
                }
                vdst++;

                vdst->objVertex.x = x; vdst->objVertex.y = y + ((RwReal)1); vdst->objVertex.z = layerDepth;
                vdst->u = cr->uv[0].x;
                vdst->v = cr->uv[0].y;
                if (brush->flag & FIELDRGBA)
                {
                    FONT_SHADE(vdst, brush->top, brush->dtop, baseu * oobaseu);
                }
                else
                {
                    RwIm3DVertexSetColor(vdst, (RwUInt8)brush->colorCache.red, (RwUInt8)brush->colorCache.green, (RwUInt8)brush->colorCache.blue, (RwUInt8)brush->colorCache.alpha);
                }
                vdst++;

                vdst->objVertex.x = x + cr->width; vdst->objVertex.y = y; vdst->objVertex.z = layerDepth;
                vdst->u = cr->uv[1].x;
                vdst->v = cr->uv[1].y;
                if (brush->flag & FIELDRGBA)
                {
                    FONT_SHADE(vdst, brush->bottom, brush->dbottom, (baseu + cr->width) * oobaseu);
                }
                else
                {
                    RwIm3DVertexSetColor(vdst, (RwUInt8)brush->colorCache.red, (RwUInt8)brush->colorCache.green, (RwUInt8)brush->colorCache.blue, (RwUInt8)brush->colorCache.alpha);
                }
                vdst++;

                vdst->objVertex.x = x + cr->width; vdst->objVertex.y = y + ((RwReal)1); vdst->objVertex.z = layerDepth;
                vdst->u = cr->uv[1].x;
                vdst->v = cr->uv[0].y;
                if (brush->flag & FIELDRGBA)
                {
                    FONT_SHADE(vdst, brush->top, brush->dtop, (baseu + cr->width) * oobaseu);
                }
                else
                {
                    RwIm3DVertexSetColor(vdst, (RwUInt8)brush->colorCache.red, (RwUInt8)brush->colorCache.green, (RwUInt8)brush->colorCache.blue, (RwUInt8)brush->colorCache.alpha);
                }
                vdst++;

                vindex += 6;

                x += font->intergap;      /* exe order: (x + intergap) + width, no rounding in between; one expression lets cl regroup it as x + (intergap + width) */
                x += cr->width;
                baseu = x;      /* the exe repeats the sum on baseu; both start at 0, so they are always equal (one sum keeps cl from regrouping it) */
            }
            character = (RwUInt32)(tmpstring[0] | ((tmpstring[uCharOffset] & charMask) << 8));
        }

        if (vindex)
        {
            if (RwIm3DTransform(Rt2dGlobals.brushVertexCache, vdst - Rt2dGlobals.brushVertexCache, ctm, rwIM3D_VERTEXUV_ | rwIM3D_NOCLIP_))
            {
                RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, (RwImVertexIndex *)Rt2dGlobals.fonttopo, vindex);
                RwIm3DEnd();
            }
        }

        anchor->x += x * height;

        Rt2dCTMPop();
    }

    return font;
}
