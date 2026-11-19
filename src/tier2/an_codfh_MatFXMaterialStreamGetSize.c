/* MatFXMaterialStreamGetSize (MFL.exe 0054ff40). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * _rpMatFXStreamSizeTexture written as a macro (MFL.exe inlines it), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN
 * -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
#define rwCHUNKHEADERSIZE (sizeof(RwUInt32)*3)
extern RwUInt32
RwTextureStreamGetSize(const RwTexture *texture);
struct RpMaterial
{
        RwTexture *texture;
        RwRGBA color;
        RxPipeline *pipeline;
        RwSurfaceProperties surfaceProps;
        RwInt16 refCount;
        RwInt16 pad;
};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpMaterial, color) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpMaterial, surfaceProps.ambient) == 0xc);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpMaterial, surfaceProps.diffuse) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpMaterial, surfaceProps.specular) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpMaterial, texture) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
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
#define MATFXMATERIALGETCONSTDATA(material) ((const rpMatFXMaterialData * const *) (((const RwUInt8 *)material)+ MatFXMaterialDataOffset))
/* MFL.exe inlines _rpMatFXStreamSizeTexture (calls RwTextureStreamGetSize directly): a macro here, as diffemu cannot relocate a static helper */
#define _rpMatFXStreamSizeTexture(texture) \
    (sizeof(RwBool) + ((texture) ? RwTextureStreamGetSize(texture) + rwCHUNKHEADERSIZE : 0))
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 MatFXMaterialStreamGetSize(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    const RpMaterial   *material;
    const rpMatFXMaterialData *materialData;

    RwInt32             size;
    RwUInt8             pass;

    material = (const RpMaterial *) object;
    materialData = (const rpMatFXMaterialData *)
        *MATFXMATERIALGETCONSTDATA(material);

    if (NULL == materialData || rpMATFXEFFECTNULL == materialData->flags)
    {
        return (0);
    }

    size = sizeof(RpMatFXMaterialFlags);

    for (pass = 0; pass < rpMAXPASS; ++pass)
    {
        RpMatFXMaterialFlags effect;

        effect = materialData->data[pass].flag;

        size += sizeof(RpMatFXMaterialFlags);

        switch (effect)
        {
            case rpMATFXEFFECTNULL:
            case rpMATFXEFFECTUVTRANSFORM: 
            {
                
                break;
            }
            case rpMATFXEFFECTBUMPMAP:
            {
                
                size += sizeof(RwReal);

                size += _rpMatFXStreamSizeTexture(
                            materialData->data[pass].data.bumpMap.texture);

                size += _rpMatFXStreamSizeTexture(
                            materialData->data[pass].data.bumpMap.bumpTexture);

                break;
            }
            case rpMATFXEFFECTENVMAP:
            {
                
                size += sizeof(RwReal);

                size += sizeof(RwBool);

                size += _rpMatFXStreamSizeTexture(
                            materialData->data[pass].data.envMap.texture);

                break;
            }
            case rpMATFXEFFECTDUAL:
            {
                
                size += 2 * sizeof(RwBlendFunction);

                size += _rpMatFXStreamSizeTexture(
                            materialData->data[pass].data.dual.texture);

                break;
            }
            default:
            {

                break;
            }
        }
    }

    return (size);
}
