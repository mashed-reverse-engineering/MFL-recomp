/* rt2dfont (MFL.exe 0x00565e10..0x00567780): RenderWare Graphics 3.7 (Criterion Software), rt2d font module: _rt2dFontOpen,
 * Rt2dFontFlow, Rt2dFontShow.  (Rt2dFontStreamRead and FontDefaultRead stay original code: they reference trapped functions
 * of tools/delink/rw_trap.txt - rt2dFontShowOutline, RwFgets, FontReadMetrics2d1..3 - which installed C must not.)
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/2d/font.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWFUNCTION / RWASSERT / RWMESSAGE / RWRETURN expanded (release build);
 * RwMalloc / RwRealloc expanded to the RwGlobals memory functions (0x108 / 0x110), rwstrlen / rwstrcpy to the RwGlobals string
 * functions (0xf4 / 0xcc); Rt2dFontShow returns the show callback's result (the exe tail-jumps through fontshowCB);
 * _rt2dFontOpen: Rt2dFontSetPath written out where the exe inlined it, no write to the word after fontReadFunc
 * (the codfh-match 'unknown' field; the exe has none), RwFreeListCreateAndPreallocateSpace and the 3.6 free-list statics
 * by their exe names, Rt2dFontSetReadCallBack as a store to Rt2dGlobals.fontReadFunc;
 * FontDefaultRead: AttemptToOpenFont inlined, the 3.6 file functions (fopen / fgets / fclose) by exe names, no RwOsGetFileInterface checks;
 * the static metrics readers (FontReadMetrics2d1..3) take (font, filename, fp, line) as in the exe; Rt2dFontFlow: format as int, the font.h
 * macros expanded; Rt2dFontStreamRead: rt2dFontOutlineStreamRead / rt2dFontBitmapStreamRead inlined, RwStreamReadReal / Int16 are the
 * same address as RwStreamReadInt32, memset to the intrinsic;
 * the structs declared here (only the parts used) with compile-time layout asserts (values of codfh font.h / gstate.h and the exe).
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

/* TIER2: validated 2026-11-10 authority=diffemu */
Rt2dFont *Rt2dFontShow(Rt2dFont *font, const RwChar *string, RwReal height, RwV2d *anchor, Rt2dBrush *brush)
{
    return font->fontshowCB(font, string, height, anchor, brush);
}

/* TIER2: validated 2026-11-10 authority=diffemu */
RwBool _rt2dFontOpen(void)
{
    RwInt32 i;
    short j, *fonttopo;

    if (NULL == (Rt2dGlobals.fontFreeList = RwFreeListCreateAndPreallocateSpace(sizeof(Rt2dFont), _rt2dFontFreeListBlockSize, sizeof(RwUInt32),
        _rt2dFontFreeListPreallocBlocks, _rt2dFontFreeList, rwMEMHINT_2DFONT_GLOBAL)))
        return 0;      /* FALSE */

    if (NULL == (Rt2dGlobals.fontDictNodeFreeList = RwFreeListCreateAndPreallocateSpace(12, _rt2dFontDictNodeFreeListBlockSize,
        sizeof(RwUInt32), _rt2dFontDictNodeFreeListPreallocBlocks, _rt2dFontDictNodeFreeList, rwMEMHINT_2DPLUGIN_GLOBAL)))
        return 0;      /* FALSE */

    /* Font Path, default size 81 chars */
    if (NULL == (Rt2dGlobals.fontPath = (RwChar *)RwMalloc(81, rwMEMHINT_2DPLUGIN_GLOBAL_RESIZABLE)))
        return 0;      /* FALSE */

    Rt2dGlobals.fontPathSize = 81;

    /* Rt2dFontSetPath("fonts\\") */
    {
        RwUInt32 l = rwstrlen(rt2dFontDefaultPath) + 1;

        if (l > Rt2dGlobals.fontPathSize)
        {
            RwChar *newFontPath = (RwChar *)RwRealloc(Rt2dGlobals.fontPath, l, rwMEMHINT_2DPLUGIN_GLOBAL_RESIZABLE);

            if (NULL == newFontPath)
                goto pathdone;

            Rt2dGlobals.fontPath = newFontPath;
            Rt2dGlobals.fontPathSize = l;
        }
        rwstrcpy(Rt2dGlobals.fontPath, rt2dFontDefaultPath);
    }
pathdone:

    fonttopo = Rt2dGlobals.fonttopo;

    for (i = 0, j = 0; i < rt2dMAXVINDEX; i += 6, j += 4)
    {
        fonttopo[i + 0] = j;
        fonttopo[i + 1] = j + 2;
        fonttopo[i + 2] = j + 3;
        fonttopo[i + 3] = j;
        fonttopo[i + 4] = j + 3;
        fonttopo[i + 5] = j + 1;
    }

    /* Font dictionary stuff */
    Rt2dGlobals.fontDictionary = NULL;
    Rt2dGlobals.fontTexDictionary = RwTexDictionaryCreate();
    Rt2dGlobals.currFontTexDictionary = Rt2dGlobals.fontTexDictionary;

    /* Font tmp index */
    Rt2dGlobals.uCharIndex = NULL;
    Rt2dGlobals.uCharIndexSize = 0;

    /* Font read function */
    Rt2dGlobals.fontReadFunc = (void *)FontDefaultRead;

    return 1;      /* TRUE */
}

