/* RpClumpDestroy (MFL.exe 004f6f40). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * callbacks DestroyClumpAtomic / DestroyClumpLight / DestroyClumpCamera and the RpClumpForAll* walks inlined by hand (MFL.exe has them
 * inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLLLinkInitialize(linkvar) ( (linkvar)->prev = (RwLLLink *)NULL, (linkvar)->next = (RwLLLink *)NULL )
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
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
extern const
RwPluginRegistry *_rwPluginRegistryDeInitObject(const RwPluginRegistry * reg,
                                               void *object);
#define rwPluginRegistryDeInitObject(reg,object) _rwPluginRegistryDeInitObject(reg, object)
typedef void (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink link;
    RwInt32 size;
    void *owner;
    RwResEntry **ownerRef;
    RwResEntryDestroyNotify destroyNotify;
};
extern RwBool RwResourcesFreeResEntry(RwResEntry * entry);
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
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
extern void _rwObjectHasFrameReleaseFrame(void *object);
#define rwObjectHasFrameReleaseFrame(object) _rwObjectHasFrameReleaseFrame(object)
extern RwBool RwCameraDestroy(RwCamera * camera);
extern RwBool
RpGeometryDestroy(RpGeometry *geometry);
typedef struct RpVertexNormal RpVertexNormal;
struct RpWorldSector
{
    RwInt32 type;

    RpTriangle *triangles;

    RwV3d *vertices;
    RpVertexNormal *normals;

    RwTexCoords *texCoords[rwMAXTEXTURECOORDS];

    RwRGBA *preLitLum;

    RwResEntry *repEntry;

    RwLinkList collAtomicsInWorldSector;

    RwLinkList lightsInWorldSector;

    RwBBox boundingBox;

    RwBBox tightBoundingBox;

    RpMeshHeader *mesh;

    RxPipeline *pipeline;

    RwUInt16 matListWindowBase;

    RwUInt16 numVertices;
    RwUInt16 numTriangles;
    RwUInt16 pad;
};
typedef struct RpLight RpLight;
extern RwBool RpLightDestroy(RpLight *light);
#define RpClumpGetFrameMacro(_clump) ((RwFrame *) rwObjectGetParent(_clump))
#define RpClumpGetFrame(_clump) RpClumpGetFrameMacro(_clump)
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwResEntry, size) == 0x8);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwResEntry, owner) == 0xc);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RwResEntry, ownerRef) == 0x10);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RwResEntry, destroyNotify) == 0x14);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RwResEntry) == 0x18);      /* an_codfh_RwResourcesAllocateResEntry.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_14, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_15, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_16, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_17, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_18, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_19, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_20, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_21, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_22, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_23, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_24, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_25, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_26, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
RW_LAYOUT_ASSERT(rw37_row_27, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
/* ---- end of the generated declarations ---- */

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
#define CLUMPCAMERAEXTFROMCAMERA(camera) ((RpClumpCameraExt *)(((RwUInt8 *)(camera)) + _rpClumpCameraExtOffset))
#define CLUMPCAMERAEXTFROMCONSTCAMERA(camera) ((const RpClumpCameraExt *)(((const RwUInt8 *)(camera)) + _rpClumpCameraExtOffset))
#define RWCLUMPGLOBAL(var) (RWPLUGINOFFSET(rpClumpGlobals, RwEngineInstance, clumpModule.globalsOffset)->var)
extern RwPluginRegistry clumpTKList;
extern RwInt32  _rpClumpCameraExtOffset;
extern RwModuleInfo clumpModule;
typedef struct RpClumpLightExt RpClumpLightExt;
struct RpClumpLightExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
extern RwInt32  _rpClumpLightExtOffset;
extern RwPluginRegistry atomicTKList;
extern RpWorld *RpAtomicGetWorld(const RpAtomic *atomic);
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpClumpGlobals, clumpFreeList) == 4 && RW_OFFSETOF(RpClumpLightExt, inClumpLink) == 4 && RW_OFFSETOF(RpClumpCameraExt, inClumpLink) == 4);      /* MFL.exe 0x4f6fcb atomic free list [globals+0], 0x4f70a1 clump free list [globals+4]; 0x4f6ffd / 0x4f704b: ext = link - offset - 4 */

