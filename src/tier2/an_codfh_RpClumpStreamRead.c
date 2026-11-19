/* RpClumpStreamRead (MFL.exe 004f7560). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the static
 * helpers GeometryListStreamRead / RpClumpAddAtomic / RpClumpAddLight / RpClumpAddCamera and RpClumpDestroy (two forms) inlined by hand as
 * MFL.exe inlines them, GeometryListDeinitialize inlined where MFL.exe inlines it, the stream version range of MFL.exe (RenderWare
 * 3.7.0.2), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLIBRARYCURRENTRWVERSION 3
#define rwLIBRARYCURRENTMAJORREVISION 6
#define rwLIBRARYCURRENTMINORREVISION 0
#define rwLIBRARYCURRENTBINARYREVISION 3
#define rwLIBRARYBASEVERSION 0x34000
#define rwLIBRARYCURRENTVERSION ((rwLIBRARYCURRENTRWVERSION << 16) | (rwLIBRARYCURRENTMAJORREVISION << 12) | (rwLIBRARYCURRENTMINORREVISION << 8) | (rwLIBRARYCURRENTBINARYREVISION))
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListAddLLLink(list,linkvar) ( (linkvar)->next = (list)->link.next, (linkvar)->prev = (&(list)->link), ((list)->link.next)->prev = (linkvar), (list)->link.next = (linkvar) )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
enum RwCorePluginID
{
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),

    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),

    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),

    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),

    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),

    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),

    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),

    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),

    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),

    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),

    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),

    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),

    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),

    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),

    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),

    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),

    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),

    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),

    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),

    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),

    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),

    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),

    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),

    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
#define rwObjectSetParent(c,p) (((RwObject *)(c))->parent) = (void *)(p)
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginErrorStrCallBack)(void *);
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
struct RwPluginRegEntry
{
    RwInt32 offset;
    RwInt32 size;
    RwUInt32 pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
extern const
RwPluginRegistry *_rwPluginRegistryDeInitObject(const RwPluginRegistry * reg,
                                               void *object);
#define rwPluginRegistryDeInitObject(reg,object) _rwPluginRegistryDeInitObject(reg, object)
extern const RwPluginRegistry *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg,
                                                              RwStream *stream,
                                                              void *object);
#define rwPluginRegistryReadDataChunks(reg,stream,object) _rwPluginRegistryReadDataChunks(reg, stream, object)
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
#define RwMemNative32(_m,_s) (_m)
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
struct RwFrame
{
    RwObject object;

    RwLLLink inDirtyListLink;

    RwMatrix modelling;
    RwMatrix ltm;

    RwLinkList objectList;

    struct RwFrame *child;
    struct RwFrame *next;
    struct RwFrame *root;


};
extern RwBool
RwFrameDestroyHierarchy(RwFrame * frame);
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
#define rwObjectHasFrameSetFrame(object,frame) _rwObjectHasFrameSetFrame(object, frame)
typedef struct rwFrameList rwFrameList;
struct rwFrameList
{
    RwFrame **frames;
    RwInt32 numFrames;
};
extern rwFrameList *
_rwFrameListDeinitialize(rwFrameList *frameList);
extern rwFrameList *
_rwFrameListStreamRead(RwStream *stream,
                      rwFrameList *fl);
#define rwFrameListDeinitialize(frameList) _rwFrameListDeinitialize(frameList)
#define rwFrameListStreamRead(stream,fl) _rwFrameListStreamRead(stream, fl)
#define RwCameraSetFrameMacro(_camera,_frame) (_rwObjectHasFrameSetFrame((_camera), (_frame)), (_camera))
#define RwCameraSetFrame(_camera,_frame) RwCameraSetFrameMacro(_camera, _frame)
typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);
extern RwCamera *RwCameraStreamRead(RwStream *stream);
extern RwBool
RpGeometryDestroy(RpGeometry *geometry);
extern RpGeometry *
RpGeometryStreamRead(RwStream *stream);
typedef struct RpLight RpLight;
typedef RpLight *(*RpLightCallBack) (RpLight * light, void *data);
#define RpLightSetFrameMacro(_light,_frame) (rwObjectHasFrameSetFrame((_light), (_frame)), (_light))
#define RpLightSetFrame(_light,_frame) RpLightSetFrameMacro(_light, _frame)
extern RpLight *RpLightStreamRead(RwStream *stream);
typedef RpAtomic *(*RpAtomicCallBack) (RpAtomic * atomic, void *data);
typedef struct RpClumpChunkInfo RpClumpChunkInfo;
typedef struct RpClumpChunkInfo _rpClump;
struct RpClumpChunkInfo
{
    RwInt32 numAtomics;
    RwInt32 numLights;
    RwInt32 numCameras;
};
#define RpClumpSetFrameMacro(_clump,_frame) (rwObjectSetParent(_clump, _frame), (_clump))
#define RpClumpGetFrameMacro(_clump) ((RwFrame *) rwObjectGetParent(_clump))
#define RpClumpSetFrame(_clump,_frame) RpClumpSetFrameMacro(_clump, _frame)
#define RpClumpGetFrame(_clump) RpClumpGetFrameMacro(_clump)
extern RpClump *
RpClumpForAllAtomics(RpClump * clump,
                     RpAtomicCallBack callback,
                     void *pData);
extern RpClump *
RpClumpForAllLights(RpClump * clump,
                    RpLightCallBack callback,
                    void *pData);
extern RpClump *
RpClumpForAllCameras(RpClump * clump,
                     RwCameraCallBack callback,
                     void *pData);
extern RwBool
RpClumpDestroy(RpClump * clump);
extern RpClump *
RpClumpCreate(void);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwResEntry, size) == 0x8);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwResEntry, owner) == 0xc);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_6, sizeof(RwResEntry) == 0x18);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwPluginRegEntry, pluginID) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwPluginRegEntry, constructCB) == 0x20);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RwPluginRegEntry, copyCB) == 0x28);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RwPluginRegEntry, nextRegEntry) == 0x30);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RwPluginRegEntry, parentRegistry) == 0x38);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RwPluginRegEntry, size) == 0x4);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RwPluginRegEntry, readCB) == 0xc);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RwPluginRegEntry, alwaysCB) == 0x18);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_19, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_20, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_22, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_23, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_24, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
RW_LAYOUT_ASSERT(rw37_row_25, sizeof(RpClumpChunkInfo) == 0xc && RW_OFFSETOF(RpClumpChunkInfo, numLights) == 4 && RW_OFFSETOF(RpClumpChunkInfo, numCameras) == 8);      /* MFL.exe 0x4f75cf RwStreamRead(stream, &cl, 0xc) into esp+0x48; numAtomics [esp+0x48] at 0x4f7a41, numLights [esp+0x4c] at 0x4f7aa8, numCameras [esp+0x50] (RpClumpStreamRead) */
RW_LAYOUT_ASSERT(rw37_row_26, RW_OFFSETOF(rwFrameList, frames) == 0);      /* MFL.exe 0x4f7b12..0x4f7b1a (RpClumpStreamRead): fl at esp+0x30, mov edx,[esp+0x30]; mov ecx,[edx+eax*4] = fl.frames[frameIndex] */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID 0x02
typedef struct RpClumpCameraExt RpClumpCameraExt;
struct RpClumpCameraExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
typedef struct RpClumpLightExt RpClumpLightExt;
struct RpClumpLightExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
typedef struct rpGeometryList rpGeometryList;
struct rpGeometryList
{
    RpGeometry        **geometries;
    RwInt32             numGeoms;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpClumpLightExt, inClumpLink) == 4 && RW_OFFSETOF(RpClumpCameraExt, inClumpLink) == 4);      /* MFL.exe 0x4f7b31..0x4f7b45: ext = light + _rpClumpLightExtOffset, link at ext+4 (prev at ext+8), ext->clump at ext+0; the camera loop the same with _rpClumpCameraExtOffset */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(rpGeometryList) == 8 && RW_OFFSETOF(rpGeometryList, numGeoms) == 4);      /* MFL.exe 0x4f78e8 / 0x4f7900: gl.geometries at esp+0x28, gl.numGeoms at esp+0x2c, fl follows at esp+0x30 */
typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
RW_LAYOUT_ASSERT(rw37_own_2, RW_OFFSETOF(rpClumpGlobals, clumpFreeList) == 4);      /* MFL.exe 0x4f6fcb atomic free list [globals+0], 0x4f70a1 clump free list [globals+4] (RpClumpDestroy) */
#define RWCLUMPGLOBAL(var) (RWPLUGINOFFSET(rpClumpGlobals, RwEngineInstance, clumpModule.globalsOffset)->var)
extern RwModuleInfo clumpModule;
/* MFL.exe is a RenderWare 3.7.0.2 build: its stream version checks are 0x35000 .. 0x37002 (0x4f75b5 / 0x4f75c0); the codfh headers give 3.6.0.3 */
#undef rwLIBRARYBASEVERSION
#undef rwLIBRARYCURRENTVERSION
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define CLUMPCAMERAEXTFROMCAMERA(camera) ((RpClumpCameraExt *)(((RwUInt8 *)(camera)) + _rpClumpCameraExtOffset))
#define CLUMPLIGHTEXTFROMLIGHT(light) ((RpClumpLightExt *)(((RwUInt8 *)(light)) + _rpClumpLightExtOffset))
extern RwPluginRegistry clumpTKList;
extern RwInt32  _rpClumpCameraExtOffset;
extern RwInt32  _rpClumpLightExtOffset;
extern rpGeometryList * GeometryListDeinitialize(rpGeometryList * geomList);
extern RpAtomic * DestroyClumpAtomic(RpAtomic * atomic, void * pData);
extern RpLight * DestroyClumpLight(RpLight * light, void * pData);
extern RwCamera * DestroyClumpCamera(RwCamera * camera, void * pData);
extern RwBool RpAtomicDestroy(RpAtomic * atomic);