/* TIER2: validated 2026-11-10 authority=diffemu */
Rt2dFont *Rt2dFontFlow(Rt2dFont *font, RwChar *string, RwReal height, Rt2dBBox *bbox, RwInt32 format, Rt2dBrush *brush)
{
    RwReal bwidth;
    RwReal justscale;
    rt2dCharRect *charMap;
    RwInt32 character, charCode, charStride, charMask, uCharOffset;

    charMap = (rt2dCharRect *)_rwSListGetArray(font->charMap);

    bwidth = bbox->w / height;

    switch (format)
    {
        default:
        case 0:      /* rt2dJUSTIFYLEFT */
            justscale = ((RwReal)0);
            break;

        case 1:      /* rt2dJUSTIFYCENTER */
            justscale = height * 0.5f;
            break;

        case 2:      /* rt2dJUSTIFYRIGHT */
            justscale = height;
            break;
    }

    if (font)
    {
        RwUInt8 *tmpstring, *substring;
        RwUInt8 *lastwordbreak;
        RwReal width, breakwidth = ((RwReal)0);
        RwV2d anchor;
        RwBool bFirstWhiteSpace;

        /* If the font is multibyte, then convert the input string to wide string */
        if (font->flag & 2)      /* rtFONTFLAG_UNICODE */
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

        tmpstring = (RwUInt8 *)string;
        substring = (RwUInt8 *)tmpstring;
        lastwordbreak = tmpstring;
        bFirstWhiteSpace = 1;
        width = ((RwReal)0);
        anchor.y = bbox->y + bbox->h - height;

        FontStrGetChar(character, tmpstring, charMask, uCharOffset);

        while (character > 0)
        {
            rt2dCharRect *cr;

            FontCharGetCode(charCode, character, font);

            if (bFirstWhiteSpace && (0x20 == character))
            {
                bFirstWhiteSpace = 0;
                lastwordbreak = tmpstring;
                breakwidth = width;
            }
            else
            {
                bFirstWhiteSpace = 1;

                if (0x0A == character)
                {
                    lastwordbreak = tmpstring + charStride;
                    breakwidth = width;
                }
            }

            if (charCode >= 0)
            {
                cr = &charMap[charCode];

                width += cr->width + font->intergap;
            }

            if ((width > bwidth) || (0x0A == character))
            {
                RwInt32 lastCharacter;
                RwUInt8 savech[2];

                /* show substring */
                savech[0] = lastwordbreak[0];
                savech[1] = lastwordbreak[uCharOffset];

                lastwordbreak[0] = (RwChar)(0);
                lastwordbreak[uCharOffset] = (RwChar)(0);

                anchor.x = bbox->x + (bwidth - breakwidth) * justscale;

                font->fontshowCB(font, (RwChar *)substring, height, &anchor, brush);

                lastwordbreak[0] = savech[0];
                lastwordbreak[uCharOffset] = savech[1];

                FontStrGetChar(lastCharacter, lastwordbreak, charMask, uCharOffset);

                /* eat white space */
                while (0x20 == lastCharacter)
                {
                    lastwordbreak += charStride;

                    FontStrGetChar(lastCharacter, lastwordbreak, charMask, uCharOffset);
                }

                substring = lastwordbreak;
                tmpstring = lastwordbreak;

                /* bump to the next line */
                bFirstWhiteSpace = 1;
                width = ((RwReal)0);
                anchor.y -= height;

                if (anchor.y < bbox->y)
                {
                    return NULL;
                }
            }
            else
            {
                tmpstring += charStride;
            }

            /* Assume it is a wide string. The mask will strip the upper 8 bit if not needed */
            FontStrGetChar(character, tmpstring, charMask, uCharOffset);
        }

        anchor.x = bbox->x + (bwidth - width) * justscale;

        font->fontshowCB(font, (RwChar *)substring, height, &anchor, brush);
    }

    return font;
}
