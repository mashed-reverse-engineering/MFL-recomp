/* RpMatFXMaterialSetupEnvMap (MFL.exe 00550350). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * four env map setters with the static MatFXGetData inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by
 * rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by
 * differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
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
#define RwTextureAddRefMacro(_tex) (((_tex)->refCount++), (_tex))
#define RwTextureAddRef(_tex) RwTextureAddRefMacro(_tex)
extern RwBool RwTextureDestroy(RwTexture * texture);
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
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwTexture, name) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwTexture, filterAddressing) == 0x50);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpMaterial, color) == 4);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpMaterial, pipeline) == 8);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpMaterial, surfaceProps) == 0xc);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpMaterial, refCount) == 0x18);      /* an_codfh_RpMaterialStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, sizeof(RwTexture) == 0x58);      /* exe: exe static textureTKList.sizeOfStruct at 0x0062e6d0 */
RW_LAYOUT_ASSERT(rw37_row_12, sizeof(RpMaterial) == 0x1c);      /* exe: exe static materialTKList.sizeOfStruct at 0x0062ec44 */
RW_LAYOUT_ASSERT(rw37_row_13, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
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
#define _rpMatFXMaterialPlatformEnvMapSetBlend(material) 
/* MATFXENVMAPGETDATA(material) = MatFXGetData(material, rpMATFXEFFECTENVMAP), static in rpmatfx.c, inlined in MFL.exe */
#define ENVMAPDATAINLINE(_env, _material)                               \
MACRO_START                                                             \
{                                                                       \
    rpMatFXMaterialData *_md = *MATFXMATERIALGETDATA(_material);        \
    RwUInt8             _pass;                                          \
                                                                        \
    (_env) = (MatFXEnvMapData *)NULL;                                   \
    for (_pass = 0; _pass < rpMAXPASS; ++_pass)                         \
    {                                                                   \
        if (_md->data[_pass].flag == rpMATFXEFFECTENVMAP)               \
        {                                                               \
            (_env) = &_md->data[_pass].data.envMap;                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
}                                                                       \
MACRO_STOP
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(MatFXBumpMapData) == 0x14 && sizeof(MatFXDualData) <= 0x14 && sizeof(MatFXUVAnimData) <= 0x14);      /* members of the 0x14 byte union MatFXEffectUnion (flag at +0x14, stride 0x18: 0x55037a) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(MatFXEnvMapData, texture) == 4 && RW_OFFSETOF(MatFXEnvMapData, useFrameBufferAlpha) == 0xc);      /* MFL.exe 0x550398 texture [esi+4], 0x550406 frame [eax], 0x550459 useFrameBufferAlpha [eax+0xc], 0x55048e coef [eax+8] */

/* TIER2: unvalidated authority=diffemu */
RpMaterial * RpMatFXMaterialSetupEnvMap(RpMaterial * material, RwTexture * texture, RwFrame * frame, RwBool useFrameBufferAlpha, RwReal coef)
{
    MatFXEnvMapData    *envMapData;

    /* RpMatFXMaterialSetEnvMapTexture(material, texture), inlined in MFL.exe */
    ENVMAPDATAINLINE(envMapData, material);
    (void)RwTextureAddRef(texture);
    if (NULL != envMapData->texture)
    {
        RwTextureDestroy(envMapData->texture);
        envMapData->texture = (RwTexture *)NULL;
    }
    envMapData->texture = texture;
    if (NULL == material)      /* the setter's result (material) */
    {
        return ((RpMaterial *) NULL);
    }

    /* RpMatFXMaterialSetEnvMapFrame / SetEnvMapFrameBufferAlpha / SetEnvMapCoefficient, inlined in MFL.exe (each returns the material,
     * never NULL here) */
    ENVMAPDATAINLINE(envMapData, material);
    envMapData->frame = frame;

    ENVMAPDATAINLINE(envMapData, material);
    envMapData->useFrameBufferAlpha = useFrameBufferAlpha;
    _rpMatFXMaterialPlatformEnvMapSetBlend(material);

    ENVMAPDATAINLINE(envMapData, material);
    envMapData->coef = coef;

    return (material);
}
