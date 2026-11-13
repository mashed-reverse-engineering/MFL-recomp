/* RwImageCreateResample (MFL.exe 004eb400). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/baresamp.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwImage RwImage;
struct RwImage
{
        RwInt32 flags;

        RwInt32 width;
        RwInt32 height;

        RwInt32 depth;
        RwInt32 stride;

        RwUInt8 *cpPixels;
        RwRGBA *palette;
};
#define RwImageGetWidthMacro(_image) ((_image)->width)
#define RwImageGetHeightMacro(_image) ((_image)->height)
#define RwImageGetDepthMacro(_image) ((_image)->depth)
#define RwImageGetWidth(_image) RwImageGetWidthMacro(_image)
#define RwImageGetHeight(_image) RwImageGetHeightMacro(_image)
#define RwImageGetDepth(_image) RwImageGetDepthMacro(_image)
extern RwImage *RwImageCreate(RwInt32 width, RwInt32 height,
                                  RwInt32 depth);
extern RwBool RwImageDestroy(RwImage * image);
extern RwImage *RwImageAllocatePixels(RwImage * image);
extern RwImage *RwImageFreePixels(RwImage * image);
extern RwImage *RwImageCopy(RwImage * destImage,
                                const RwImage * sourceImage);
extern RwImage *RwImageResample(RwImage *dstImage, const RwImage *srcImage);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwImage, palette) == 0x18);      /* an_codfh_rtcharse.c */
RW_LAYOUT_ASSERT(rw37_row_1, sizeof(RwImage) == 0x1c);      /* exe: exe static imageTKList.sizeOfStruct at 0x0062e848 */
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RwImage * RwImageCreateResample(const RwImage * srcImage, RwInt32 width, RwInt32 height)
{
    RwImage            *dstImage;

    dstImage = RwImageCreate(width, height, 32);
    if (!dstImage)
    {
        return ((RwImage *)NULL);
    }

    if (!RwImageAllocatePixels(dstImage))
    {
        RwImageDestroy(dstImage);
        return ((RwImage *)NULL);
    }

    if (RwImageGetDepth(srcImage) != 32)
    {
        RwImage            *ipUse;

        ipUse =
            RwImageCreate(RwImageGetWidth(srcImage),
                          RwImageGetHeight(srcImage), 32);
        if (!ipUse)
        {
            RwImageFreePixels(dstImage);
            RwImageDestroy(dstImage);
            return ((RwImage *)NULL);
        }

        if (!RwImageAllocatePixels(ipUse))
        {
            RwImageDestroy(ipUse);
            RwImageFreePixels(dstImage);
            RwImageDestroy(dstImage);
            return ((RwImage *)NULL);
        }

        RwImageCopy(ipUse, srcImage);

        if (!RwImageResample(dstImage, ipUse))
        {
            RwImageFreePixels(ipUse);
            RwImageDestroy(ipUse);
            RwImageFreePixels(dstImage);
            RwImageDestroy(dstImage);
            return ((RwImage *)NULL);
        }

        RwImageFreePixels(ipUse);
        RwImageDestroy(ipUse);
    }
    else
    {
        if (!RwImageResample(dstImage, srcImage))
        {
            RwImageFreePixels(dstImage);
            RwImageDestroy(dstImage);
            return ((RwImage *)NULL);
        }
    }

    return (dstImage);
}
