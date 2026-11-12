/* rtcharse (MFL.exe 0x00568810 / 0x005689d0 / 0x00568a90): RenderWare Graphics 3.7 (Criterion Software), raster character set
 * toolkit: RtCharsetCreate, CharsetGlyphSet, RtCharsetOpen.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/charse/rtcharse.c @e578672
 * (+ chrprint.h for _rtgBuffer), not byte-identical, changes: RWAPIFUNCTION / RWFUNCTION / RWASSERT / RWRETURN expanded (release
 * build), RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin rwID_CHARSEPLUGIN 0x180), RwMalloc / RwFree
 * expanded to the RwGlobals memory functions (0x108 / 0x10c); in RtCharsetCreate the exe's inlining written out: the Atari font
 * size loop of _rtCharsetAtariFontGet (91 glyphs, 9 x 15 texels incl. border) twice, RtCharsetSetColors, CharsetImageSet (which
 * calls _rtCharsetAtariFontGet itself) and RtCharsetDestroy (= RwRasterDestroy); memset in CharsetGlyphSet as a byte loop; RwImage /
 * RtCharsetDesc / RtCharsetBuffer declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef RwUInt8 RtGlyphRow;
typedef struct RtCharsetDesc {
    RwInt32 width;
    RwInt32 height;
    RwInt32 width_internal;
    RwInt32 height_internal;
    RwInt32 count;
    RwInt32 tilewidth;
    RwInt32 tileheight;
} RtCharsetDesc;
typedef struct rtImageHead {      /* RwImage (baimage.h) */
    RwInt32 flags;
    RwInt32 width;
    RwInt32 height;
    RwInt32 depth;
    RwInt32 stride;                     /* 0x10 */
    RwUInt8 *cpPixels;                  /* 0x14 */
    RwUInt32 *palette;                  /* 0x18 RwRGBA */
} rtImageHead;
typedef struct RtCharsetBuffer {
    RwBool initialised;
    void *charSet;                      /* RwRaster * */
    RwUInt32 numChars;
    void *vertices;                     /* RwIm2DVertex * */
    void *indices;                      /* RwImVertexIndex * */
} RtCharsetBuffer;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_desc_tilewidth, RW_OFFSETOF(RtCharsetDesc, tilewidth) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_image_palette, RW_OFFSETOF(rtImageHead, palette) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_buffer_indices, RW_OFFSETOF(RtCharsetBuffer, indices) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);

#define rtATARI_COUNT 91                /* sizeof(charSetBits) / sizeof(RtAtariGlyph) */
#define rtATARI_WIDTH_INTERNAL 9        /* rtATARICHARSETWIDTH 8 + CHARSET_TEXEL_BORDER_SIZE */
#define rtATARI_HEIGHT_INTERNAL 15      /* rtATARICHARSETHEIGHT 14 + CHARSET_TEXEL_BORDER_SIZE */
#define rwRASTERTYPETEXTURE 4
#define BUFFERNUMCHARS 1024
#define RW_SIZEOF_IM2DVERTEX 0x1c
#define RW_SIZEOF_IMVERTEXINDEX 2
#define rwID_CHARSEPLUGIN 0x180
#define rwCHARSE_MEMHINT 0x40180        /* rwID_CHARSEPLUGIN | rwMEMHINTDUR_GLOBAL */
#define E_RW_NULLP ((RwInt32)0x80000016)
#define RW_GLOBALS_RWMALLOC 0x108
#define RW_GLOBALS_RWFREE 0x10c

typedef void *(*RwMallocFn)(size_t size, RwUInt32 hint);
typedef void (*RwFreeFn)(void *mem);
/*007d2128*/ extern void *RwEngineInstance;
/*007da9ec*/ extern RtCharsetBuffer _rtgBuffer;
/*00568750*/ extern const RtGlyphRow *_rtCharsetAtariFontGet(RtCharsetDesc *desc, RwInt32 *rasSize);
/*004d7950*/ extern void *RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
/*004d77e0*/ extern RwBool RwRasterDestroy(void *raster);
/*004e7480*/ extern void *RwRasterSetFromImage(void *raster, rtImageHead *image);
/*004dd550*/ extern rtImageHead *RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth);
/*004dd5b0*/ extern RwBool RwImageDestroy(rtImageHead *image);
/*004dd610*/ extern rtImageHead *RwImageAllocatePixels(rtImageHead *image);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
#define RwMalloc(size, hint) ((*(RwMallocFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWMALLOC))((size), (hint)))
#define RwFree(mem) ((*(RwFreeFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWFREE))(mem))

const RtGlyphRow *CharsetGlyphSet(const RtGlyphRow *charBitsIn, RtCharsetDesc *desc, RwUInt8 *charRow, RwInt32 stride);

