/* RpSkinPluginAttach (MFL.exe 0053e870). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define __RWUNUSEDRELEASE__ 
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
extern RwInt32
RpGeometryRegisterPlugin(RwInt32 size,
                         RwUInt32 pluginID,
                         RwPluginObjectConstructor constructCB,
                         RwPluginObjectDestructor destructCB,
                         RwPluginObjectCopy copyCB);
extern RwInt32
RpGeometryRegisterPluginStream(RwUInt32 pluginID,
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
extern RwInt32
RpAtomicSetStreamAlwaysCallBack(RwUInt32 pluginID,
                                     RwPluginDataChunkAlwaysCallBack alwaysCB);
extern RwInt32
RpAtomicSetStreamRightsCallBack(RwUInt32 pluginID,
                                     RwPluginDataChunkRightsCallBack rightsCB);
#define rwID_SKINPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16)
typedef struct RpSkin RpSkin;
/* ---- end of the generated declarations ---- */

struct SkinGlobalPlatform
{
    RwUInt32 unverifiedSize[1];
};
typedef void * SkinUnaligned;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;
typedef struct SkinSplitData SkinSplitData;
typedef struct SkinAtomicData SkinAtomicData;
struct SkinAtomicData
{
    RpHAnimHierarchy *hierarchy;
};
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
extern void * SkinOpen( void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinClose( void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinGeometryConstructor( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinGeometryDestructor( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinGeometryCopy( void *dstObject, const void *srcObject __RWUNUSEDRELEASE__, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinAtomicConstructor( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinAtomicDestructor( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern void * SkinAtomicCopy( void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern RwBool SkinAtomicAlways( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__ );
extern RwBool SkinAtomicRights( void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__, RwUInt32 extraData );
extern RwInt32 SkinGeometrySize(const void *object, RwInt32 offset __RWUNUSED__, RwInt32 bytes __RWUNUSED__);
extern RwStream * SkinGeometryWrite(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__ );
extern RwStream * SkinGeometryRead( RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__ );
extern RwStream * SkinAtomicRead( RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject );
extern RwStream * SkinAtomicWrite( RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object __RWUNUSED__, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__ );
extern RwInt32 SkinAtomicGetSize( const void *object __RWUNUSED__, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__ );
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(SkinGlobals, freeList) == 0x18);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(SkinGlobals, module) == 0x1c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_4, sizeof(SkinAtomicData) == 4);      /* exe: RpSkinPluginAttach 0x0053e89a `push 0x4` (atomic plugin data size: no D3D9 atomic platform data) */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: validated 2026-11-14 authority=diffemu */
RwBool RpSkinPluginAttach(void)
{
    RwInt32 success;

    _rpSkinGlobals.engineOffset =
        RwEngineRegisterPlugin( 0,
                                rwID_SKINPLUGIN,
                                SkinOpen,
                                SkinClose );

    _rpSkinGlobals.atomicOffset =
        RpAtomicRegisterPlugin( sizeof(SkinAtomicData),
                                rwID_SKINPLUGIN,
                                SkinAtomicConstructor,
                                SkinAtomicDestructor,
                                SkinAtomicCopy );

    success = RpAtomicRegisterPluginStream( rwID_SKINPLUGIN,
                                            SkinAtomicRead,
                                            SkinAtomicWrite,
                                            SkinAtomicGetSize );

    success = RpAtomicSetStreamAlwaysCallBack( rwID_SKINPLUGIN,
                                               SkinAtomicAlways );

    success = RpAtomicSetStreamRightsCallBack( rwID_SKINPLUGIN,
                                               SkinAtomicRights );

    _rpSkinGlobals.geometryOffset =
        RpGeometryRegisterPlugin( sizeof(RpSkin *),
                                  rwID_SKINPLUGIN,
                                  SkinGeometryConstructor,
                                  SkinGeometryDestructor,
                                  SkinGeometryCopy );

    success = RpGeometryRegisterPluginStream( rwID_SKINPLUGIN,
                                              SkinGeometryRead,
                                              SkinGeometryWrite,
                                              SkinGeometrySize );

    return (TRUE);
}
