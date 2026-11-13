/* CharsetProcessString (MFL.exe 00568e30). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/charse/chrprint.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * function-local static space[] taken from its MFL.exe address (0x63a524), release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwRect RwRect;
struct RwRect
{
    RwInt32 x;
    RwInt32 y;
    RwInt32 w;
    RwInt32 h;
};
#define RwIm2DVertexSetRecipCameraZ(vert,recipz) ((vert)->rhw = recipz)
#define RwIm2DVertexSetScreenX(vert,scrnx) ((vert)->x = (scrnx))
#define RwIm2DVertexSetScreenY(vert,scrny) ((vert)->y = (scrny))
#define RwIm2DVertexSetScreenZ(vert,scrnz) ((vert)->z = (scrnz))
#define RwIm2DVertexSetU(vert,texU,recipz) ((vert)->u = (texU))
#define RwIm2DVertexSetV(vert,texV,recipz) ((vert)->v = (texV))
#define RwIm2DVertexSetIntRGBA(vert,red,green,blue,alpha) ((vert)->emissiveColor = ((((RwUInt32)(alpha)) << 24) | (((RwUInt32)(red)) << 16) | (((RwUInt32)(green)) << 8) | ((RwUInt32)(blue))))
#define RwIm2DGetNearScreenZMacro() (RWSRCGLOBAL(dOpenDevice).zBufferNear)
#define RwIm2DGetNearScreenZ() RwIm2DGetNearScreenZMacro()
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
#define RwRasterGetWidth(_raster) RwRasterGetWidthMacro(_raster)
#define RwRasterGetHeight(_raster) RwRasterGetHeightMacro(_raster)
#define RwCameraGetRasterMacro(_camera) ((_camera)->frameBuffer)
#define RwCameraGetNearClipPlaneMacro(_camera) ((_camera)->nearPlane)
#define RwCameraGetCurrentCameraMacro() ((RwCamera *)RWSRCGLOBAL(curCamera))
#define RwCameraGetRaster(_camera) RwCameraGetRasterMacro(_camera)
#define RwCameraGetNearClipPlane(_camera) RwCameraGetNearClipPlaneMacro(_camera)
#define RwCameraGetCurrentCamera() RwCameraGetCurrentCameraMacro()
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
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RtCharsetDesc, tilewidth) == 0x14);      /* an_codfh_rtcharse.c */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwRect, w) == 8 && RW_OFFSETOF(RwRect, h) == 0xc);      /* MFL.exe 0x4d7908 / 0x4d790f (RwRasterSubRaster): rect->w -> width, rect->h -> height; x / y at 0 / 4 (0x4d7919 / 0x4d7924) */
/* ---- end of the generated declarations ---- */

#define BASE (' ')
typedef RwUInt8     RtGlyphRow;
extern const RtGlyphRow *_rtCharsetAtariFontGet(RtCharsetDesc * desc,
                                                RwInt32 * rasSize);