/* _rtCharsetAtariFontGet's raster size (the part RtCharsetCreate inlines) */
#define ATARI_RAS_SIZE(size)                                                \
    {                                                                       \
        RwInt32 tilewidth_, tileheight_;                                    \
        (size) = 16;                                                        \
        do                                                                  \
        {                                                                   \
            (size) += (size);                                               \
            tilewidth_ = (size) / rtATARI_WIDTH_INTERNAL;                   \
            tileheight_ = (rtATARI_COUNT + tilewidth_ - 1) / tilewidth_;    \
        }                                                                   \
        while ((size) < (tileheight_ * rtATARI_HEIGHT_INTERNAL));          \
    }

/* TIER2: validated 2026-11-10 authority=diffemu */
const RtGlyphRow *CharsetGlyphSet(const RtGlyphRow *charBitsIn, RtCharsetDesc *desc, RwUInt8 *charRow, RwInt32 stride)
{
    RtGlyphRow charPixels;
    RwUInt8 *target;
    RwInt32 i, k;
    RwInt32 height_internal = desc->height_internal;
    RwInt32 width_internal = desc->width_internal;

    for (i = 0; i < height_internal; i++)
    {
        target = charRow;
        charRow += stride;

        for (k = 0; k < width_internal; k++) target[k] = 0;      /* memset(target, 0, width_internal) */

        for (charPixels = (*charBitsIn); charPixels; charPixels >>= 1)
        {
            *target++ = (RwUInt8)(charPixels & 0x01);
        }
        charBitsIn++;
    }
    return charBitsIn;
}

/* TIER2: validated 2026-11-10 authority=diffemu */
void *RtCharsetCreate(const RwUInt32 *foreGround, const RwUInt32 *backGround)
{
    void *result = NULL;

    if (foreGround && backGround)
    {
        RwInt32 rasSize;
        void *charSet;

        ATARI_RAS_SIZE(rasSize);
        charSet = RwRasterCreate(rasSize, rasSize, 0, rwRASTERTYPETEXTURE);

        if (charSet)
        {
            /* RtCharsetSetColors */
            rtImageHead *image;

            ATARI_RAS_SIZE(rasSize);
            image = RwImageCreate(rasSize, rasSize, 8);
            if (image)
            {
                if (RwImageAllocatePixels(image))
                {
                    /* CharsetImageSet */
                    RwUInt8 *charBase;
                    RwInt32 stride, i, j = 0, row = 0, column = 0;
                    RtCharsetDesc desc;
                    const RtGlyphRow *charBitsIn;

                    image->palette[0] = *backGround;
                    image->palette[1] = *foreGround;

                    charBase = image->cpPixels;
                    stride = image->stride;
                    charBitsIn = _rtCharsetAtariFontGet(&desc, &i);
                    while (j++ < desc.count)
                    {
                        RwUInt8 *charRow = &charBase[column * desc.width_internal + stride * row * desc.height_internal];

                        charBitsIn = CharsetGlyphSet(charBitsIn, &desc, charRow, stride);
                        if (desc.tilewidth == ++column)
                        {
                            column = 0;
                            ++row;
                        }
                    }
                    RwRasterSetFromImage(charSet, image);
                    result = charSet;
                }
                RwImageDestroy(image);
            }
            if (!result)
            {
                RwRasterDestroy(charSet);      /* RtCharsetDestroy */
            }
        }
    }
    else
    {
        RwError error;

        error.pluginID = rwID_CHARSEPLUGIN;
        error.errorCode = _rwerror(E_RW_NULLP);
        RwErrorSet(&error);
    }
    return result;
}

/* TIER2: validated 2026-11-10 authority=diffemu */
RwBool RtCharsetOpen(void)
{
    /* Create the triangle buffer */
    _rtgBuffer.vertices = RwMalloc(BUFFERNUMCHARS * 4 * RW_SIZEOF_IM2DVERTEX, rwCHARSE_MEMHINT);
    if (NULL == _rtgBuffer.vertices)
        return 0;      /* FALSE */
    _rtgBuffer.indices = RwMalloc(BUFFERNUMCHARS * 6 * RW_SIZEOF_IMVERTEXINDEX, rwCHARSE_MEMHINT);
    if (NULL == _rtgBuffer.indices)
    {
        RwFree(_rtgBuffer.vertices);
        _rtgBuffer.vertices = NULL;
        return 0;      /* FALSE */
    }

    _rtgBuffer.numChars = 0;
    _rtgBuffer.charSet = NULL;
    _rtgBuffer.initialised = 1;      /* TRUE */
    return 1;      /* TRUE */
}
