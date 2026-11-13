/* MatFXOpen (MFL.exe 0054f210). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
extern RwBool
_rpMatFXPipelinesCreate(void);
extern RwMatFXInfo MatFXInfo;
extern RwInt32 _rpMatFXMaterialDataFreeListBlockSize;
extern RwInt32 _rpMatFXMaterialDataFreeListPreallocBlocks;
extern RwFreeList _rpMatFXMaterialDataFreeList;
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(MatFXEffectUnion) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(MatFXEffectData, flag) == 0x14);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_2, sizeof(MatFXEffectData) == 0x18);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(MatFXEnvMapData, coef) == 8);      /* an_codfh_rpmatfx_set.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(rpMatFXMaterialData) == 0x34);      /* exe: MatFXOpen 0x0054f232 `push 0x34` (entry size of the material data free list) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(MatFXBumpMapData, bumpTexture) == 8);      /* MFL.exe 0x54f2e2 / 0x54f2f1 (MatFXMaterialDataClean, esi = &data[pass] + 4): texture at [esi], bumpTexture at [esi+0x4] */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(MatFXDualData, dstBlendMode) == 8);      /* MFL.exe 0x550836 / 0x550839 (RpMatFXMaterialSetDualBlendModes): src to [esi+0x4], dst to [esi+0x8] */
RW_LAYOUT_ASSERT(rw37_own_2, RW_OFFSETOF(MatFXUVAnimData, dualTransform) == 4);      /* MFL.exe 0x54f30c / 0x54f30f (MatFXMaterialDataClean): baseTransform [esi-0x4] and dualTransform [esi] cleared */
RW_LAYOUT_ASSERT(rw37_own_3, RW_OFFSETOF(RwMatFXInfo, MaterialData) == 8);      /* MFL.exe 0x54f23c: the free list goes to 0x7da728; MatFXInfo is at 0x7da720, numInstances at 0x7da724 */
/* TIER2: unvalidated authority=diffemu */
void * MatFXOpen(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{

    if (MatFXInfo.Module.numInstances == 0)
    {
        MatFXInfo.MaterialData =
            RwFreeListCreateAndPreallocateSpace(sizeof(rpMatFXMaterialData),
                             _rpMatFXMaterialDataFreeListBlockSize,
                             rpMATFXALIGNMENT,
                             _rpMatFXMaterialDataFreeListPreallocBlocks,
                             &_rpMatFXMaterialDataFreeList,
                             rwID_MATERIALEFFECTSPLUGIN | rwMEMHINTDUR_GLOBAL);

        if (MatFXInfo.MaterialData == NULL)
        {
            instance = NULL;
            return (instance);
        }

        if (FALSE == _rpMatFXPipelinesCreate())
        {
            instance = NULL;
            return (instance);
        }

    }

    ++MatFXInfo.Module.numInstances;

    return (instance);
}
