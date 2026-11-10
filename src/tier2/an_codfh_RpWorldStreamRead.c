/* RpWorldStreamRead (MFL.exe 0x004f9bd0): RenderWare Graphics 3.7 (Criterion Software), reads a binary world (struct chunk, material
 * list, the BSP of plane / world sectors, extension chunks) from a stream into one allocation.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/babinwor.c @e578672,
 * not byte-identical, changes: the PC 3.7 signature (no codfh world arena: no arena argument, RwMalloc, every failure after the
 * allocation is RpWorldDestroy, also the extension chunk failure), no 3.4 header / bounding box compatibility code (dead after the
 * version check, not in the exe), PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone), RWAPIFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN),
 * rwObjectInitialize / rwObjectSetPrivateFlags / RpWorldSetFlags / RpWorldSetRenderOrder / RwV3dScale / rwLinkListInitialize /
 * RpMaterialGetTexture written out, _rpWorldUnlock is the exe's RpWorldUnlock, the structs declared here (codfh babinwor.h /
 * baworld.h / baerr.h) with compile-time layout asserts, memset as a zero initialiser / a byte loop.  Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpWorld {               /* RpWorldChunkInfo */
    RwBool rootIsWorldSector;
    RwV3d invWorldOrigin;
    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numPlaneSectors;
    RwInt32 numWorldSectors;
    RwInt32 colSectorSize;
    RwInt32 format;                     /* Flags about the world */
    RwBBox boundingBox;                 /* Added in 34003 */
} _rpWorld;
typedef struct RwLinkList {
    void *next;                         /* RwLLLink link: next, prev */
    void *prev;
} RwLinkList;
typedef struct RpMaterial {
    RwTexture *texture;
} RpMaterial;
typedef struct RpMaterialList {
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;
typedef struct RpSector RpSector;
typedef struct RpWorld {
    RwUInt8 type;                       /* RwObject */
    RwUInt8 subType;
    RwUInt8 objFlags;
    RwUInt8 privateFlags;
    void *parent;
    RwUInt32 flags;
    RwInt32 renderOrder;
    RpMaterialList matList;
    RpSector *rootSector;
    RwInt32 numTexCoordSets;
    RwInt32 numClumpsInWorld;
    void *currentClumpLink;             /* RwLLLink * */
    RwLinkList clumpList;
    RwLinkList lightList;
    RwLinkList directionalLightList;
    RwV3d worldOrigin;
    RwBBox boundingBox;
    void *renderCallBack;
    void *pipeline;                     /* RxPipeline * */
} RpWorld;
typedef struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
} RwPluginRegistry;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_rpworld_size, sizeof(_rpWorld) == 0x40);
RW_LAYOUT_ASSERT(rw_assert_world_flags, RW_OFFSETOF(RpWorld, flags) == 8);
RW_LAYOUT_ASSERT(rw_assert_world_matlist, RW_OFFSETOF(RpWorld, matList) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_world_root, RW_OFFSETOF(RpWorld, rootSector) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_world_clumps, RW_OFFSETOF(RpWorld, clumpList) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_world_origin, RW_OFFSETOF(RpWorld, worldOrigin) == 0x44);
RW_LAYOUT_ASSERT(rw_assert_world_bbox, RW_OFFSETOF(RpWorld, boundingBox) == 0x50);
RW_LAYOUT_ASSERT(rw_assert_world_pipeline, RW_OFFSETOF(RpWorld, pipeline) == 0x6c);

#define rwID_STRUCT 0x1
#define rwID_MATLIST 0x8
#define rwID_ATOMICSECT 0x9
#define rwID_PLANESECT 0xa
#define rwID_WORLD 0xb
#define rwID_WORLDPLUGIN 0x2
#define rwMEMHINTDUR_EVENT 0x30000
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rpWORLD 7
#define rpWORLDSINGLEMALLOC 0x01
#define rpWORLDRENDERBACK2FRONT 2
#define rpWORLDTEXTURED 0x00000004
#define rpWORLDPRELIT 0x00000008
#define rpWORLDNORMALS 0x00000010
#define rpWORLDTEXTURED2 0x00000080
#define rpWORLDNATIVE 0x01000000
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_NOMEM ((RwInt32)0x80000013)
#define WorldFormatGetNumTexCoordSetsMacro(_fmt)                \
    (((_fmt) & 0xff0000) ? (((_fmt) & 0xff0000) >> 16) :        \
        (((_fmt) & rpWORLDTEXTURED2) ? 2 :                      \
            (((_fmt) & rpWORLDTEXTURED) ? 1 : 0)))

