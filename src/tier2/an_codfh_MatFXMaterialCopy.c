/* MatFXMaterialCopy (MFL.exe 0054f340). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the effect getters / setters MFL.exe inlines
 * written out in its form (getters through the out-of-line MatFXGetData 0x54f770, setters as an inline pass search; the bump coefficient
 * goes through the negating getter / setter pair; _rpMatFXSetupDualRenderState is the folded stub_return_one), release build macros
 * expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK
 * includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked;
 * compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
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
#define rwID_MATERIALEFFECTSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
extern RpMaterial *
RpMatFXMaterialSetEffects( RpMaterial *material,
                           RpMatFXMaterialFlags flags );
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
/* ---- end of the generated declarations ---- */

typedef struct RwMatFXInfo RwMatFXInfo;
struct RwMatFXInfo
{
        RwModuleInfo Module;
        RwFreeList * MaterialData;
};
extern RwMatFXInfo MatFXInfo;
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
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_priv_5, RW_OFFSETOF(RwMatFXInfo, MaterialData) == 8);      /* exe: 0x54f36a MatFXInfo.MaterialData at 0x7da728 (MatFXInfo 0x7da720) */

/* MatFXGetData stays out of line in MFL.exe (0x54f770): the effect getters call it */
extern MatFXEffectUnion *MatFXGetData(RpMaterial *material, RpMatFXMaterialFlags flags);
/* _rpMatFXSetupDualRenderState is the folded empty function stub_return_one (0x4a34a0), as in an_codfh_rpmatfx_set.c */
extern RwInt32 stub_return_one(void *data, RwInt32 state);
#define _rpMatFXSetupDualRenderState(dualData, state) stub_return_one((dualData), (state))

/* the inlined setters' pass search (MatFXGetData written out): the effect union of the pass whose flag is `effect`, else NULL
 * (MFL.exe then stores through NULL, as the source does) */