#define GETCHARSETBITS(_desc_ptr,rasSizePtr) _rtCharsetAtariFontGet((_desc_ptr), (rasSizePtr))
extern RwChar charsetProcessStringSpace[2];
/* TIER2: unvalidated authority=diffemu */
RwInt32 CharsetProcessString(const RwChar * string, RwUInt32 length, RtCharset * charSet, RwInt32 x, RwInt32 y, RwIm2DVertex * vertices, RwImVertexIndex * indices, RwImVertexIndex indexOffset, RwBool hideSpaces)
{
    /* the function-local static `space` (" ", cleared below when spaces are drawn) is MFL.exe data at 0x63a524 */
    RwChar             *space = charsetProcessStringSpace;
    RwInt32             chHeight, chWidth;
    RwCamera           *camera;
    RwRaster           *dest;
    RwInt32             rasHeight, rasWidth;
    RwReal              yScale, xScale;
    RwRect              rectQuad, rectUV;
    RtCharsetDesc       desc;
    RwUInt32            i, j, index, numDrawnChars;
    RwReal              nearScreenZ, recipCamZ;

    RwReal              fOffsetU;
    RwReal              fOffsetV;

    GETCHARSETBITS(&desc, &rasHeight); 

    chHeight = RwRasterGetHeight((RwRaster *) charSet);
    chWidth = RwRasterGetWidth((RwRaster *) charSet);
    yScale = 1 / ((RwReal) chHeight);
    xScale = 1 / ((RwReal) chWidth);

    fOffsetU = ( 1.0f / ((RwReal) chWidth ) ) / 2.0f;
    fOffsetV = ( 1.0f / ((RwReal) chHeight) ) / 2.0f;

    camera = RwCameraGetCurrentCamera();
    
    dest = RwCameraGetRaster(camera);

    rasHeight = RwRasterGetHeight(dest);
    rasWidth = RwRasterGetWidth(dest);

    nearScreenZ = RwIm2DGetNearScreenZ();
    recipCamZ = (RwReal) 1 / RwCameraGetNearClipPlane(camera);

    if (hideSpaces == FALSE)
    {
        
        space[0] = 0;
    }

    numDrawnChars = 0;

    for (i = 0; i < length; i++)
    {
        if (string[i] != space[0])
        {
            rectQuad.x = x + ( desc.width ) * i;
            rectQuad.y = y;
            
            rectQuad.w = rectQuad.x + desc.width;
            rectQuad.h = y + desc.height;

            if ((rectQuad.y < rasHeight) && (rectQuad.h > 0) &&
                (rectQuad.x < rasWidth) && (rectQuad.w > 0))
            {
                index = (string[i] - BASE);

                rectUV.y = index / desc.tilewidth;
                rectUV.x = index - rectUV.y * desc.tilewidth;
                rectUV.x *= desc.width_internal;
                rectUV.y *= desc.height_internal;

                rectUV.w = rectUV.x + desc.width;
                rectUV.h = rectUV.y + desc.height;

                if (rectQuad.y < 0)
                {
                    rectUV.y = rectUV.y - rectQuad.y;
                    rectQuad.y = 0;
                }
                if (rectQuad.h > (rasHeight - 1))
                {
                    rectUV.h =
                        rectUV.y + ((rasHeight - 1) - rectQuad.y);
                    rectQuad.h = rasHeight - 1;
                }
                if (rectQuad.x < 0)
                {
                    rectUV.x = rectUV.x - rectQuad.x;
                    rectQuad.x = 0;
                }
                if (rectQuad.w > (rasWidth - 1))
                {
                    rectUV.w = rectUV.x + ((rasWidth - 1) - rectQuad.x);
                    rectQuad.w = rasWidth - 1;
                }

                RwIm2DVertexSetU(&(vertices[numDrawnChars * 4 + 0]),
                                 xScale * (rectUV.x) + fOffsetU, 1);
                RwIm2DVertexSetV(&(vertices[numDrawnChars * 4 + 0]),
                                 yScale * (rectUV.y) + fOffsetV, 1);
                RwIm2DVertexSetU(&(vertices[numDrawnChars * 4 + 1]),
                                 xScale * (rectUV.x) + fOffsetU, 1);
                RwIm2DVertexSetV(&(vertices[numDrawnChars * 4 + 1]),
                                 yScale * (rectUV.h) + fOffsetV, 1);
                RwIm2DVertexSetU(&(vertices[numDrawnChars * 4 + 2]),
                                 xScale * (rectUV.w) + fOffsetU, 1);
                RwIm2DVertexSetV(&(vertices[numDrawnChars * 4 + 2]),
                                 yScale * (rectUV.h) + fOffsetV, 1);
                RwIm2DVertexSetU(&(vertices[numDrawnChars * 4 + 3]),
                                 xScale * (rectUV.w) + fOffsetU, 1);
                RwIm2DVertexSetV(&(vertices[numDrawnChars * 4 + 3]),
                                 yScale * (rectUV.y) + fOffsetV, 1);

                RwIm2DVertexSetScreenX(&
                                       (vertices
                                        [numDrawnChars * 4 + 0]),
                                       (RwReal) (rectQuad.x));
                RwIm2DVertexSetScreenY(&
                                       (vertices
                                        [numDrawnChars * 4 + 0]),
                                       (RwReal) (rectQuad.y));
                RwIm2DVertexSetScreenX(&
                                       (vertices
                                        [numDrawnChars * 4 + 1]),
                                       (RwReal) (rectQuad.x));
                RwIm2DVertexSetScreenY(&
                                       (vertices
                                        [numDrawnChars * 4 + 1]),
                                       (RwReal) (rectQuad.h));
                RwIm2DVertexSetScreenX(&
                                       (vertices
                                        [numDrawnChars * 4 + 2]),
                                       (RwReal) (rectQuad.w));
                RwIm2DVertexSetScreenY(&
                                       (vertices
                                        [numDrawnChars * 4 + 2]),
                                       (RwReal) (rectQuad.h));
                RwIm2DVertexSetScreenX(&
                                       (vertices
                                        [numDrawnChars * 4 + 3]),
                                       (RwReal) (rectQuad.w));
                RwIm2DVertexSetScreenY(&
                                       (vertices
                                        [numDrawnChars * 4 + 3]),
                                       (RwReal) (rectQuad.y));

                for (j = 0; j < 4; j++)
                {
                    RwIm2DVertexSetRecipCameraZ(&
                                                (vertices
                                                 [numDrawnChars * 4 +
                                                  j]), recipCamZ);
                    RwIm2DVertexSetScreenZ(&
                                           (vertices
                                            [numDrawnChars * 4 + j]),
                                           nearScreenZ);
                    RwIm2DVertexSetIntRGBA(&
                                           (vertices
                                            [numDrawnChars * 4 + j]),
                                           255, 255, 255, 255);
                }

                indices[numDrawnChars * 6 + 0] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 0);
                indices[numDrawnChars * 6 + 1] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 1);
                indices[numDrawnChars * 6 + 2] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 2);
                indices[numDrawnChars * 6 + 3] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 0);
                indices[numDrawnChars * 6 + 4] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 2);
                indices[numDrawnChars * 6 + 5] = (RwImVertexIndex)
                    (indexOffset + numDrawnChars * 4 + 3);

                numDrawnChars++;
            }
        }
    }

    return (numDrawnChars);
}