/*007d2128*/ extern void *RwEngineInstance;
/*0062ebec*/ extern RwPluginRegistry worldTKList;
/*0062ef88*/ extern RwPluginRegistry sectorTKList;
/*007d53a8*/ extern RwUInt32 lastSeenWorldRightsPluginId;
/*007d53a4*/ extern RwUInt32 lastSeenWorldExtraData;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004f53a0*/ extern void _rpWorldRegisterWorld(RpWorld *world, RwUInt32 memorySize);
/*005040c0*/ extern RpMaterialList *_rpMaterialListStreamRead(RwStream *stream, RpMaterialList *matList);
/*004fa060*/ extern void *WorldSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags);
/*004fa440*/ extern void *PlaneSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags);
/*004f5300*/ extern RpWorld *_rpWorldSetupSectorBoundingBoxes(RpWorld *world);
/*004f5940*/ extern RpWorld *RpWorldSetSectorRenderCallBack(RpWorld *world, void *fpCallBack);
/*004f8230*/ extern RpMaterial *RpMaterialSetTexture(RpMaterial *material, RwTexture *texture);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(const RwPluginRegistry *reg, void *object);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*004f1fd0*/ extern RwBool _rwPluginRegistryInvokeRights(const RwPluginRegistry *reg, RwUInt32 id, void *obj, RwUInt32 extraData);
/*004f5420*/ extern RpWorld *RpWorldUnlock(RpWorld *world);
/*004f5820*/ extern RwBool RpWorldDestroy(RpWorld *world);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(args) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror args; RwErrorSet(&error); }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpWorld *RpWorldStreamRead(RwStream *stream)
{
    RpWorld *world;
    _rpWorld w = { 0 };
    RwInt32 i;
    RwUInt8 *binaryWorldMallocAddr;
    RwInt32 worldSize;
    RwUInt32 size, worldVersion, version;
    RwInt32 flags;
    RwUInt32 numTexCoordSets;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &size, &worldVersion))
    {
        return NULL;
    }

    if ((worldVersion < rwLIBRARYBASEVERSION) || (worldVersion > rwLIBRARYCURRENTVERSION))
    {
        RWERROR_SET((E_RW_BADVERSION));
        return NULL;
    }

    /* Read it */
    if (RwStreamRead(stream, &w, size) != size)
    {
        return NULL;
    }

    if (w.colSectorSize > 0)
    {
        RWERROR_SET((E_RW_BADVERSION));
        return NULL;
    }

    /* Figure out how big it is */
    worldSize = worldTKList.sizeOfStruct;
    worldSize += w.numPlaneSectors * 0x18;      /* sizeof(RpPlaneSector) */
    worldSize += w.numWorldSectors * sectorTKList.sizeOfStruct;

    /* The format contains the flags, so just copy */
    flags = w.format;

    numTexCoordSets = WorldFormatGetNumTexCoordSetsMacro(w.format);

    if (!(rpWORLDNATIVE & flags))
    {
        worldSize += w.numVertices * sizeof(RwV3d);

        if (flags & (RwInt32)rpWORLDNORMALS)
        {
            worldSize += w.numVertices * sizeof(RwUInt32);      /* RpVertexNormal */
        }

        if (flags & (RwInt32)rpWORLDPRELIT)
        {
            worldSize += w.numVertices * sizeof(RwRGBA);
        }

        if (numTexCoordSets > 0)
        {
            /* Allow space for texture coordinates and pointer arrays in sectors */
            worldSize += numTexCoordSets * w.numVertices * sizeof(RwTexCoords);
        }

        worldSize += w.numTriangles * sizeof(RpTriangle);
    }

    /* Can just allocate for the whole world's memory */
    world = (RpWorld *)RwMalloc(worldSize, rwMEMHINTDUR_EVENT | rwID_WORLD);
    if (!world)
    {
        RWERROR_SET((E_RW_NOMEM, (worldSize)));
        return NULL;
    }

    /* Clear it to zero. This would set pointers to NULL (memset) */
    {
        RwUInt8 *bytes = (RwUInt8 *)world;
        RwUInt32 k;

        for (k = 0; k < (RwUInt32)worldSize; k++)
        {
            bytes[k] = 0;
        }
    }

    binaryWorldMallocAddr = (RwUInt8 *)world + worldTKList.sizeOfStruct;
    world->type = rpWORLD;      /* rwObjectInitialize(world, rpWORLD, 0) */
    world->subType = 0;
    world->objFlags = 0;
    world->privateFlags = 0;
    world->parent = NULL;
    world->privateFlags = rpWORLDSINGLEMALLOC;      /* rwObjectSetPrivateFlags */

    /* Add to the list */
    _rpWorldRegisterWorld(world, worldSize);

    /* Set the worlds flags */
    world->flags = flags;

    /* Set the default rendering order */
    world->renderOrder = rpWORLDRENDERBACK2FRONT;

    /* Set the world offset */
    RwV3dScaleMacro(&world->worldOrigin, &w.invWorldOrigin, (RwReal)(-1.0f));

    /* Set number of texture coordinates used in this world */
    world->numTexCoordSets = numTexCoordSets;

    /* No atomic sectors yet */
    world->rootSector = NULL;

    /* use the default world sector object pipeline */
    world->pipeline = NULL;

    /* Initialise the link lists */
    world->clumpList.next = &world->clumpList;
    world->clumpList.prev = &world->clumpList;
    world->lightList.next = &world->lightList;
    world->lightList.prev = &world->lightList;
    world->directionalLightList.next = &world->directionalLightList;
    world->directionalLightList.prev = &world->directionalLightList;

    /* Read all of the materials */
    if (!RwStreamFindChunk(stream, rwID_MATLIST, NULL, &version))
    {
        RpWorldDestroy(world);
        return NULL;
    }

    if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
    {
        RWERROR_SET((E_RW_BADVERSION));
        RpWorldDestroy(world);
        return NULL;
    }

    if (!_rpMaterialListStreamRead(stream, &world->matList))
    {
        RpWorldDestroy(world);
        return NULL;
    }

    /* Create all of the sectors */
    if (w.rootIsWorldSector)
    {
        /* We have an atomic sector */
        if (!RwStreamFindChunk(stream, rwID_ATOMICSECT, NULL, &version))
        {
            RpWorldDestroy(world);
            return NULL;
        }

        if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
        {
            RWERROR_SET((E_RW_BADVERSION));
            RpWorldDestroy(world);
            return NULL;
        }

        if (!(world->rootSector = (RpSector *)WorldSectorStreamRead(stream, &binaryWorldMallocAddr, world, flags)))
        {
            RpWorldDestroy(world);
            return NULL;
        }
    }
    else
    {
        /* We have a plane sector */
        if (!RwStreamFindChunk(stream, rwID_PLANESECT, NULL, &version))
        {
            RpWorldDestroy(world);
            return NULL;
        }

        if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
        {
            RWERROR_SET((E_RW_BADVERSION));
            RpWorldDestroy(world);
            return NULL;
        }

        if (!(world->rootSector = (RpSector *)PlaneSectorStreamRead(stream, &binaryWorldMallocAddr, world, flags)))
        {
            RpWorldDestroy(world);
            return NULL;
        }
    }

    world->numClumpsInWorld = 0;
    world->currentClumpLink = &world->clumpList;      /* rwLinkListGetTerminator */

    /* Set up the bbox */
    world->boundingBox = w.boundingBox;

    /* Set up the sector bounding boxes based on the BSP planes. These are
     * not the same as the tight bounding boxes that were streamed in
     */
    _rpWorldSetupSectorBoundingBoxes(world);

    /* Reset all its callback functions */
    RpWorldSetSectorRenderCallBack(world, NULL);

    /* Go through the mat list - make textured correct */
    for (i = 0; i < world->matList.numMaterials; i++)
    {
        RwTexture *tex;
        RpMaterial *mat = (world->matList).materials[i];

        tex = mat->texture;      /* RpMaterialGetTexture */

        if (tex)
        {
            RpMaterialSetTexture(mat, tex);
        }
    }

    /* Call the constructor on the world as late as we can (so world sectors are in place) */
    _rwPluginRegistryInitObject(&worldTKList, world);

    lastSeenWorldRightsPluginId = 0;
    lastSeenWorldExtraData = 0;

    /* Read in the plugin info */
    if (!_rwPluginRegistryReadDataChunks(&worldTKList, stream, world))
    {
        /* Failed to read extension data */
        RpWorldDestroy(world);
        return NULL;
    }

    if (lastSeenWorldRightsPluginId)
    {
        /* Silently ignore errors if any */
        _rwPluginRegistryInvokeRights(&worldTKList, lastSeenWorldRightsPluginId, world, lastSeenWorldExtraData);
    }

    /* Release the world so platform specific info can be set up */
    if (!RpWorldUnlock(world))
    {
        /* Failed to unlock, fail the world read */
        RpWorldDestroy(world);
        return NULL;
    }

    return world;
}
