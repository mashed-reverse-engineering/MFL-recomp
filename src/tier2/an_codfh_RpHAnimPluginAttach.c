/* RpHAnimPluginAttach (MFL.exe 00543c50). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
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
RwFrameRegisterPlugin(RwInt32 size,
                      RwUInt32 pluginID,
                      RwPluginObjectConstructor constructCB,
                      RwPluginObjectDestructor destructCB,
                      RwPluginObjectCopy copyCB);
extern RwInt32
RwFrameRegisterPluginStream(RwUInt32 pluginID,
                            RwPluginDataChunkReadCallBack readCB,
                            RwPluginDataChunkWriteCallBack writeCB,
                            RwPluginDataChunkGetSizeCallBack getSizeCB);
#define rwID_HANIMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1e)
typedef struct RpHAnimFrameExtension RpHAnimFrameExtension;
struct RpHAnimFrameExtension
{
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
extern void * HAnimOpen(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * HAnimClose(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * HAnimConstructor(void *object, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * HAnimDestructor(void *object, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * HAnimCopy(void *dstObject, const void * srcObject, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern RwStream * HAnimWrite(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, const void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject);
extern RwStream * HAnimRead(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject);
extern RwInt32 HAnimSize(const void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject);
/* TIER2: unvalidated authority=diffemu */
RwBool RpHAnimPluginAttach(void)
{
    RwInt32             success = 0;

    if (RwEngineRegisterPlugin(0,
                               rwID_HANIMPLUGIN,
                               HAnimOpen, HAnimClose) < 0)
    {
        return (FALSE);
    }

    RpHAnimAtomicGlobals.engineOffset =
        RwFrameRegisterPlugin(sizeof(RpHAnimFrameExtension),
                              rwID_HANIMPLUGIN,
                              HAnimConstructor, HAnimDestructor,
                              HAnimCopy);

    success =
        RwFrameRegisterPluginStream(rwID_HANIMPLUGIN,
                                    HAnimRead, HAnimWrite, HAnimSize);

    return ((success >= 0) && (RpHAnimAtomicGlobals.engineOffset >= 0));
}
