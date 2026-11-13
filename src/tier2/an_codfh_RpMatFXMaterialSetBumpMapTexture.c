/* RpMatFXMaterialSetBumpMapTexture (MFL.exe 005504e0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers MatFXGetData and GenBumpedTextureName inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
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
#define RwRasterGetWidth(_raster) RwRasterGetWidthMacro(_raster)
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
#define RwTextureAddRefMacro(_tex) (((_tex)->refCount++), (_tex))
#define RwTextureGetRaster(_tex) RwTextureGetRasterMacro(_tex)
#define RwTextureAddRef(_tex) RwTextureAddRefMacro(_tex)
extern RwBool RwTextureDestroy(RwTexture * texture);
extern RwTexture *RwTexDictionaryAddTexture(RwTexDictionary * dict,
                                              RwTexture * texture);
extern RwTexture *RwTexDictionaryFindNamedTexture(RwTexDictionary *
                                                    dict,
                                                    const RwChar * name);
extern RwTexDictionary *RwTexDictionaryGetCurrent(void);
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
};
#define RpMaterialGetTextureMacro(_material) ((_material)->texture)
#define RpMaterialGetTexture(_material) RpMaterialGetTextureMacro(_material)
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwRaster, height) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwRaster, width) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_14, sizeof(RwRaster) == 0x34);      /* exe: exe static rasterTKList.sizeOfStruct at 0x0062e718 */
RW_LAYOUT_ASSERT(rw37_row_15, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_16, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
/* ---- end of the generated declarations ---- */

#define rpMATFXALIGNMENT rwFRAMEALIGNMENT
typedef struct rpMatFXMaterialData RWALIGN(rpMatFXMaterialData, rpMATFXALIGNMENT);
struct MatFXDualData
{
    RwTexture          *texture;
    RwBlendFunction     srcBlendMode;
    RwBlendFunction     dstBlendMode;
    
};
struct MatFXBumpMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwTexture *bumpTexture;
    RwReal    coef;
    RwReal    invBumpWidth;
};
enum MatFXPass
{
    rpSECONDPASS = 0,
    rpTHIRDPASS  = 1,
    rpMAXPASS    = 2
};
typedef enum MatFXPass MatFXPass;
typedef struct MatFXBumpMapData RWALIGN(MatFXBumpMapData, rpMATFXALIGNMENT);
typedef struct MatFXEnvMapData RWALIGN(MatFXEnvMapData, rpMATFXALIGNMENT);
struct MatFXEnvMapData
{
    RwFrame   *frame;
    RwTexture *texture;
    RwReal    coef;
    RwBool    useFrameBufferAlpha;
};
typedef struct MatFXUVAnimData RWALIGN(MatFXUVAnimData, rpMATFXALIGNMENT);
struct MatFXUVAnimData
{
    RwMatrix *baseTransform;
    RwMatrix *dualTransform;
};
typedef struct MatFXDualData RWALIGN(MatFXDualData, rpMATFXALIGNMENT);
typedef union MatFXEffectUnion RWALIGN(MatFXEffectUnion, rpMATFXALIGNMENT);
union MatFXEffectUnion
{
    MatFXBumpMapData  bumpMap;
    MatFXEnvMapData   envMap;
    MatFXDualData     dual;
    MatFXUVAnimData   uvAnim;
    
};
typedef struct MatFXEffectData RWALIGN(MatFXEffectData, rpMATFXALIGNMENT);
struct MatFXEffectData
{
    MatFXEffectUnion     data;
    RpMatFXMaterialFlags flag;
    
};
struct rpMatFXMaterialData
{
    MatFXEffectData      data[rpMAXPASS];
    RpMatFXMaterialFlags flags;
};
extern RwInt32 MatFXMaterialDataOffset;
#define MATFXMATERIALGETDATA(material) ((rpMatFXMaterialData **) (((RwUInt8 *)material)+ MatFXMaterialDataOffset))
extern RwTexture *
_rpMatFXSetupBumpMapTexture(const RwTexture *baseTexture,
                            const RwTexture *effectTexture);
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(MatFXBumpMapData, texture) == 4 && RW_OFFSETOF(MatFXBumpMapData, bumpTexture) == 8 && RW_OFFSETOF(MatFXBumpMapData, invBumpWidth) == 0x10);      /* MFL.exe 0x550525 bumpTexture [ebx+8], 0x550538 texture [ebx+4], 0x55054b / 0x550687 invBumpWidth [ebx+0x10] */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(MatFXDualData) <= 0x14 && sizeof(MatFXUVAnimData) <= 0x14);      /* members of the 0x14 byte union MatFXEffectUnion (flag at +0x14, stride 0x18: 0x55050c) */