/* The static helpers of baclump.c that MFL.exe inlines into RpClumpStreamRead are written out here (diffemu and the build take one
 * function per row): GeometryListStreamRead, RpClumpAddAtomic, RpClumpAddLight, RpClumpAddCamera, and RpClumpDestroy in two forms (see below).
 * GeometryListDeinitialize (MFL.exe 0x4f7150) is inlined at some sites and called at others, as in MFL.exe. */

/* GeometryListDeinitialize, inlined */
#define GEOMETRYLISTFREE(_gl)                                           \
MACRO_START                                                             \
{                                                                       \
    RwInt32             _g;                                             \
                                                                        \
    for (_g = 0; _g < (_gl)->numGeoms; _g++)                            \
    {                                                                   \
        RpGeometryDestroy((_gl)->geometries[_g]);                       \
    }                                                                   \
    if ((_gl)->geometries)                                              \
    {                                                                   \
        RwFree((_gl)->geometries);                                      \
        (_gl)->geometries = (RpGeometry **)NULL;                        \
    }                                                                   \
}                                                                       \
MACRO_STOP

/* RpClumpDestroy as MFL.exe inlines it into the error paths before the geometry list is read: RpClumpForAllAtomics with its
 * callback DestroyClumpAtomic (RpAtomicDestroy, returns the atomic) inlined as well */
#define CLUMPDESTROYEARLY(_clump)                                       \
MACRO_START                                                             \
{                                                                       \
    RwLLLink           *_cur, *_end, *_next;                            \
                                                                        \
    rwPluginRegistryDeInitObject(&clumpTKList, (_clump));               \
    _cur = rwLinkListGetFirstLLLink(&(_clump)->atomicList);             \
    _end = rwLinkListGetTerminator(&(_clump)->atomicList);              \
    while (_cur != _end)                                                \
    {                                                                   \
        RpAtomic *_atomic = rwLLLinkGetData(_cur, RpAtomic, inClumpLink); \
                                                                        \
        _next = rwLLLinkGetNext(_cur);                                  \
        RpAtomicDestroy(_atomic);                                       \
        if (!_atomic)                                                   \
        {                                                               \
            break;                                                      \
        }                                                               \
        _cur = _next;                                                   \
    }                                                                   \
    RpClumpForAllLights((_clump), DestroyClumpLight, NULL);             \
    RpClumpForAllCameras((_clump), DestroyClumpCamera, NULL);           \
    if (RpClumpGetFrame(_clump))                                        \
    {                                                                   \
        RwFrameDestroyHierarchy(RpClumpGetFrame(_clump));               \
    }                                                                   \
    RwFreeListFree(RWCLUMPGLOBAL(clumpFreeList), (_clump));             \
}                                                                       \
MACRO_STOP

/* RpClumpDestroy as MFL.exe inlines it into the later error paths (the RpClumpForAll* calls kept) */
#define CLUMPDESTROYLATE(_clump)                                        \
MACRO_START                                                             \
{                                                                       \
    rwPluginRegistryDeInitObject(&clumpTKList, (_clump));               \
    RpClumpForAllAtomics((_clump), DestroyClumpAtomic, NULL);           \
    RpClumpForAllLights((_clump), DestroyClumpLight, NULL);             \
    RpClumpForAllCameras((_clump), DestroyClumpCamera, NULL);           \
    if (RpClumpGetFrame(_clump))                                        \
    {                                                                   \
        RwFrameDestroyHierarchy(RpClumpGetFrame(_clump));               \
    }                                                                   \
    RwFreeListFree(RWCLUMPGLOBAL(clumpFreeList), (_clump));             \
}                                                                       \
MACRO_STOP