/* TIER2: unvalidated authority=diffemu */
RwBool RpClumpDestroy(RpClump * clump)
{
    RwLLLink           *cur, *end, *next;
    RwFrame            *frame;

    rwPluginRegistryDeInitObject(&clumpTKList, clump);

    /* RpClumpForAllAtomics(clump, DestroyClumpAtomic, NULL) with DestroyClumpAtomic = RpAtomicDestroy (and its
     * RpAtomicSetGeometry(atomic, NULL, 0)) inlined in MFL.exe (0x4f6f53..0x4f6fe3) */
    cur = rwLinkListGetFirstLLLink(&clump->atomicList);
    end = rwLinkListGetTerminator(&clump->atomicList);
    while (cur != end)
    {
        RpAtomic           *atomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

        next = rwLLLinkGetNext(cur);

        rwPluginRegistryDeInitObject(&atomicTKList, atomic);
        if (atomic->repEntry)
        {
            RwResourcesFreeResEntry(atomic->repEntry);
        }
        if (atomic->geometry)
        {
            RwFrame *atomicFrame;

            RpGeometryDestroy(atomic->geometry);
            atomicFrame = RpAtomicGetFrame(atomic);
            atomic->geometry = (RpGeometry *)NULL;
            if (atomicFrame && RpAtomicGetWorld(atomic))
            {
                RwFrameUpdateObjects(atomicFrame);
            }
        }
        rwObjectHasFrameReleaseFrame(atomic);
        RwFreeListFree(RWCLUMPGLOBAL(atomicFreeList), atomic);

        cur = next;
    }

    /* RpClumpForAllLights(clump, DestroyClumpLight, NULL) with DestroyClumpLight / RpClumpRemoveLight inlined (0x4f6fe9..0x4f7031) */
    cur = rwLinkListGetFirstLLLink(&clump->lightList);
    end = rwLinkListGetTerminator(&clump->lightList);
    while (cur != end)
    {
        RpClumpLightExt    *lightExt = rwLLLinkGetData(cur, RpClumpLightExt, inClumpLink);
        RpLight            *light = (RpLight *)((RwUInt8 *)lightExt - _rpClumpLightExtOffset);

        next = rwLLLinkGetNext(cur);
        rwLinkListRemoveLLLink(&lightExt->inClumpLink);
        rwLLLinkInitialize(&lightExt->inClumpLink);
        lightExt->clump = (RpClump *)NULL;
        RpLightDestroy(light);
        if (!light)
        {
            break;
        }
        cur = next;
    }

    /* RpClumpForAllCameras(clump, DestroyClumpCamera, NULL) with DestroyClumpCamera / RpClumpRemoveCamera inlined (0x4f7037..0x4f707f) */
    cur = rwLinkListGetFirstLLLink(&clump->cameraList);
    end = rwLinkListGetTerminator(&clump->cameraList);
    while (cur != end)
    {
        RpClumpCameraExt   *cameraExt = rwLLLinkGetData(cur, RpClumpCameraExt, inClumpLink);
        RwCamera           *camera = (RwCamera *)((RwUInt8 *)cameraExt - _rpClumpCameraExtOffset);

        next = rwLLLinkGetNext(cur);
        rwLinkListRemoveLLLink(&cameraExt->inClumpLink);
        rwLLLinkInitialize(&cameraExt->inClumpLink);
        cameraExt->clump = (RpClump *)NULL;
        RwCameraDestroy(camera);
        if (!camera)
        {
            break;
        }
        cur = next;
    }

    frame = RpClumpGetFrame(clump);
    if (frame)
    {
        RwFrameDestroyHierarchy(frame);
    }

    RwFreeListFree(RWCLUMPGLOBAL(clumpFreeList), clump);

    return (TRUE);
}
