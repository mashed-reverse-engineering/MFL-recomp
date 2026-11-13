/* MatFXMaterialDataClean (MFL.exe 0054f2c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
extern RwBool RwTextureDestroy(RwTexture * texture);
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
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(MatFXBumpMapData, bumpTexture) == 8);      /* MFL.exe 0x54f2e2 / 0x54f2f1 (esi = &data[pass] + 4): texture at [esi], bumpTexture at [esi+0x4] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(MatFXDualData, dstBlendMode) == 8);      /* MFL.exe 0x550836 / 0x550839 (RpMatFXMaterialSetDualBlendModes): src to [esi+0x4], dst to [esi+0x8] */
RW_LAYOUT_ASSERT(rw37_own_2, RW_OFFSETOF(MatFXUVAnimData, dualTransform) == 4);      /* MFL.exe 0x54f30c / 0x54f30f: baseTransform [esi-0x4] and dualTransform [esi] cleared */
/* TIER2: unvalidated authority=diffemu */
rpMatFXMaterialData * MatFXMaterialDataClean(rpMatFXMaterialData * materialData)
{
    RwUInt8             pass;

    for (pass = 0; pass < rpMAXPASS; ++pass)
    {
        switch (materialData->data[pass].flag)
        {
            case rpMATFXEFFECTNULL:
                {
                    
                    break;
                }
            case rpMATFXEFFECTBUMPMAP:
                {
                    MatFXBumpMapData   *bumpMapData;

                    bumpMapData =
                        &(materialData->data[pass].data.bumpMap);

                    if (NULL != bumpMapData->texture)
                    {
                        RwTextureDestroy(bumpMapData->texture);
                    }

                    if (NULL != bumpMapData->bumpTexture)
                    {
                        RwTextureDestroy(bumpMapData->bumpTexture);
                    }

                    break;
                }
            case rpMATFXEFFECTENVMAP:
                {
                    MatFXEnvMapData    *envMapData;

                    envMapData =
                        &(materialData->data[pass].data.envMap);

                    if (NULL != envMapData->texture)
                    {
                        RwTextureDestroy(envMapData->texture);
                    }

                    break;
                }
            case rpMATFXEFFECTDUAL:
                {
                    MatFXDualData      *dualData;

                    dualData = &(materialData->data[pass].data.dual);

                    if (NULL != dualData->texture)
                    {
                        RwTextureDestroy(dualData->texture);
                    }

                    break;
                }
            case rpMATFXEFFECTUVTRANSFORM:
                {
                    MatFXUVAnimData *uvAnimData;

                    uvAnimData = &(materialData->data[pass].data.uvAnim);

                    uvAnimData->baseTransform = (RwMatrix *)NULL;
                    uvAnimData->dualTransform = (RwMatrix *)NULL;

                    break;
                }
            default:
                {

                    break;
                }
        }
    }

    memset(materialData, 0, sizeof(rpMatFXMaterialData));

    return (materialData);
}
