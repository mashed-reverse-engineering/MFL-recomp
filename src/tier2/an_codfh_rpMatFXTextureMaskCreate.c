/* rpMatFXTextureMaskCreate (MFL.exe 0054ee50). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper GenBumpedTextureName inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT
 * dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data
 * and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwRasterType
{
    rwRASTERTYPENORMAL = 0x00,
    rwRASTERTYPEZBUFFER = 0x01,
    rwRASTERTYPECAMERA = 0x02,
    rwRASTERTYPETEXTURE = 0x04,
    rwRASTERTYPECAMERATEXTURE = 0x05,
    rwRASTERTYPEMASK = 0x07,

    rwRASTERPALETTEVOLATILE = 0x40,
    rwRASTERDONTALLOCATE = 0x80,
    rwRASTERTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterType RwRasterType;
enum RwRasterFormat
{
    rwRASTERFORMATDEFAULT = 0x0000,

    rwRASTERFORMAT1555 = 0x0100,
    rwRASTERFORMAT565 = 0x0200,
    rwRASTERFORMAT4444 = 0x0300,
    rwRASTERFORMATLUM8 = 0x0400,
    rwRASTERFORMAT8888 = 0x0500,
    rwRASTERFORMAT888 = 0x0600,
    rwRASTERFORMAT16 = 0x0700,
    rwRASTERFORMAT24 = 0x0800,
    rwRASTERFORMAT32 = 0x0900,
    rwRASTERFORMAT555 = 0x0a00,

    rwRASTERFORMATAUTOMIPMAP = 0x1000,

    rwRASTERFORMATPAL8 = 0x2000,
    rwRASTERFORMATPAL4 = 0x4000,

    rwRASTERFORMATMIPMAP = 0x8000,

    rwRASTERFORMATPIXELFORMATMASK = 0x0f00,

    rwRASTERFORMATMASK = 0xff00 ,
    rwRASTERFORMATFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFormat RwRasterFormat;
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
#define RwRasterGetWidthMacro(_raster) ((_raster)->width)
#define RwRasterGetHeightMacro(_raster) ((_raster)->height)
#define RwRasterGetFormatMacro(_raster) ((((_raster)->cFormat) & (rwRASTERFORMATMASK >> 8)) << 8)
#define RwRasterGetWidth(_raster) RwRasterGetWidthMacro(_raster)
#define RwRasterGetHeight(_raster) RwRasterGetHeightMacro(_raster)
#define RwRasterGetFormat(_raster) RwRasterGetFormatMacro(_raster)
extern RwRaster *RwRasterCreate(RwInt32 width, RwInt32 height,
                                   RwInt32 depth, RwInt32 flags);
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
#define RwImageGetStrideMacro(_image) ((_image)->stride)
#define RwImageGetPixelsMacro(_image) ((_image)->cpPixels)
#define RwImageGetStride(_image) RwImageGetStrideMacro(_image)
#define RwImageGetPixels(_image) RwImageGetPixelsMacro(_image)
extern RwImage *RwImageCreate(RwInt32 width, RwInt32 height,
                                  RwInt32 depth);
extern RwBool RwImageDestroy(RwImage * image);
extern RwImage *RwImageAllocatePixels(RwImage * image);
extern RwImage *RwImageApplyMask(RwImage * image,
                                     const RwImage * mask);
extern RwImage *RwImageMakeMask(RwImage * image);
#define rwTEXTUREBASENAMELENGTH 32
#define rwTEXTUREFILTERMODEMASK 0x000000FF
#define rwTEXTUREADDRESSINGUMASK 0x00000F00
#define rwTEXTUREADDRESSINGVMASK 0x0000F000
#define rwTEXTUREADDRESSINGMASK (rwTEXTUREADDRESSINGUMASK | rwTEXTUREADDRESSINGVMASK)
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
#define RwTextureSetFilterModeMacro(_tex,_filtering) (((_tex)->filterAddressing = ((_tex)->filterAddressing & ~rwTEXTUREFILTERMODEMASK) | (((RwUInt32)(_filtering)) & rwTEXTUREFILTERMODEMASK)), (_tex))
#define RwTextureGetFilterModeMacro(_tex) ((RwTextureFilterMode)((_tex)->filterAddressing & rwTEXTUREFILTERMODEMASK))
#define RwTextureSetAddressingMacro(_tex,_addressing) (((_tex)->filterAddressing = ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGMASK) | (((((RwUInt32)(_addressing)) << 8) & rwTEXTUREADDRESSINGUMASK) | ((((RwUInt32)(_addressing)) << 12) & rwTEXTUREADDRESSINGVMASK))), (_tex))
#define RwTextureGetAddressingMacro(_tex) (((((_tex)->filterAddressing & rwTEXTUREADDRESSINGUMASK) >> 8) == (((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) ? ((RwTextureAddressMode)(((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) : rwTEXTUREADDRESSNATEXTUREADDRESS)
#define RwTextureGetRaster(_tex) RwTextureGetRasterMacro(_tex)
#define RwTextureSetFilterMode(_tex,_filtering) RwTextureSetFilterModeMacro(_tex, _filtering)
#define RwTextureGetFilterMode(_tex) RwTextureGetFilterModeMacro(_tex)
#define RwTextureSetAddressing(_tex,_addressing) RwTextureSetAddressingMacro(_tex, _addressing)
#define RwTextureGetAddressing(_tex) RwTextureGetAddressingMacro(_tex)
extern RwTexture *RwTextureSetName(RwTexture * texture,
                                     const RwChar * name);
extern RwTexture *RwTextureCreate(RwRaster * raster);
extern RwImage *RwImageResample(RwImage *dstImage, const RwImage *srcImage);
extern RwImage *RwImageSetFromRaster(RwImage *image, RwRaster *raster);
extern RwRaster *RwRasterSetFromImage(RwRaster *raster, RwImage *image);
extern RwImage *RwImageFindRasterFormat(RwImage *ipImage,RwInt32 nRasterType,
                                        RwInt32 *npWidth,RwInt32 *npHeight,
                                        RwInt32 *npDepth,RwInt32 *npFormat);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwImage, palette) == 0x18);      /* an_codfh_rtcharse.c */