/* RpClumpAddAtomic / RpClumpAddLight / RpClumpAddCamera */
#define CLUMPADDATOMIC(_clump, _atomic)                                 \
MACRO_START                                                             \
{                                                                       \
    rwLinkListAddLLLink(&(_clump)->atomicList, &(_atomic)->inClumpLink); \
    (_atomic)->clump = (_clump);                                        \
}                                                                       \
MACRO_STOP
#define CLUMPADDLIGHT(_clump, _light)                                   \
MACRO_START                                                             \
{                                                                       \
    RpClumpLightExt *_ext = CLUMPLIGHTEXTFROMLIGHT(_light);             \
                                                                        \
    rwLinkListAddLLLink(&(_clump)->lightList, &_ext->inClumpLink);      \
    _ext->clump = (_clump);                                             \
}                                                                       \
MACRO_STOP
#define CLUMPADDCAMERA(_clump, _camera)                                 \
MACRO_START                                                             \
{                                                                       \
    RpClumpCameraExt *_ext = CLUMPCAMERAEXTFROMCAMERA(_camera);         \
                                                                        \
    rwLinkListAddLLLink(&(_clump)->cameraList, &_ext->inClumpLink);     \
    _ext->clump = (_clump);                                             \
}                                                                       \
MACRO_STOP

extern RpAtomic * ClumpAtomicStreamRead(RwStream * stream, rwFrameList * fl, rpGeometryList * gl);

