/* RtCharsetPrintBuffered (MFL.exe 00568cc0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/charse/chrprint.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
size_t __cdecl strlen(const char *);
#define rwstrlen RWSRCGLOBAL(stringFuncs).vecStrlen
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
typedef struct RtCharsetDesc RtCharsetDesc;
struct RtCharsetDesc
{
    RwInt32 width;

    RwInt32 height;

    RwInt32 width_internal;

    RwInt32 height_internal;

    RwInt32 count;

    RwInt32 tilewidth;

    RwInt32 tileheight;

};
extern RwBool RtCharsetBufferFlush(void);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RtCharsetDesc, tilewidth) == 0x14);      /* an_codfh_rtcharse.c */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
/* ---- end of the generated declarations ---- */

struct _RtCharsetBuffer
{
    RwBool              initialised;
    RtCharset          *charSet;
    RwUInt32            numChars;
    RwIm2DVertex       *vertices;
    RwImVertexIndex    *indices;
};
typedef struct _RtCharsetBuffer RtCharsetBuffer;
extern RtCharsetBuffer _rtgBuffer;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(struct _RtCharsetBuffer, numChars) == 8 && RW_OFFSETOF(struct _RtCharsetBuffer, vertices) == 0xc && RW_OFFSETOF(struct _RtCharsetBuffer, indices) == 0x10);      /* MFL.exe 0x568b20 / 0x568c0f / 0x568c03 (RtCharsetBufferFlush): numChars 0x7da9f4, vertices 0x7da9f8, indices 0x7da9fc; _rtgBuffer is at 0x7da9ec */
#define BUFFERNUMCHARS ( 1024 )
typedef RwUInt8     RtGlyphRow;
extern const RtGlyphRow *_rtCharsetAtariFontGet(RtCharsetDesc * desc,
                                                RwInt32 * rasSize);
#define GETCHARSETBITS(_desc_ptr,rasSizePtr) _rtCharsetAtariFontGet((_desc_ptr), (rasSizePtr))
extern RwInt32 CharsetProcessString(const RwChar * string, RwUInt32 length, RtCharset * charSet, RwInt32 x, RwInt32 y, RwIm2DVertex * vertices, RwImVertexIndex * indices, RwImVertexIndex indexOffset, RwBool hideSpaces);
/* TIER2: unvalidated authority=diffemu */
RtCharset * RtCharsetPrintBuffered(RtCharset * charSet, const RwChar * string, RwInt32 x, RwInt32 y, RwBool hideSpaces)
{
    RtCharset          *result = (RwRaster *) NULL;
    RwInt32             numChars = rwstrlen(string);

    if (numChars)
    {
        RwUInt32            length;
        RtCharsetDesc       desc;
        RwInt32             temp;

        GETCHARSETBITS(&desc, &temp);

        if ((NULL != _rtgBuffer.charSet)
            && (charSet != _rtgBuffer.charSet))
        {
            RtCharsetBufferFlush();
        }
        _rtgBuffer.charSet = charSet;

        length = strlen(string);
        while (length > 0)
        {
            if (length >= BUFFERNUMCHARS - _rtgBuffer.numChars)
            {
                length = BUFFERNUMCHARS - _rtgBuffer.numChars;
                _rtgBuffer.numChars +=
                    CharsetProcessString(string,
                                         length,
                                         charSet,
                                         x,
                                         y,
                                         &(_rtgBuffer.
                                           vertices[4 *
                                                    _rtgBuffer.
                                                    numChars]),
                                         &(_rtgBuffer.
                                           indices[6 *
                                                   _rtgBuffer.
                                                   numChars]),
                                         (RwImVertexIndex) (4 *
                                                            _rtgBuffer.
                                                            numChars),
                                         hideSpaces);
                RtCharsetBufferFlush();
                
                _rtgBuffer.charSet = charSet;
                x += length * desc.width;
                string += length;
                length = strlen(string);
            }
            else
            {
                _rtgBuffer.numChars +=
                    CharsetProcessString(string,
                                         length,
                                         charSet,
                                         x,
                                         y,
                                         &(_rtgBuffer.
                                           vertices[4 *
                                                    _rtgBuffer.
                                                    numChars]),
                                         &(_rtgBuffer.
                                           indices[6 *
                                                   _rtgBuffer.
                                                   numChars]),
                                         (RwImVertexIndex) (4 *
                                                            _rtgBuffer.
                                                            numChars),
                                         hideSpaces);
                length = 0;
            }
        }

        if (_rtgBuffer.numChars <= 0)
        {

            _rtgBuffer.charSet = (RwRaster *) NULL;
        }

        result = charSet;
    }

    return (result);
}
