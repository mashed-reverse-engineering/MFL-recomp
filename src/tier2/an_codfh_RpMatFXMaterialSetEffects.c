/* RpMatFXMaterialSetEffects (MFL.exe 005501d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper MatFXMaterialGetData and RpMatFXMaterialSetDualBlendModes inlined by hand (MFL.exe has them inlined),
 * _rpMatFXSetupDualRenderState as the folded stub_return_one of MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define rwID_MATERIALEFFECTSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
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
extern RwMatFXInfo MatFXInfo;
#define _rpMatFXMaterialPlatformInitialize(material) 
extern rpMatFXMaterialData * MatFXMaterialDataClean(rpMatFXMaterialData * materialData);
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RwMatFXInfo, MaterialData) == 8 && RW_OFFSETOF(MatFXDualData, srcBlendMode) == 4 && RW_OFFSETOF(MatFXDualData, dstBlendMode) == 8 && RW_OFFSETOF(rpMatFXMaterialData, flags) == 0x30);      /* MFL.exe 0x5501e3 MatFXInfo.MaterialData at 0x7da728 (MatFXInfo 0x7da720); 0x55030e / 0x550316 dual->dstBlendMode [esi+8], srcBlendMode [esi+4]; 0x550237 flags [esi+0x30] */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(MatFXBumpMapData) == 0x14 && sizeof(MatFXUVAnimData) <= 0x14);      /* MFL.exe: union MatFXEffectUnion is 0x14 bytes (flag at +0x14, 0x5502a6 stride 0x18) */

/* _rpMatFXSetupDualRenderState: empty in the D3D9 build, folded into stub_return_one (0x4a34a0), as in an_codfh_rpmatfx_set.c */
extern RwInt32 stub_return_one(void *data, RwInt32 state);
#define _rpMatFXSetupDualRenderState(dualData, state) stub_return_one((dualData), (state))

/* TIER2: unvalidated authority=diffemu */
RpMaterial * RpMatFXMaterialSetEffects(RpMaterial * material, RpMatFXMaterialFlags flags)
{
    rpMatFXMaterialData *materialData;

    /* MatFXMaterialGetData (static in rpmatfx.c, inlined in MFL.exe): the material's effect data, created on first use */
    materialData = *MATFXMATERIALGETDATA(material);
    if (NULL == materialData)
    {
        materialData = (rpMatFXMaterialData *)
            RwFreeListAlloc(MatFXInfo.MaterialData,
                rwID_MATERIALEFFECTSPLUGIN | rwMEMHINTDUR_EVENT);
        if (NULL == materialData)
        {
            return ((RpMaterial *) NULL);
        }
        memset(materialData, 0, sizeof(*materialData));
        *MATFXMATERIALGETDATA(material) = materialData;
    }

    if ((flags == rpMATFXEFFECTNULL) ||
        ((materialData->flags != rpMATFXEFFECTNULL) &&
         (materialData->flags != flags)))
    {
        MatFXMaterialDataClean(materialData);
    }

    materialData->flags = flags;

    switch (materialData->flags)
    {
        case rpMATFXEFFECTBUMPMAP:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTBUMPMAP;

                return (material);
            }
        case rpMATFXEFFECTENVMAP:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTENVMAP;

                return (material);
            }
        case rpMATFXEFFECTBUMPENVMAP:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTBUMPMAP;
                materialData->data[rpTHIRDPASS].flag =
                    rpMATFXEFFECTENVMAP;

                return (material);
            }
        case rpMATFXEFFECTDUAL:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTDUAL;
                break;
            }
        case rpMATFXEFFECTUVTRANSFORM:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTUVTRANSFORM;

                return (material);
            }
        case rpMATFXEFFECTDUALUVTRANSFORM:
            {
                materialData->data[rpSECONDPASS].flag =
                    rpMATFXEFFECTUVTRANSFORM;
                materialData->data[rpTHIRDPASS].flag =
                    rpMATFXEFFECTDUAL;
                break;
            }
        default:
            {
                return (material);
            }
    }

    /* RpMatFXMaterialSetDualBlendModes(material, rwBLENDSRCALPHA, rwBLENDINVSRCALPHA), inlined in MFL.exe (0x550281..0x55032a):
     * the dual pass of the material, its blend modes, then the platform render state setup */
    {
        rpMatFXMaterialData *dualMaterial = *MATFXMATERIALGETDATA(material);
        MatFXDualData      *dualData = (MatFXDualData *)NULL;
        RwUInt8             pass;

        for (pass = rpSECONDPASS; pass < rpMAXPASS; pass++)
        {
            if (dualMaterial->data[pass].flag == rpMATFXEFFECTDUAL)
            {
                dualData = &dualMaterial->data[pass].data.dual;
                break;
            }
        }

        dualData->srcBlendMode = rwBLENDSRCALPHA;
        dualData->dstBlendMode = rwBLENDINVSRCALPHA;

        _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATESRCBLEND);
        _rpMatFXSetupDualRenderState(dualData, rwRENDERSTATEDESTBLEND);
    }

    return (material);
}