#define MATFXSEARCH(res, material, effect)                                      \
    {                                                                          \
        rpMatFXMaterialData *sd_ = *MATFXMATERIALGETDATA(material);            \
        RwUInt8 sp_;                                                           \
                                                                               \
        (res) = (MatFXEffectUnion *)NULL;                                      \
        for (sp_ = 0; sp_ < rpMAXPASS; ++sp_)                                  \
        {                                                                      \
            if (sd_->data[sp_].flag == (effect))                               \
            {                                                                  \
                (res) = &(sd_->data[sp_].data);                                \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void * MatFXMaterialCopy(void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpMaterial          *srcMaterial = (RpMaterial *) srcObject;
    RpMaterial          *dstMaterial = (RpMaterial *) dstObject;
    rpMatFXMaterialData *srcMaterialData;
    rpMatFXMaterialData *dstMaterialData;
    RwUInt8              pass;

    srcMaterialData = *MATFXMATERIALGETDATA(srcMaterial);
    if (NULL == srcMaterialData)
    {
        return (NULL);
    }

    /* MatFXMaterialGetData(dstMaterial), inlined */
    dstMaterialData = *MATFXMATERIALGETDATA(dstMaterial);
    if (NULL == dstMaterialData)
    {
        dstMaterialData = (rpMatFXMaterialData *)
            RwFreeListAlloc(MatFXInfo.MaterialData, rwID_MATERIALEFFECTSPLUGIN | rwMEMHINTDUR_EVENT);
        if (NULL == dstMaterialData)
        {
            return (NULL);
        }
        memset(dstMaterialData, 0, sizeof(*dstMaterialData));
        *MATFXMATERIALGETDATA(dstMaterial) = dstMaterialData;
    }

    RpMatFXMaterialSetEffects(dstMaterial, srcMaterialData->flags);

    for (pass = 0; pass < rpMAXPASS; ++pass)
    {
        switch (srcMaterialData->data[pass].flag)
        {
            case rpMATFXEFFECTBUMPMAP:
            {
                RwFrame *frame;
                RwReal negCoef;      /* RpMatFXMaterialGetBumpMapCoefficient returns -coef, the setter stores -value */
                RwReal coef;
                MatFXBumpMapData *srcBump = &srcMaterialData->data[pass].data.bumpMap;
                MatFXBumpMapData *dstBump = &dstMaterialData->data[pass].data.bumpMap;

                frame = MatFXGetData(srcMaterial, rpMATFXEFFECTBUMPMAP)->bumpMap.frame;
                negCoef = -MatFXGetData(srcMaterial, rpMATFXEFFECTBUMPMAP)->bumpMap.coef;
                MatFXGetData(dstMaterial, rpMATFXEFFECTBUMPMAP)->bumpMap.frame = frame;
                coef = -negCoef;
                MatFXGetData(dstMaterial, rpMATFXEFFECTBUMPMAP)->bumpMap.coef = coef;

                dstBump->texture = srcBump->texture;
                dstBump->bumpTexture = srcBump->bumpTexture;
                if (NULL != dstBump->texture)
                {
                    (void)RwTextureAddRef(dstBump->texture);
                }
                if (NULL != dstBump->bumpTexture)
                {
                    (void)RwTextureAddRef(dstBump->bumpTexture);
                }
                break;
            }
            case rpMATFXEFFECTENVMAP:
            {
                RwTexture *texture;
                RwFrame   *frame;
                RwReal     coef;
                RwBool     useFrameBufferAlpha;
                MatFXEffectUnion *u;

                texture = MatFXGetData(srcMaterial, rpMATFXEFFECTENVMAP)->envMap.texture;
                frame = MatFXGetData(srcMaterial, rpMATFXEFFECTENVMAP)->envMap.frame;
                coef = MatFXGetData(srcMaterial, rpMATFXEFFECTENVMAP)->envMap.coef;
                useFrameBufferAlpha = MatFXGetData(srcMaterial, rpMATFXEFFECTENVMAP)->envMap.useFrameBufferAlpha;

                if (NULL != texture)
                {
                    /* RpMatFXMaterialSetEnvMapTexture, inlined */
                    MatFXEnvMapData *envMap = &MatFXGetData(dstMaterial, rpMATFXEFFECTENVMAP)->envMap;

                    (void)RwTextureAddRef(texture);
                    if (envMap->texture)
                    {
                        RwTextureDestroy(envMap->texture);
                        envMap->texture = (RwTexture *)NULL;
                    }
                    envMap->texture = texture;
                }

                MATFXSEARCH(u, dstMaterial, rpMATFXEFFECTENVMAP);      /* RpMatFXMaterialSetEnvMapFrame */
                u->envMap.frame = frame;
                MATFXSEARCH(u, dstMaterial, rpMATFXEFFECTENVMAP);      /* RpMatFXMaterialSetEnvMapFrameBufferAlpha */
                u->envMap.useFrameBufferAlpha = useFrameBufferAlpha;
                MATFXSEARCH(u, dstMaterial, rpMATFXEFFECTENVMAP);      /* RpMatFXMaterialSetEnvMapCoefficient */
                u->envMap.coef = coef;
                break;
            }
            case rpMATFXEFFECTDUAL:
            {
                RwTexture *texture;
                RwBlendFunction srcBlendMode, dstBlendMode;
                MatFXEffectUnion *u;
                MatFXDualData *dual;

                MATFXSEARCH(u, srcMaterial, rpMATFXEFFECTDUAL);        /* RpMatFXMaterialGetDualTexture */
                texture = u->dual.texture;
                MATFXSEARCH(u, srcMaterial, rpMATFXEFFECTDUAL);        /* RpMatFXMaterialGetDualBlendModes */
                srcBlendMode = u->dual.srcBlendMode;
                dstBlendMode = u->dual.dstBlendMode;

                if (NULL != texture)
                {
                    /* RpMatFXMaterialSetDualTexture, inlined */
                    dual = &MatFXGetData(dstMaterial, rpMATFXEFFECTDUAL)->dual;
                    (void)RwTextureAddRef(texture);
                    if (dual->texture)
                    {
                        RwTextureDestroy(dual->texture);
                        dual->texture = (RwTexture *)NULL;
                    }
                    dual->texture = texture;
                    _rpMatFXSetupDualRenderState(dual, 2);      /* rwRENDERSTATETEXTUREADDRESS (MFL.exe 0x54f66c push 2) */
                    _rpMatFXSetupDualRenderState(dual, 1);      /* rwRENDERSTATETEXTURERASTER (0x54f676 push 1) */
                }

                /* RpMatFXMaterialSetDualBlendModes, inlined */
                dual = &MatFXGetData(dstMaterial, rpMATFXEFFECTDUAL)->dual;
                dual->srcBlendMode = srcBlendMode;
                dual->dstBlendMode = dstBlendMode;
                _rpMatFXSetupDualRenderState(dual, 10);     /* rwRENDERSTATESRCBLEND (0x54f68f push 0xa) */
                _rpMatFXSetupDualRenderState(dual, 11);     /* rwRENDERSTATEDESTBLEND (0x54f69d push 0xb) */
                break;
            }
            case rpMATFXEFFECTUVTRANSFORM:
            {
                RwMatrix *baseTransform, *dualTransform;
                MatFXEffectUnion *u;

                /* RpMatFXMaterialGetUVTransformMatrices, inlined (its NULL tests of the out pointers are always true) */
                u = MatFXGetData(srcMaterial, rpMATFXEFFECTUVTRANSFORM);
                baseTransform = u->uvAnim.baseTransform;
                dualTransform = u->uvAnim.dualTransform;

                MATFXSEARCH(u, dstMaterial, rpMATFXEFFECTUVTRANSFORM); /* RpMatFXMaterialSetUVTransformMatrices */
                u->uvAnim.baseTransform = baseTransform;
                u->uvAnim.dualTransform = dualTransform;
                break;
            }
            default:
                break;
        }
    }

    return (dstObject);
}
