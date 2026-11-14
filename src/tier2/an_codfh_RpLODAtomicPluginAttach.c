/* RpLODAtomicPluginAttach (MFL.exe 00567e70). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
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
/* ---- end of the generated declarations ---- */

extern RwBool
        RpLODAtomicPluginAttach( void );
typedef struct _rpLODAtomicGlobalVars RpLODAtomicGlobalVars;
struct _rpLODAtomicGlobalVars
{
    RwInt32             engineOffset; 
    RwInt32             atmCacheOffset; 
    RwCamera           *atmCamera;
    RwFreeList         *atmCacheFreeList;
};
typedef struct _rpLODAtomicCache RpLODAtomicCache;
extern RpLODAtomicGlobalVars RpLODAtomicGlobals;
extern RwStream * LODAtomicCallBackReadChunk(RwStream * stream, RwInt32 __RWUNUSED__ len, RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size);
extern RwStream * LODAtomicCallBackWriteChunk(RwStream * stream, RwInt32 __RWUNUSED__ len, RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size);
extern RwInt32 LODAtomicCallBackGetChunkSize(RpAtomic * atomic, RwInt32 offset, RwInt32 size);
extern void * LODAtomicCallBackConstructor(RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size);
extern void * LODAtomicCallBackCopy(RpAtomic * dstAtomic, RpAtomic * srcAtomic, RwInt32 offset, RwInt32 __RWUNUSED__ size);
extern void * LODAtomicCallBackDestructor(RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size);
extern void * LODAtomicCallBackOpen(void *ptr, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * LODAtomicCallBackClose(void *ptr, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
/* TIER2: unvalidated authority=diffemu */
/* TIER2: validated 2026-11-14 authority=diffemu */
RwBool RpLODAtomicPluginAttach(void)
{
    RwInt32             offset;
    RwBool              result = FALSE;

    RpLODAtomicGlobals.engineOffset =
        RwEngineRegisterPlugin(0,
                               rwID_LODATMPLUGIN,
                               (RwPluginObjectConstructor)
                               LODAtomicCallBackOpen,
                               (RwPluginObjectDestructor)
                               LODAtomicCallBackClose);

    RpLODAtomicGlobals.atmCacheOffset =
        RpAtomicRegisterPlugin(sizeof(RpLODAtomicCache *),
                               rwID_LODATMPLUGIN,
                               (RwPluginObjectConstructor)
                               LODAtomicCallBackConstructor,
                               (RwPluginObjectDestructor)
                               LODAtomicCallBackDestructor,
                               (RwPluginObjectCopy)
                               LODAtomicCallBackCopy);

    offset =
        RpAtomicRegisterPluginStream(rwID_LODATMPLUGIN,
                                     (RwPluginDataChunkReadCallBack)
                                     LODAtomicCallBackReadChunk,
                                     (RwPluginDataChunkWriteCallBack)
                                     LODAtomicCallBackWriteChunk,
                                     (RwPluginDataChunkGetSizeCallBack)
                                     LODAtomicCallBackGetChunkSize);

    RpLODAtomicGlobals.atmCamera = NULL;
    RpLODAtomicGlobals.atmCacheFreeList = NULL;

    result = (RpLODAtomicGlobals.atmCacheOffset >= 0);

    return (result);
}
