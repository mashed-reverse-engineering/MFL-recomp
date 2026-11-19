/* RpMatFXPluginAttach (MFL.exe 0054f0c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * plugin callbacks declared extern at their MFL.exe addresses instead of copied, no _rpMultiTexturePlatformPluginsAttach (absent in
 * MFL.exe), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define __RWUNUSEDRELEASE__ 
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;
    RwReal specular;
    RwReal diffuse;
};
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
#define rwCHUNKHEADERSIZE (sizeof(RwUInt32)*3)
extern RwStream *RwStreamWriteInt32(RwStream *stream, const RwInt32 *ints,
                                    RwUInt32 numBytes);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
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
extern RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                        RwPluginObjectConstructor constructCB,
                                        RwPluginObjectDestructor destructCB,
                                        RwPluginObjectCopy copyCB);
extern RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID,
                                              RwPluginDataChunkReadCallBack readCB,
                                              RwPluginDataChunkWriteCallBack writeCB,
                                              RwPluginDataChunkGetSizeCallBack getSizeCB);
extern RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                           RwPluginObjectConstructor constructCB,
                                           RwPluginObjectDestructor destructCB,
                                           RwPluginObjectCopy copyCB);
extern RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID,
                                                 RwPluginDataChunkReadCallBack readCB,
                                                 RwPluginDataChunkWriteCallBack writeCB,
                                                 RwPluginDataChunkGetSizeCallBack getSizeCB);
extern RwInt32
RpAtomicRegisterPlugin(RwInt32 size,
                       RwUInt32 pluginID,
                       RwPluginObjectConstructor constructCB,
                       RwPluginObjectDestructor destructCB,
                       RwPluginObjectCopy copyCB);
extern RwInt32
RpAtomicRegisterPluginStream(RwUInt32 pluginID,
                             RwPluginDataChunkReadCallBack
                             readCB,
                             RwPluginDataChunkWriteCallBack
                             writeCB,
                             RwPluginDataChunkGetSizeCallBack
                             getSizeCB);
#define rwID_MATERIALEFFECTSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
extern RpAtomic *
RpMatFXAtomicEnableEffects( RpAtomic *atomic );
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
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RwSurfaceProperties) == 12);      /* RpMaterial: surfaceProps 0xc, refCount 0x18 (an_codfh_RpMaterialStreamRead.c) */
/* ---- end of the generated declarations ---- */

typedef struct RwMatFXInfo RwMatFXInfo;
struct RwMatFXInfo
{
        RwModuleInfo Module;
        RwFreeList * MaterialData;
};
extern RwMatFXInfo MatFXInfo;
extern RwUInt32
_rpMatFXStreamSizeTexture(const RwTexture * texture);
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
#define MATFXMATERIALGETCONSTDATA(material) ((const rpMatFXMaterialData * const *) (((const RwUInt8 *)material)+ MatFXMaterialDataOffset))
extern RwBool
_rpMatFXPipelinesCreate(void);
extern RwBool
_rpMatFXPipelinesDestroy(void);
extern RpWorldSector *
_rpMatFXPipelineWorldSectorSetup(RpWorldSector *worldSector);
extern RwBool   
_rpMultiTexturePlatformPluginsAttach(void);
typedef struct MatFXWorldSectorData MatFXWorldSectorData;
struct MatFXWorldSectorData
{
        RwBool              enabled;
};
typedef struct MatFXAtomicData MatFXAtomicData;
struct MatFXAtomicData
{
        RwBool              enabled;
};
extern RwInt32             MatFXMaterialDataOffset;
extern RwMatFXInfo MatFXInfo;
extern RwInt32      MatFXAtomicDataOffset;
extern RwInt32      MatFXWorldSectorDataOffset;
#define MATFXATOMICGETDATA(atomic) ((MatFXAtomicData *)(((RwUInt8 *)atomic)+ MatFXAtomicDataOffset))
#define MATFXATOMICGETCONSTDATA(atomic) ((const MatFXAtomicData *)(((const RwUInt8 *)atomic)+ MatFXAtomicDataOffset))
#define MATFXWORLDSECTORGETDATA(worldSector) ((MatFXWorldSectorData *)(((RwUInt8 *)worldSector)+ MatFXWorldSectorDataOffset))
#define MATFXWORLDSECTORGETCONSTDATA(worldSector) ((const MatFXWorldSectorData *)(((const RwUInt8 *)worldSector)+ MatFXWorldSectorDataOffset))
/* the plugin callbacks: rows of their own, at their MFL.exe addresses (lane aliases; MFL.exe folds the identical atomic and world sector
 * constructor / destructor pairs into one function each) */
