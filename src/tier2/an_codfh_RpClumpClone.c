/* RpClumpClone (MFL.exe 004f6bf0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, RpClumpSetFrame,
 * RpClumpForAllAtomics with ClumpAtomicCloneAndAttach (and RpAtomicSetFrame / RpAtomicDestroy in it) and the two RpClumpDestroy calls
 * inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return),
 * conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names
 * declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwObjectSetParent(c,p) (((RwObject *)(c))->parent) = (void *)(p)
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
RwPluginRegistry *_rwPluginRegistryCopyObject(const RwPluginRegistry * reg,
                                             void *dstObject,
                                             const void *srcObject);
#define rwPluginRegistryCopyObject(reg,dstObject,srcObject) _rwPluginRegistryCopyObject(reg, dstObject, srcObject)
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
extern RwFrame *
_rwFrameCloneAndLinkClones(RwFrame * root);
extern
RwFrame *
_rwFramePurgeClone(RwFrame *root);
enum e_rwdb_CriterionWorld
{

E_RP_WORLD_INVSPHERE,
E_RP_WORLD_INVINTERSECTION,
E_RP_WORLD_MATRANGE,
E_RP_WORLD_CLUMPINWORLD,
E_RP_WORLD_CLUMPNOTINWORLD,
E_RP_WORLD_ATOMICNOFRAME,
E_RP_WORLD_TOOMANYVERTICES,

    e_rwdb_CriterionWorldLAST = RWFORCEENUMSIZEINT
};
typedef enum e_rwdb_CriterionWorld e_rwdb_CriterionWorld;
typedef RpAtomic *(*RpAtomicCallBack) (RpAtomic * atomic, void *data);
#define RpClumpSetFrameMacro(_clump,_frame) (rwObjectSetParent(_clump, _frame), (_clump))
#define RpClumpGetFrameMacro(_clump) ((RwFrame *) rwObjectGetParent(_clump))
#define RpClumpSetFrame(_clump,_frame) RpClumpSetFrameMacro(_clump, _frame)
#define RpClumpGetFrame(_clump) RpClumpGetFrameMacro(_clump)
extern RpClump *
RpClumpForAllAtomics(RpClump * clump,
                     RpAtomicCallBack callback,
                     void *pData);
extern RpAtomic *
RpAtomicSetFrame(RpAtomic * atomic,
                 RwFrame * frame);
extern RpClump *
RpClumpAddAtomic(RpClump * clump,
                 RpAtomic * atomic);
extern RwBool
RpClumpDestroy(RpClump * clump);
extern RpClump *
RpClumpCreate(void);
extern RwBool
RpAtomicDestroy(RpAtomic * atomic);
extern RpAtomic *
RpAtomicClone(RpAtomic * atomic);
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwPluginRegEntry, pluginID) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwPluginRegEntry, constructCB) == 0x20);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwPluginRegEntry, copyCB) == 0x28);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwPluginRegEntry, nextRegEntry) == 0x30);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwPluginRegEntry, parentRegistry) == 0x38);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RwPluginRegEntry, size) == 0x4);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RwPluginRegEntry, readCB) == 0xc);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RwPluginRegEntry, alwaysCB) == 0x18);      /* an_codfh__rwPluginRegistryReadDataChunks.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_19, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID 0x02
typedef struct RpClumpCloneStatus RpClumpCloneStatus;
struct RpClumpCloneStatus
{
    RpClump            *oldClump;
    RpClump            *newClump;
    RwBool              success;
};
extern RwPluginRegistry clumpTKList;
typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
typedef struct RpClumpCameraExt RpClumpCameraExt;
struct RpClumpCameraExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
#define RWCLUMPGLOBAL(var) (RWPLUGINOFFSET(rpClumpGlobals, RwEngineInstance, clumpModule.globalsOffset)->var)
extern RwPluginRegistry clumpTKList;
extern RwInt32  _rpClumpCameraExtOffset;
extern RwModuleInfo clumpModule;
/* the SDK list / object macros the inlined callbacks use (the codfh function itself goes through RpClumpForAllAtomics) */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define rwObjectGetPrivateFlags(c) (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f) (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
#define rwObjectHasFrameSetFrame(object,frame) _rwObjectHasFrameSetFrame(object, frame)
#define rwObjectHasFrameReleaseFrame(object) _rwObjectHasFrameReleaseFrame(object)
#define rpATOMICPRIVATEWORLDBOUNDDIRTY 0x01
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
extern void _rwObjectHasFrameReleaseFrame(void *object);
typedef struct RpLight RpLight;
typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);
extern RwBool RpLightDestroy(RpLight *light);
extern RwBool RwResourcesFreeResEntry(RwResEntry *entry);
extern RwBool RwFrameDestroyHierarchy(RwFrame *frame);
extern RpClump *RpClumpRemoveLight(RpClump *clump, RpLight *light);
extern RpLight *DestroyClumpLight(RpLight *light, void *pData);
extern RwCamera *DestroyClumpCamera(RwCamera *camera, void *pData);
extern RpAtomic *RpAtomicSetGeometry(RpAtomic *atomic, RpGeometry *geometry, RwUInt32 flags);
extern RpClump *RpClumpForAllCameras(RpClump *clump, RwCameraCallBack callback, void *pData);
extern RwInt32  _rpClumpLightExtOffset;
extern RwPluginRegistry atomicTKList;

