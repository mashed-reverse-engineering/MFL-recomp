/* SkinAtomicRights (MFL.exe 0053ebc0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helpers MFL.exe inlines written as macros, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return),
 * conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names
 * declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwInt32 RwEngineGetPluginOffset(RwUInt32 pluginID);
#define rwID_MATERIALEFFECTSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
#define rwID_TOONPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2e)
typedef struct RpSkin RpSkin;
extern RpSkin *
RpSkinGeometryGetSkin( RpGeometry *geometry );
enum RpSkinType
{
    rpNASKINTYPE = 0,
    rpSKINTYPEGENERIC = 1,
    rpSKINTYPEMATFX = 2,
    rpSKINTYPETOON = 3,

    rpSKINTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpSkinType RpSkinType;
/* ---- end of the generated declarations ---- */

struct SkinGlobalPlatform
{
    RwUInt32 unverifiedSize[1];
};
typedef void * SkinUnaligned;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;
typedef struct SkinSplitData SkinSplitData;
typedef struct SkinMatrixCache SkinMatrixCache;
struct SkinMatrixCache
{
    RwMatrix      *aligned;
    SkinUnaligned  unaligned;
};
typedef struct SkinGlobals SkinGlobals;
struct SkinGlobals
{
    
    RwInt32 engineOffset;
    RwInt32 atomicOffset;
    RwInt32 geometryOffset;

    SkinMatrixCache     matrixCache;
    RwUInt32            numMatrices;

    RwFreeList         *freeList;
    RwModuleInfo        module;

    SkinGlobalPlatform  platform;

    SkinSplitData       *skinSplitData;
};
extern SkinGlobals _rpSkinGlobals;
#define RPSKINGEOMETRYGETDATA(geometryInstance) ((RpSkin **) (((RwUInt8 *)geometryInstance) + _rpSkinGlobals.geometryOffset))
/* recomp/aliases.txt names 0x540570 rpSkinPipelinesAttach */
extern RpAtomic *rpSkinPipelinesAttach(RpAtomic *atomic, RpSkinType type);
#define _rpSkinPipelinesAttach(atomic, type) rpSkinPipelinesAttach((atomic), (type))
/* MFL.exe inlines the static helpers of SkinAtomicRights (SkinAtomicSetup, SkinAtomicAttachBestPipeForAttachedPlugins,
 * Matfx / ToonPluginIsAttached, RpSkinGeometryGetSkin): macros here, as diffemu cannot relocate a call to a static helper */
#define MatfxPluginIsAttached() (RwEngineGetPluginOffset(rwID_MATERIALEFFECTSPLUGIN) != -1)
#define ToonPluginIsAttached() (RwEngineGetPluginOffset(rwID_TOONPLUGIN) != -1)
#define RpSkinGeometryGetSkin(geometry) (*RPSKINGEOMETRYGETDATA(geometry))
/* SkinAtomicAttachBestPipeForAttachedPlugins (release build: no debug message) */
#define SkinAtomicAttachBestPipeForAttachedPlugins(atomic, type)                       \
    {                                                                                  \
        if (!MatfxPluginIsAttached() && ((type) == rpSKINTYPEMATFX))                   \
        {                                                                              \
            (type) = rpSKINTYPEGENERIC;                                                \
        }                                                                              \
        else if (!ToonPluginIsAttached() && ((type) == rpSKINTYPETOON))                \
        {                                                                              \
            (type) = rpSKINTYPEGENERIC;                                                \
        }                                                                              \
        _rpSkinPipelinesAttach((atomic), (type));                                      \
    }
/* SkinAtomicSetup */
#define SkinAtomicSetup(atomic, type)                                                  \
    {                                                                                  \
        RpGeometry *geometry_ = RpAtomicGetGeometry(atomic);                           \
                                                                                       \
        if ((NULL != geometry_) && (NULL != RpSkinGeometryGetSkin(geometry_)))         \
        {                                                                              \
            SkinAtomicAttachBestPipeForAttachedPlugins((atomic), (type));              \
        }                                                                              \
    }
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(SkinGlobals, freeList) == 0x18);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(SkinGlobals, module) == 0x1c);      /* an_codfh_rpskin.c */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool SkinAtomicRights(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__, RwUInt32 extraData)
{
    RpAtomic *atomic;
    RpSkinType type;

    atomic = (RpAtomic *)object;
    
    type = (RpSkinType)extraData;

    SkinAtomicSetup(atomic, type);

    return (TRUE);
}