/* TIER2: unvalidated authority=diffemu */
RpMaterial * RpMatFXMaterialSetBumpMapTexture(RpMaterial * material, RwTexture * bumpTexture)
{
    MatFXBumpMapData   *bumpMapData;

    /* MATFXBUMPMAPGETDATA(material) = MatFXGetData(material, rpMATFXEFFECTBUMPMAP), static in rpmatfx.c, inlined in MFL.exe */
    {
        rpMatFXMaterialData *materialData = *MATFXMATERIALGETDATA(material);
        RwUInt8             pass;

        bumpMapData = (MatFXBumpMapData *)NULL;
        for (pass = 0; pass < rpMAXPASS; ++pass)
        {
            if (materialData->data[pass].flag == rpMATFXEFFECTBUMPMAP)
            {
                bumpMapData = &materialData->data[pass].data.bumpMap;
                break;
            }
        }
    }

    if (NULL != bumpMapData->bumpTexture)
    {
        RwTextureDestroy(bumpMapData->bumpTexture);

        bumpMapData->bumpTexture = (RwTexture *)NULL;
    }

    if (NULL != bumpMapData->texture)
    {
        RwTextureDestroy(bumpMapData->texture);

        bumpMapData->texture = (RwTexture *)NULL;
        bumpMapData->invBumpWidth = 0;
    }

    if (bumpTexture)
    {
        RwBool      dummyTextures;
        RwRaster   *bumpRaster;
        RwTexture  *baseTexture;

        bumpMapData->bumpTexture = bumpTexture;
        (void)RwTextureAddRef(bumpMapData->bumpTexture);

        bumpRaster = RwTextureGetRaster(bumpTexture);

        dummyTextures = (0 == RwRasterGetWidth(bumpRaster));

        baseTexture = RpMaterialGetTexture(material);
        if (!dummyTextures && baseTexture)
        {
            RwRaster   *baseRaster;
            baseRaster = RwTextureGetRaster(baseTexture);

            dummyTextures = (0 == RwRasterGetWidth(baseRaster));
        }

        if (!dummyTextures)
        {
            RwChar bumpedName[rwTEXTUREBASENAMELENGTH] = { 0 };
            RwTexDictionary *dict;
            RwRaster    *bumpedRaster;

            /* GenBumpedTextureName(bumpedName, baseTexture, bumpTexture), static in rpmatfx.c, inlined in MFL.exe */
            {
                RwInt32             i = 0, j;
                const RwChar       *oldName[2];
                RwChar             *newPtr = bumpedName;

                if (baseTexture)
                {
                    oldName[0] = baseTexture->name;
                    oldName[1] = bumpTexture->name;
                }
                else
                {
                    oldName[0] = oldName[1] = bumpTexture->name;
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
            }

            dict = RwTexDictionaryGetCurrent();
            bumpMapData->texture = (RwTexture *) NULL;
            if (dict)
            {
                bumpMapData->texture =
                    RwTexDictionaryFindNamedTexture(dict, bumpedName);
            }

            if (!bumpMapData->texture)
            {
                bumpMapData->texture = _rpMatFXSetupBumpMapTexture(baseTexture,
                                                                   bumpTexture);
                if (!bumpMapData->texture)
                {
                    return ((RpMaterial *) NULL);
                }

                if (dict)
                {
                    RwTexDictionaryAddTexture( dict, bumpMapData->texture );
                }
            }
            else
            {
                (void)RwTextureAddRef(bumpMapData->texture);
            }

            bumpedRaster = RwTextureGetRaster(bumpMapData->texture);

            bumpMapData->invBumpWidth =
                1.0f / ((RwReal) RwRasterGetWidth(bumpedRaster));
        }
    }
    else
    {
        RwTexture  *baseTexture;
        RwRaster   *raster;

        baseTexture = RpMaterialGetTexture(material);

        raster = RwTextureGetRaster(baseTexture);

        bumpMapData->invBumpWidth =
            1.0f / ((RwReal) RwRasterGetWidth(raster));
    }

    return (material);
}
