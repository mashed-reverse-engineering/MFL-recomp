/* compile: /G5 */
/* ImageStraightCopy (MFL.exe 0x004de3a0): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/baimage.c, function ImageStraightCopy), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x004de3a0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
typedef int  RwInt32;
typedef unsigned char RwUInt8;
typedef RwInt32 RwBool;
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};
typedef struct RwImage RwImage;
struct RwImage
{
        RwInt32             flags;

        RwInt32             width;  /* Device may have different ideas */
        RwInt32             height; /* internally !! */

        RwInt32             depth;  /* Of referenced image */
        RwInt32             stride;

        RwUInt8            *cpPixels;
        RwRGBA             *palette;
};

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Copying/converting images
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/****************************************************************************
 ImageConvertDepth
 Assumes that the palette in the image is the same as the devices !!!
 On entry   : Destination image
            : Source image
 On exit    : TRUE on success
 */
/****************************************************************************
 ImageStraightCopy
 On entry   : Destination
            : Source
 On exit    : TRUE on success
 */
RwBool
ImageStraightCopy(RwImage * ipDestin, const RwImage * ipSource)
{
    RwInt32             i;
    RwInt32             nSpanLength;
    const RwUInt8      *cpSrc;
    RwUInt8            *cpDst;

    RWFUNCTION(RWSTRING("ImageStraightCopy"));
    RWASSERT(ipDestin);
    RWASSERT(ipSource);

    RWASSERT(ipDestin->width == ipSource->width);
    RWASSERT(ipDestin->height == ipSource->height);

    if ((ipDestin->palette) && (ipSource->palette)
        && (ipSource->depth <= 8))
    {
        /* Copy the palette first */
        memcpy(ipDestin->palette, ipSource->palette,
               sizeof(RwRGBA) * (1 << ipSource->depth));
    }

    /* Copy the pixels */
    nSpanLength = ((ipDestin->depth + 7) >> 3) * ipDestin->width;

    cpSrc = ipSource->cpPixels;
    cpDst = ipDestin->cpPixels;

    for (i = 0; i < ipDestin->height; ++i)
    {
        memcpy(cpDst, cpSrc, nSpanLength);

        cpDst += ipDestin->stride;
        cpSrc += ipSource->stride;
    }

    /* All done */
    RWRETURN(TRUE);
}