/* TIER2: validated 2026-11-14 authority=diffemu */
RpClump * RpClumpStreamRead(RwStream * stream)
{
    RwBool              status;
    RwUInt32            size;
    RwUInt32            version;

    status = RwStreamFindChunk(stream, rwID_STRUCT, &size, &version);

    if (!status)
    {
        RWERROR((E_RW_READ));
        return ((RpClump *)NULL);
    }

    if ((version >= rwLIBRARYBASEVERSION) &&
        (version <= rwLIBRARYCURRENTVERSION))
    {
        RpClump            *clump;
        RpAtomic           *atom;
        _rpClump            cl;
        rwFrameList         fl;
        rpGeometryList      gl;
        RwInt32             i;
        RwUInt32            chunkversion;

        status = (sizeof(cl) == RwStreamRead(stream, &cl, sizeof(cl)) );
        if (!status)
        {
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        (void)RwMemNative32(&cl, sizeof(cl));

        clump = RpClumpCreate();
        if (!clump)
        {
            return ((RpClump *)NULL);
        }

        status = RwStreamFindChunk(stream, (RwUInt32)rwID_FRAMELIST,
                                   (RwUInt32 *)NULL, &chunkversion);
        if (!status)
        {
            CLUMPDESTROYEARLY(clump);
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        status = (NULL != rwFrameListStreamRead(stream, &fl));
        if (!status)
        {
            CLUMPDESTROYEARLY(clump);
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        RpClumpSetFrame(clump, fl.frames[0]);

        status = RwStreamFindChunk (stream, (RwUInt32)rwID_GEOMETRYLIST,
                                    (RwUInt32 *)NULL, &chunkversion);
        if (!status)
        {
            rwFrameListDeinitialize(&fl);
            CLUMPDESTROYEARLY(clump);
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        /* GeometryListStreamRead(stream, &gl) */
        {
            RwInt32             numGeoms;
            RwUInt32            glsize;
            RwUInt32            glversion;

            status = RwStreamFindChunk(stream, rwID_STRUCT, &glsize, &glversion);
            if (status)
            {
                if ((glversion >= rwLIBRARYBASEVERSION)
                    && (glversion <= rwLIBRARYCURRENTVERSION))
                {
                    status = (RwStreamRead(stream, &numGeoms, sizeof(numGeoms)) == sizeof(numGeoms));
                    if (status)
                    {
                        (void)RwMemNative32(&numGeoms, sizeof(numGeoms));

                        gl.numGeoms = 0;

                        if (numGeoms > 0)
                        {
                            gl.geometries =
                                (RpGeometry **) RwMalloc(sizeof(RpGeometry *) * numGeoms,
                                                  rwID_GEOMETRY | rwMEMHINTDUR_EVENT);
                            if (!gl.geometries)
                            {
                                RWERROR((E_RW_NOMEM, (sizeof(RpGeometry *) * numGeoms)));
                                status = FALSE;
                            }
                        }
                        else
                        {
                            gl.geometries = (RpGeometry **)NULL;
                        }

                        for (i = 0; status && i < numGeoms; ++i)
                        {
                            if (!RwStreamFindChunk(stream, rwID_GEOMETRY,
                                                   (RwUInt32 *)NULL, &glversion))
                            {
                                GEOMETRYLISTFREE(&gl);
                                status = FALSE;
                            }
                            else if ((glversion >= rwLIBRARYBASEVERSION)
                                     && (glversion <= rwLIBRARYCURRENTVERSION))
                            {
                                if (!(gl.geometries[i] = RpGeometryStreamRead(stream)))
                                {
                                    GeometryListDeinitialize(&gl);
                                    status = FALSE;
                                }
                                else
                                {
                                    ++gl.numGeoms;
                                }
                            }
                            else
                            {
                                GeometryListDeinitialize(&gl);
                                status = FALSE;
                            }
                        }
                    }
                }
                else
                {
                    RWERROR((E_RW_BADVERSION));
                    status = FALSE;
                }
            }
        }
        if (!status)
        {
            rwFrameListDeinitialize(&fl);
            CLUMPDESTROYLATE(clump);
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        for (i = 0; i < cl.numAtomics; ++i)
        {
            status = RwStreamFindChunk(stream, (RwUInt32)rwID_ATOMIC,
                                       (RwUInt32 *)NULL, &version);
            if (status)
            {
                atom = ClumpAtomicStreamRead(stream, &fl, &gl);
                status = (NULL != atom);
            }

            if (!status)
            {
                GEOMETRYLISTFREE(&gl);
                rwFrameListDeinitialize(&fl);
                CLUMPDESTROYLATE(clump);
                RWERROR((E_RW_READ));
                return ((RpClump *)NULL);
            }

            CLUMPADDATOMIC(clump, atom);
        }

        for (i = 0; i < cl.numLights; ++i)
        {
            RpLight *light;
            RwInt32 frameIndex;

            status = RwStreamFindChunk(stream,
                        rwID_STRUCT, (RwUInt32 *)NULL, (RwUInt32 *)NULL)
                 && (RwStreamReadInt32(stream, &frameIndex, sizeof(frameIndex)) != NULL);

            if (status)
            {
                status = RwStreamFindChunk(stream,
                            rwID_LIGHT, (RwUInt32 *)NULL, (RwUInt32 *)NULL);
            }

            if (status)
            {
                light = RpLightStreamRead(stream);
                status = (light != NULL);
            }

            if (!status)
            {
                GEOMETRYLISTFREE(&gl);
                rwFrameListDeinitialize(&fl);
                CLUMPDESTROYLATE(clump);
                RWERROR((E_RW_READ));
                return ((RpClump *)NULL);
            }

            RpLightSetFrame(light, fl.frames[frameIndex]);
            CLUMPADDLIGHT(clump, light);
        }

        for (i = 0; i < cl.numCameras; ++i)
        {
            RwCamera   *camera;
            RwInt32     frameIndex;

            status = RwStreamFindChunk(stream,
                        rwID_STRUCT, (RwUInt32 *)NULL, (RwUInt32 *)NULL)
                 && (RwStreamReadInt32(stream, &frameIndex, sizeof(frameIndex)) != NULL);

            if (status)
            {
                status = RwStreamFindChunk(stream,
                            rwID_CAMERA, (RwUInt32 *)NULL, (RwUInt32 *)NULL);
            }

            if (status)
            {
                camera = RwCameraStreamRead(stream);
                status = (camera != NULL);
            }

            if (!status)
            {
                GEOMETRYLISTFREE(&gl);
                rwFrameListDeinitialize(&fl);
                CLUMPDESTROYLATE(clump);
                RWERROR((E_RW_READ));
                return ((RpClump *)NULL);
            }

            RwCameraSetFrame(camera, fl.frames[frameIndex]);
            CLUMPADDCAMERA(clump, camera);
        }

        GEOMETRYLISTFREE(&gl);

        rwFrameListDeinitialize(&fl);

        status = (NULL !=
                  rwPluginRegistryReadDataChunks(&clumpTKList, stream, clump));

        if (!status)
        {
            RpClumpDestroy(clump);
            RWERROR((E_RW_READ));
            return ((RpClump *)NULL);
        }

        return (clump);
    }
    else
    {
        RWERROR((E_RW_BADVERSION));
        return ((RpClump *)NULL);
    }
}