RW_LAYOUT_ASSERT(rw37_row_5, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_6, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_7, sizeof(RwImage) == 0x1c);      /* exe: exe static imageTKList.sizeOfStruct at 0x0062e848 */
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RwTexture * rpMatFXTextureMaskCreate(const RwTexture * baseTexture, const RwTexture * maskTexture)
{
    RwRaster           *baseRaster = (RwRaster *)NULL;
    RwRaster           *maskRaster;
    RwRaster           *newRaster;

    RwInt32             maskX, maskY;
    RwInt32             baseX, baseY;

    RwImage            *maskImage;
    RwImage            *baseImage;

    RwTexture          *result;

    RwTextureFilterMode filterMode;
    RwTextureAddressMode addrsMode;

    maskRaster = RwTextureGetRaster(maskTexture);

    maskX = RwRasterGetWidth(maskRaster);
    maskY = RwRasterGetHeight(maskRaster);
    maskImage = RwImageCreate(maskX, maskY, 32);

    RwImageAllocatePixels(maskImage);
    RwImageSetFromRaster(maskImage, maskRaster);

    if (baseTexture)
    {
        baseRaster = RwTextureGetRaster(baseTexture);

        baseX = RwRasterGetWidth(baseRaster);
        baseY = RwRasterGetHeight(baseRaster);
        baseImage = RwImageCreate(baseX, baseY, 32);

        RwImageAllocatePixels(baseImage);
        RwImageSetFromRaster(baseImage, baseRaster);
    }
    else
    {
        RwInt32             x, y;

        baseX = RwRasterGetWidth(maskRaster);
        baseY = RwRasterGetHeight(maskRaster);
        baseImage = RwImageCreate(baseX, baseY, 32);

        RwImageAllocatePixels(baseImage);

        for (y = 0; y < baseY; ++y)
        {
            for (x = 0; x < baseX; ++x)
            {
                *(RwUInt32 *) (RwImageGetPixels(baseImage) +
                               RwImageGetStride(baseImage) * y +
                               x * 4) = 0xFfFfFfFf;
            }
        }
    }

    if ((baseX != maskX) || (baseY != maskY))
    {
        RwImage            *resampleImage;

        resampleImage = RwImageCreate(baseX, baseY, 32);

        RwImageAllocatePixels(resampleImage);
        RwImageResample(resampleImage, maskImage);
        RwImageDestroy(maskImage);

        maskImage = resampleImage;
    }

    RwImageMakeMask(maskImage);
    RwImageApplyMask(baseImage, maskImage);

    {
        RwInt32             rasterWidth, rasterHeight;
        RwInt32             rasterDepth, rasterFlags, rasterBaseFlags;

        RwImageFindRasterFormat(baseImage, rwRASTERTYPETEXTURE,
                                &rasterWidth, &rasterHeight,
                                &rasterDepth, &rasterFlags);

        if (baseTexture)
        {
            rasterBaseFlags = RwRasterGetFormat(baseRaster);
        }
        else
        {
            rasterBaseFlags = RwRasterGetFormat(maskRaster);
        }

        if (rasterBaseFlags & rwRASTERFORMATMIPMAP)
        {
            rasterFlags |= (rwRASTERFORMATAUTOMIPMAP |
                              rwRASTERFORMATMIPMAP);
        }

        newRaster = RwRasterCreate(rasterWidth, rasterHeight,
                                   rasterDepth, rasterFlags);

        RwRasterSetFromImage(newRaster, baseImage);
    }

    result = RwTextureCreate(newRaster);

    if (baseTexture)
    {
        filterMode = RwTextureGetFilterMode(baseTexture);
        addrsMode = RwTextureGetAddressing(baseTexture);
    }
    else
    {
        filterMode = RwTextureGetFilterMode(maskTexture);
        addrsMode = RwTextureGetAddressing(maskTexture);
    }

    RwTextureSetAddressing(result, addrsMode);
    RwTextureSetFilterMode(result, filterMode);

    RwImageDestroy(baseImage);
    RwImageDestroy(maskImage);

    {
        RwChar              newName[rwTEXTUREBASENAMELENGTH] = { 0 };

        /* GenBumpedTextureName(newName, baseTexture, maskTexture): static in rpmatfx.c, inlined in MFL.exe (0x54f03d..0x54f09b) */
        RwInt32             i = 0, j;
        const RwChar       *oldName[2];
        RwChar             *newPtr;

        newPtr = newName;

        if (baseTexture)
        {
            oldName[0] = baseTexture->name;
            oldName[1] = maskTexture->name;
        }
        else
        {
            oldName[0] = oldName[1] = maskTexture->name;
        }

        while ((i < (rwTEXTUREBASENAMELENGTH - 2))
               && ((*oldName[0] != 0) || (*oldName[1] != 0)))
        {
            for (j = 0; j < 2; ++j)
            {
                if (*oldName[j] != 0)
                {
                    *newPtr++ = *oldName[j]++;
                    ++i;
                }
            }
        }

        *newPtr = 0;

        RwTextureSetName(result, newName);
    }

    return (result);
}