/* RpClumpDestroy(clump), inlined twice in MFL.exe with the atomic walk calling RpAtomicDestroy and the cameras through
 * RpClumpForAllCameras(clump, DestroyClumpCamera, NULL); the lights: lightsInline = DestroyClumpLight inlined (RpClumpRemoveLight then
 * RpLightDestroy, 0x4f6c5f..0x4f6c8e), else the DestroyClumpLight call (0x4f6db9..0x4f6ddb). A macro: diffemu links no static helper. */
#define ClumpCloneDestroyNew(_clump, _lightsInline)                                         \
MACRO_START                                                                                 \
{                                                                                           \
    RpClump            *dClump = (_clump);                                                  \
    RwLLLink           *cur, *end, *next;                                                   \
                                                                                            \
    rwPluginRegistryDeInitObject(&clumpTKList, dClump);                                     \
                                                                                            \
    cur = rwLinkListGetFirstLLLink(&dClump->atomicList);                                    \
    end = rwLinkListGetTerminator(&dClump->atomicList);                                     \
    while (cur != end)                                                                      \
    {                                                                                       \
        RpAtomic           *atomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);           \
                                                                                            \
        next = rwLLLinkGetNext(cur);                                                        \
        RpAtomicDestroy(atomic);                                                            \
        if (!atomic)                                                                        \
        {                                                                                   \
            break;                                                                          \
        }                                                                                   \
        cur = next;                                                                         \
    }                                                                                       \
                                                                                            \
    cur = rwLinkListGetFirstLLLink(&dClump->lightList);                                     \
    end = rwLinkListGetTerminator(&dClump->lightList);                                      \
    while (cur != end)                                                                      \
    {                                                                                       \
        RpLight            *light = (RpLight *)((RwUInt8 *)cur - _rpClumpLightExtOffset - 4); \
                                                                                            \
        next = rwLLLinkGetNext(cur);                                                        \
        if (_lightsInline)                                                                  \
        {                                                                                   \
            RpClumpRemoveLight(*(RpClump **)((RwUInt8 *)cur - 4), light);                   \
            RpLightDestroy(light);                                                          \
            if (!light)                                                                     \
            {                                                                               \
                break;                                                                      \
            }                                                                               \
        }                                                                                   \
        else if (!DestroyClumpLight(light, NULL))                                           \
        {                                                                                   \
            break;                                                                          \
        }                                                                                   \
        cur = next;                                                                         \
    }                                                                                       \
                                                                                            \
    RpClumpForAllCameras(dClump, DestroyClumpCamera, NULL);                                 \
                                                                                            \
    if (RpClumpGetFrame(dClump))                                                            \
    {                                                                                       \
        RwFrameDestroyHierarchy(RpClumpGetFrame(dClump));                                   \
    }                                                                                       \
                                                                                            \
    RwFreeListFree(RWCLUMPGLOBAL(clumpFreeList), dClump);                                   \
}                                                                                           \
MACRO_STOP

