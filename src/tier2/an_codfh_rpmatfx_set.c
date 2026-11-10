/* rpmatfx setters (MFL.exe 0x005506a0 - 0x00550860): RenderWare Graphics 3.7 (Criterion Software), material effects:
 * RpMatFXMaterialSetEnvMapTexture / SetEnvMapFrame / SetEnvMapCoefficient / SetDualBlendModes / SetUVTransformMatrices.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build); the static MatFXGetData inlined as the
 * exe has it (a not-found effect gives NULL and the store goes through it, like the exe); RwTextureAddRef expanded (refCount +0x54);
 * _rpMatFXSetupDualRenderState is the exe's folded empty function stub_return_one (0x4a34a0); the platform blend hooks are empty on
 * D3D9; structs declared here (codfh rpmatfx.h / matfx.h) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rpMAXPASS 2

typedef struct MatFXEnvMapData {
    RwFrame *frame;                     /* 0x00 */
    RwTexture *texture;                 /* 0x04 */
    RwReal coef;                        /* 0x08 */
    RwInt32 fbAlpha;
    RwInt32 useFrameBufferAlpha;
} MatFXEnvMapData;
typedef struct MatFXDualData {
    RwTexture *texture;                 /* 0x00 */
    RwBlendFunction srcBlendMode;       /* 0x04 */
    RwBlendFunction dstBlendMode;       /* 0x08 */
} MatFXDualData;
typedef struct MatFXUVAnimData {
    RwMatrix *baseTransform;            /* 0x00 */
    RwMatrix *dualTransform;            /* 0x04 */
} MatFXUVAnimData;
typedef union MatFXEffectUnion {
    MatFXEnvMapData envMap;
    MatFXDualData dual;
    MatFXUVAnimData uvAnim;
    RwUInt8 raw[0x14];
} MatFXEffectUnion;
typedef struct MatFXEffectData {
    MatFXEffectUnion data;              /* 0x00 */
    RwInt32 flag;                       /* 0x14 */
} MatFXEffectData;
typedef struct rpMatFXMaterialData {
    MatFXEffectData data[rpMAXPASS];
} rpMatFXMaterialData;
RW_LAYOUT_ASSERT(rw_assert_matfx_union, sizeof(MatFXEffectUnion) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_matfx_flag, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_matfx_effect, sizeof(MatFXEffectData) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_matfx_coef, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);

#define RW_TEXTURE_REFCOUNT 0x54      /* RwTexture.refCount (RW 3.7) */

/*007da71c*/ extern RwInt32 MatFXMaterialDataOffset;
/*004d5bf0*/ extern RwBool RwTextureDestroy(RwTexture *texture);
/*004a34a0*/ extern RwInt32 stub_return_one(void *data, RwInt32 state);      /* _rpMatFXSetupDualRenderState, folded */
#define _rpMatFXSetupDualRenderState(dualData, state) stub_return_one((dualData), (state))
#define rwRENDERSTATESRCBLEND 10
#define rwRENDERSTATEDESTBLEND 11

/* MatFXGetData(material, flags), inlined in the exe */
#define MATFX_GET_DATA(out, material, flags)                                        \
    {                                                                               \
        rpMatFXMaterialData *materialData_ =                                        \
            *(rpMatFXMaterialData **)((RwUInt8 *)(material) + MatFXMaterialDataOffset); \
        RwUInt8 pass_;                                                              \
        (out) = NULL;                                                               \
        for (pass_ = 0; pass_ < rpMAXPASS; ++pass_)                                 \
        {                                                                           \
            if (materialData_->data[pass_].flag == (flags))                         \
            {                                                                       \
                (out) = &materialData_->data[pass_].data;                           \
                break;                                                              \
            }                                                                       \
        }                                                                           \
    }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMatFXMaterialSetEnvMapTexture(RpMaterial *material, RwTexture *texture)
{
    MatFXEffectUnion *effect;
    MatFXEnvMapData *envMapData;

    MATFX_GET_DATA(effect, material, rpMATFXEFFECTENVMAP);
    envMapData = &effect->envMap;

    /* Add texture reference. */
    ++*(RwInt32 *)((RwUInt8 *)texture + RW_TEXTURE_REFCOUNT);

    /* Do we already have an envTexture? */
    if (NULL != envMapData->texture)
    {
        RwTextureDestroy(envMapData->texture);
        envMapData->texture = (RwTexture *)NULL;
    }

    envMapData->texture = texture;

    return material;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMatFXMaterialSetEnvMapFrame(RpMaterial *material, RwFrame *frame)
{
    MatFXEffectUnion *effect;

    MATFX_GET_DATA(effect, material, rpMATFXEFFECTENVMAP);
    effect->envMap.frame = frame;

    return material;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMatFXMaterialSetEnvMapCoefficient(RpMaterial *material, RwReal coef)
{
    MatFXEffectUnion *effect;

    MATFX_GET_DATA(effect, material, rpMATFXEFFECTENVMAP);
    effect->envMap.coef = coef;

    return material;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMatFXMaterialSetDualBlendModes(RpMaterial *material, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode)
{
    MatFXEffectUnion *effect;
    MatFXDualData *dualData;

    MATFX_GET_DATA(effect, material, rpMATFXEFFECTDUAL);
    dualData = &effect->dual;

    dualData->srcBlendMode = srcBlendMode;
    dualData->dstBlendMode = dstBlendMode;

    _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATESRCBLEND);
    _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATEDESTBLEND);

    return material;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpMaterial *
RpMatFXMaterialSetUVTransformMatrices(RpMaterial *material, RwMatrix *baseTransform, RwMatrix *dualTransform)
{
    MatFXEffectUnion *effect;

    MATFX_GET_DATA(effect, material, rpMATFXEFFECTUVTRANSFORM);
    effect->uvAnim.baseTransform = baseTransform;
    effect->uvAnim.dualTransform = dualTransform;

    return material;
}
