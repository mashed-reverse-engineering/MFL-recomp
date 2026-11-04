/* compile: /G5 */
/* RwImageMakeMask (MFL.exe 0x004dd700): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/baimage.c, function RwImageMakeMask), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004dd700 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
typedef int  RwInt32;
typedef unsigned char RwUInt8;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)    /* No op */
#define RWASSERT(condition)    /* No op */
typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
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
extern RwImage     *RwImageMakeMask(RwImage * image);
extern RwModuleInfo imageModule;

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Messing around with images
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/**
 * \ingroup rwimage
 * \ref RwImageMakeMask generates an alpha channel for the specified
 * image based on it's pixel color values. The alpha channel is that part of
 * the image controlling transparency and its value for each pixel is
 * calculated here from the color channel values as follows:
 * max(red, green, blue).
 *
 * \param image  A pointer to the image whose alpha channel will be generated.
 *
 * \return Returns a pointer to the image.
 *
 * \see RwImageApplyMask
 * \see RwImageReadMaskedImage
 *
 */
RwImage            *
RwImageMakeMask(RwImage * image)
{
    RwInt32             i;

    RWAPIFUNCTION(RWSTRING("RwImageMakeMask"));
    RWASSERT(imageModule.numInstances);
    RWASSERT(image);

    switch (image->depth)
    {
        case 4:
        case 8:
            {
                RwInt32             palSize = (1 << image->depth);
                RwRGBA             *rpPal = image->palette;

                /* 8 bit image */
                for (i = 0; i < palSize; ++i)
                {
                    RwInt32             nOpacity;

                    /* Opacity is max colour channel value */
                    nOpacity = rpPal[i].red;
                    if (rpPal[i].green > nOpacity)
                    {
                        nOpacity = rpPal[i].green;
                    }
                    if (rpPal[i].blue > nOpacity)
                    {
                        nOpacity = rpPal[i].blue;
                    }

                    rpPal[i].alpha = (RwUInt8) nOpacity;
                }
                break;
            }
        case 32:
            {
                RwUInt8            *cpSpan;
                RwInt32             j;

                /* 32 bit image */
                cpSpan = image->cpPixels;

                for (i = 0; i < image->height; ++i)
                {
                    RwRGBA             *rpCur = (RwRGBA *) cpSpan;

                    for (j = 0; j < image->width; j++)
                    {
                        RwInt32             nOpacity;

                        /* Opacity is max colour channel value */
                        nOpacity = rpCur[j].red;
                        if (rpCur[j].green > nOpacity)
                        {
                            nOpacity = rpCur[j].green;
                        }
                        if (rpCur[j].blue > nOpacity)
                        {
                            nOpacity = rpCur[j].blue;
                        }

                        rpCur[j].alpha = (RwUInt8) nOpacity;
                    }

                    cpSpan += image->stride;

                }
            }
    }

    RWRETURN(image);
}