/* TIER2: unvalidated authority=diffemu */
RpClump * RpClumpClone(RpClump * clump)
{
    RpClumpCloneStatus  clumpCloneStatus;
    RwFrame            *clone;

    clumpCloneStatus.oldClump = clump;

    if (!(clumpCloneStatus.newClump = RpClumpCreate()))
    {
        return ((RpClump *)NULL);
    }

    /* RpClumpSetFrame(newClump, clone), inlined in MFL.exe: the frame link only */
    clone =
        _rwFrameCloneAndLinkClones(RpClumpGetFrame(clumpCloneStatus.oldClump));
    clumpCloneStatus.newClump->object.parent = clone;

    if (!RpClumpGetFrame(clumpCloneStatus.newClump))
    {
        ClumpCloneDestroyNew(clumpCloneStatus.newClump, TRUE);

        return ((RpClump *)NULL);
    }

    clumpCloneStatus.success = TRUE;

    /* RpClumpForAllAtomics(oldClump, ClumpAtomicCloneAndAttach, &clumpCloneStatus), inlined in MFL.exe (0x4f6c95..0x4f6d5f) with
     * RpAtomicSetFrame and, for an atomic without a frame, RpAtomicDestroy of the clone inlined; a NULL callback result stops the walk */
    {
        RwLLLink           *cur, *end, *next;

        cur = rwLinkListGetFirstLLLink(&clumpCloneStatus.oldClump->atomicList);
        end = rwLinkListGetTerminator(&clumpCloneStatus.oldClump->atomicList);
        while (cur != end)
        {
            RpAtomic           *atomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);
            RpAtomic           *newAtomic;
            RwFrame            *frame;

            next = rwLLLinkGetNext(cur);

            newAtomic = RpAtomicClone(atomic);
            if (!newAtomic)
            {
                clumpCloneStatus.success = FALSE;
                break;
            }

            frame = (RwFrame *) rwObjectGetParent(atomic);
            if (!frame)
            {
                rwPluginRegistryDeInitObject(&atomicTKList, newAtomic);
                if (newAtomic->repEntry)
                {
                    RwResourcesFreeResEntry(newAtomic->repEntry);
                }
                RpAtomicSetGeometry(newAtomic, (RpGeometry *)NULL, 0);
                rwObjectHasFrameReleaseFrame(newAtomic);
                RwFreeListFree(RWCLUMPGLOBAL(atomicFreeList), newAtomic);
                RWERROR((E_RP_WORLD_ATOMICNOFRAME));
                break;
            }

            rwObjectHasFrameSetFrame(newAtomic, frame->root);
            rwObjectSetPrivateFlags(newAtomic, rwObjectGetPrivateFlags(newAtomic) | rpATOMICPRIVATEWORLDBOUNDDIRTY);

            RpClumpAddAtomic(clumpCloneStatus.newClump, newAtomic);

            cur = next;
        }
    }

    _rwFramePurgeClone(RpClumpGetFrame(clumpCloneStatus.oldClump));

    if (!clumpCloneStatus.success)
    {
        ClumpCloneDestroyNew(clumpCloneStatus.newClump, FALSE);

        return ((RpClump *)NULL);
    }

    clumpCloneStatus.newClump->callback = clumpCloneStatus.oldClump->callback;

    rwPluginRegistryCopyObject(&clumpTKList,
                               clumpCloneStatus.newClump,
                               clumpCloneStatus.oldClump);

    return (clumpCloneStatus.newClump);
}