extern void * MatFXClose(void *instance, RwInt32 offset, RwInt32 size);
extern void * MatFXOpen(void *instance, RwInt32 offset, RwInt32 size);
extern void * MatFXMaterialConstructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXMaterialDestructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXMaterialCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
extern RwStream * MatFXMaterialStreamRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwStream * MatFXMaterialStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 MatFXMaterialStreamGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern void * MatFXAtomicConstructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXAtomicDestructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXAtomicCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
extern RwStream * MatFXAtomicStreamRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwStream * MatFXAtomicStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 MatFXAtomicStreamGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern void * MatFXWorldSectorConstructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXWorldSectorDestructor(void *object, RwInt32 offset, RwInt32 size);
extern void * MatFXWorldSectorCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
extern RwStream * MatFXWorldSectorStreamRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwStream * MatFXWorldSectorStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 MatFXWorldSectorStreamGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/* TIER2: validated 2026-11-14 authority=diffemu */
RwBool RpMatFXPluginAttach(void)
{
    RwInt32             offset;

    if (RwEngineRegisterPlugin(0,
                               (RwUInt32) rwID_MATERIALEFFECTSPLUGIN,
                               MatFXOpen, MatFXClose) < 0)
    {
        return (FALSE);
    }

    MatFXMaterialDataOffset =
        RpMaterialRegisterPlugin(sizeof(rpMatFXMaterialData *),
                                 (RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                                 MatFXMaterialConstructor,
                                 MatFXMaterialDestructor,
                                 MatFXMaterialCopy);

    if (MatFXMaterialDataOffset < 0)
    {
        return (FALSE);
    }

    offset =
        RpMaterialRegisterPluginStream((RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                                       MatFXMaterialStreamRead,
                                       MatFXMaterialStreamWrite,
                                       MatFXMaterialStreamGetSize);
    if (offset < 0)
    {
        return (FALSE);
    }

    MatFXAtomicDataOffset =
        RpAtomicRegisterPlugin(sizeof(RwBool),
                               (RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                               MatFXAtomicConstructor,
                               MatFXAtomicDestructor, MatFXAtomicCopy);

    if (MatFXAtomicDataOffset < 0)
    {
        return (FALSE);
    }

    offset =
        RpAtomicRegisterPluginStream((RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                                     MatFXAtomicStreamRead,
                                     MatFXAtomicStreamWrite,
                                     MatFXAtomicStreamGetSize);
    if (offset < 0)
    {
        return (FALSE);
    }

    MatFXWorldSectorDataOffset =
        RpWorldSectorRegisterPlugin(sizeof(RwBool),
                                    (RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                                    MatFXWorldSectorConstructor,
                                    MatFXWorldSectorDestructor,
                                    MatFXWorldSectorCopy);

    if (MatFXWorldSectorDataOffset < 0)
    {
        return (FALSE);
    }

    /* D3D9 build (MFL.exe 0x54f1b8..0x54f1c9): no _rpMultiTexturePlatformPluginsAttach, the result is the last registration's */
    offset =
        RpWorldSectorRegisterPluginStream((RwUInt32)rwID_MATERIALEFFECTSPLUGIN,
                                          MatFXWorldSectorStreamRead,
                                          MatFXWorldSectorStreamWrite,
                                          MatFXWorldSectorStreamGetSize);

    return (offset >= 0);
}
