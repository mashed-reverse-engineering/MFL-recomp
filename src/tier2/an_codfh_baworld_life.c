/* baworld lifecycle (MFL.exe 0x004f5420 / 0x004f56f0 / 0x004f5820 / 0x004f5970): RenderWare Graphics 3.7 (Criterion Software):
 * RpWorldUnlock, RpWorldSectorGetWorld, RpWorldDestroy and RpWorldCreate.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), no THREADSAFE_CHECK (codfh threading
 * patch, not in the exe), no RWDEBUG light / clump removal; as the exe has them inlined: the static WorldBuildMeshAtomicSector into
 * RpWorldUnlock, _rpWorldUnregisterWorld and _rpWorldLock into RpWorldDestroy, _rpWorldRegisterWorld, RpWorldSetRenderOrder,
 * RpWorldSetFlags, RpWorldSetSectorRenderCallBack and the whole RpWorldDestroy (failure path) into RpWorldCreate (WORLD_DESTROY),
 * RpWorldForAllWorldSectors with the static WorldFindSector callback into RpWorldSectorGetWorld (the exe walks the root without a
 * NULL test);
 * RpWorldGetNumMaterials / RpMaterialGetTexture / RwTextureGetRaster / RpMaterialGetPipeline / RpWorldGetFlags / rwObjectInitialize
 * / rwLinkList macros written out; RwMalloc / RwFree / RwFreeListAlloc / RwFreeListFree as the engine's memory function slots
 * (0x108 / 0x10c / 0x118 / 0x11c), RPWORLDGLOBAL as the rpWorldGlobals block at worldModule.globalsOffset in *RwEngineInstance;
 * RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN; the sector NOMEM reports
 * sizeof(sectorTKList.sizeOfStruct) = 4 as codfh does); memset of the texture coordinate pointers as a loop; structs declared here
 * (codfh baworld.h / basector.h / bamatlst.h / bamateri.h) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwLLLink {
    struct RwLLLink *next;
    struct RwLLLink *prev;
} RwLLLink;
typedef struct RwLinkList {
    RwLLLink link;
} RwLinkList;
typedef struct RwTextureHead {          /* RwTexture: raster first */
    void *raster;
} RwTextureHead;
typedef struct RpMaterial {
    RwTextureHead *texture;
    RwRGBA color;
    void *pipeline;                     /* RxPipeline * */
} RpMaterial;
typedef struct RpMaterialList {
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;
typedef struct RpSector {
    RwInt32 type;
} RpSector;
typedef struct RpPlaneSector {
    RwInt32 type;
    RwReal value;
    RpSector *leftSubTree;
    RpSector *rightSubTree;
    RwReal leftValue;
    RwReal rightValue;
} RpPlaneSector;
typedef struct RpWorldSector {
    RwInt32 type;
    RpTriangle *triangles;
    RwV3d *vertices;
    void *normals;                      /* RpVertexNormal * */
    void *texCoords[8];                 /* RwTexCoords *[rwMAXTEXTURECOORDS] */
    RwRGBA *preLitLum;
    void *repEntry;                     /* RwResEntry * */
    RwLinkList collAtomicsInWorldSector;
    RwLinkList lightsInWorldSector;
    RwBBox boundingBox;
    RwBBox tightBoundingBox;
    void *mesh;                         /* RpMeshHeader * */
    void *pipeline;                     /* RxPipeline * */
    RwUInt16 matListWindowBase;
    RwUInt16 numVertices;
    RwUInt16 numTriangles;
    RwUInt16 pad;
} RpWorldSector;
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
    RwLLLink *currentClumpLink;
    RwLinkList clumpList;
    RwLinkList lightList;
    RwLinkList directionalLightList;
    RwV3d worldOrigin;
    RwBBox boundingBox;
    void *renderCallBack;
    void *pipeline;                     /* RxPipeline * */
} RpWorld;
typedef struct RpListedWorld {
    RpWorld *world;
    RwUInt32 memorySize;
    RwLLLink nextEl;
} RpListedWorld;
typedef struct rpWorldGlobals {
    void *worldListFreeList;            /* RwFreeList * */
    RwLinkList worldListHead;
} rpWorldGlobals;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
} RwPluginRegistry;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_mat_pipeline, RW_OFFSETOF(RpMaterial, pipeline) == 8);
RW_LAYOUT_ASSERT(rw_assert_plane_right, RW_OFFSETOF(RpPlaneSector, rightSubTree) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_sector_prelit, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_sector_coll, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);
RW_LAYOUT_ASSERT(rw_assert_sector_bbox, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);
RW_LAYOUT_ASSERT(rw_assert_sector_mesh, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);
RW_LAYOUT_ASSERT(rw_assert_sector_matbase, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);
RW_LAYOUT_ASSERT(rw_assert_sector_numtri, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);
RW_LAYOUT_ASSERT(rw_assert_world_matlist, RW_OFFSETOF(RpWorld, matList) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_world_root, RW_OFFSETOF(RpWorld, rootSector) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_world_clumps, RW_OFFSETOF(RpWorld, clumpList) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_world_origin, RW_OFFSETOF(RpWorld, worldOrigin) == 0x44);
RW_LAYOUT_ASSERT(rw_assert_world_bbox, RW_OFFSETOF(RpWorld, boundingBox) == 0x50);
RW_LAYOUT_ASSERT(rw_assert_world_render, RW_OFFSETOF(RpWorld, renderCallBack) == 0x68);
RW_LAYOUT_ASSERT(rw_assert_listed_link, RW_OFFSETOF(RpListedWorld, nextEl) == 8);

#define rpWORLD 7
#define rpWORLDSINGLEMALLOC 0x01        /* privateFlags */
#define rpWORLDTRISTRIP 0x01            /* flags */
#define rpMESHHEADERTRISTRIP 0x0001
#define rpWORLDRENDERBACK2FRONT 2
#define rwSECTORATOMIC (-1)
#define rpWORLDMAXBSPDEPTH 64
#define rwID_TEXTURE 0x06
#define rwID_WORLD 0x0b
#define rwID_WORLDMODULE 0x0507
#define rwID_WORLDPLUGIN 0x2
#define rwMEMHINTDUR_FUNCTION 0x00010000
#define rwMEMHINTDUR_EVENT 0x00030000
#define rwMEMHINTDUR_GLOBAL 0x00040000
#define E_RW_NOMEM ((RwInt32)0x80000013)

typedef void *(*RwFreeListAllocFunc)(void *freeList, RwUInt32 hint);
typedef void *(*RwFreeListFreeFunc)(void *freeList, void *entry);

/*007d2128*/ extern void *RwEngineInstance;
/*007d52fc*/ extern RwModuleInfo worldModule;
/*0062ebec*/ extern RwPluginRegistry worldTKList;
/*0062ef88*/ extern RwPluginRegistry sectorTKList;

#define RW_ENGINE_SLOT(off, type) (*(type *)((RwUInt8 *)RwEngineInstance + (off)))
/* RwMalloc / RwFree (engine slots 0x108 / 0x10c) come from mfl_tier2.h */
#define RwFreeListAlloc RW_ENGINE_SLOT(0x118, RwFreeListAllocFunc)
#define RwFreeListFree RW_ENGINE_SLOT(0x11c, RwFreeListFreeFunc)
#define RPWORLDGLOBALS ((rpWorldGlobals *)((RwUInt8 *)RwEngineInstance + worldModule.globalsOffset))

/*00500d10*/ extern void *_rpBuildMeshCreate(RwUInt32 bufferSize);
/*00500e00*/ extern RwBool _rpBuildMeshDestroy(void *buildMesh);
/*00500e40*/ extern RwBool _rpMeshDestroy(void *mesh);
/*00500e80*/ extern void *_rpBuildMeshAddTriangle(void *buildMesh, RpMaterial *material, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3,
                                                  RwUInt16 matIndex, RwUInt16 textureIndex, RwUInt16 rasterIndex, RwUInt16 pipelineIndex);
/*00503d30*/ extern void *_rpMeshOptimise(void *buildMesh, RwUInt32 flags);
/*00503d90*/ extern RpMaterialList *_rpMaterialListDeinitialize(RpMaterialList *matList);
/*00503df0*/ extern RpMaterialList *_rpMaterialListInitialize(RpMaterialList *matList);
/*004f5090*/ extern void _rpWorldSectorDeinstanceAll(RpSector *sector);
/*004f5140*/ extern void _rpWorldSectorDestroyRecurse(RpSector *sector);
/*004f52b0*/ extern RpWorldSector *_rpSectorDefaultRenderCallBack(RpWorldSector *sector);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(const RwPluginRegistry *reg, void *object);
/*004e9d40*/ extern RwBool _rwPluginRegistryDeInitObject(const RwPluginRegistry *reg, void *object);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RWERROR_SET(args) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror args; RwErrorSet(&error); }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpWorld *
RpWorldUnlock(RpWorld *world)
{
    RpSector *sector;
    RpSector *sectorStack[rpWORLDMAXBSPDEPTH];
    RwInt32 stack = 0;

    /* Start at the top */
    sector = world->rootSector;

    if (sector == NULL)
        return (RpWorld *)NULL;

    do
    {
        if (sector->type < 0)
        {
            RpWorldSector *worldSector = (RpWorldSector *)sector;
            RpMaterial **matBase = &((world->matList.materials)[worldSector->matListWindowBase]);

            /* Its an atomic sector */
            if (!worldSector->mesh)
            {
                RwUInt32 numMaterials;
                void *buildMesh;
                void *mesh;

                /* Create a mesh for this sector (provide a useful hint as to size) */
                buildMesh = _rpBuildMeshCreate(worldSector->numTriangles);
                if (NULL == buildMesh)
                {
                    /* Failed to generate the mesh - fail the world unlock */
                    return (RpWorld *)NULL;
                }

                /* WorldBuildMeshAtomicSector(world, buildMesh, worldSector, matBase) */
                numMaterials = world->matList.numMaterials;
                if (numMaterials)
                {
                    RwInt32 i;
                    RwTextureHead **textureArray;
                    void **rasterArray;
                    void **pipelineArray;
                    RwUInt16 numTex = 0;
                    RwUInt16 numRas = 0;
                    RwUInt16 numPip = 0;

                    textureArray = (RwTextureHead **)RwMalloc(sizeof(RwTextureHead *) * numMaterials, rwID_TEXTURE | rwMEMHINTDUR_FUNCTION);
                    rasterArray = (void **)RwMalloc(sizeof(void *) * numMaterials, rwMEMHINTDUR_FUNCTION | rwID_WORLDMODULE);
                    pipelineArray = (void **)RwMalloc(sizeof(void *) * numMaterials, rwMEMHINTDUR_FUNCTION | rwID_WORLDMODULE);

                    for (i = 0; i < worldSector->numTriangles; ++i)
                    {
                        RpTriangle *tri;
                        RpMaterial *material;
                        RwUInt16 texIndex, rasIndex, pipIndex;
                        void *pipeline;
                        RwTextureHead *texture;
                        void *raster = NULL;

                        tri = &worldSector->triangles[i];
                        material = matBase[tri->matIndex];

                        texture = material->texture;
                        for (texIndex = 0; texIndex < numTex; ++texIndex)
                        {
                            if (textureArray[texIndex] == texture)
                            {
                                /* Found texture break out the loop */
                                break;
                            }
                        }
                        /* Was the texture found, if not add it */
                        if (texIndex == numTex)
                        {
                            textureArray[texIndex] = texture;
                            ++numTex;
                        }
                        if (texture)
                        {
                            raster = texture->raster;
                        }

                        for (rasIndex = 0; rasIndex < numRas; ++rasIndex)
                        {
                            if (rasterArray[rasIndex] == raster)
                            {
                                /* Found raster break out the loop */
                                break;
                            }
                        }
                        /* Was the raster found, if not add it */
                        if (rasIndex == numRas)
                        {
                            rasterArray[rasIndex] = raster;
                            ++numRas;
                        }

                        pipeline = material->pipeline;
                        for (pipIndex = 0; pipIndex < numPip; ++pipIndex)
                        {
                            if (pipelineArray[pipIndex] == pipeline)
                            {
                                /* Found pipeline break out the loop */
                                break;
                            }
                        }
                        /* Was the pipeline found, if not add it */
                        if (pipIndex == numPip)
                        {
                            pipelineArray[pipIndex] = pipeline;
                            ++numPip;
                        }

                        _rpBuildMeshAddTriangle(buildMesh, material,
                                                tri->vertIndex[0], tri->vertIndex[1],
                                                tri->vertIndex[2], tri->matIndex,
                                                texIndex, rasIndex, pipIndex);
                    }

                    RwFree(textureArray);
                    RwFree(rasterArray);
                    RwFree(pipelineArray);
                }

                if (rpWORLDTRISTRIP & world->flags)
                {
                    mesh = _rpMeshOptimise(buildMesh, rpMESHHEADERTRISTRIP);
                }
                else
                {
                    mesh = _rpMeshOptimise(buildMesh, 0);
                }

                if (!mesh)
                {
                    /* Failed to generate the mesh - fail the world unlock */
                    _rpBuildMeshDestroy(buildMesh);
                    return (RpWorld *)NULL;
                }
                worldSector->mesh = mesh;
            }

            /* Stack back */
            sector = sectorStack[stack--];
        }
        else
        {
            /* Its a plane */
            RpPlaneSector *planeSector = (RpPlaneSector *)sector;

            /* Left then right */
            sector = planeSector->leftSubTree;
            sectorStack[++stack] = planeSector->rightSubTree;
        }
    }
    while (stack >= 0);

    return world;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpWorld *
RpWorldSectorGetWorld(const RpWorldSector *sector)
{
    RwLLLink *cur, *end;

    end = &RPWORLDGLOBALS->worldListHead.link;
    for (cur = end->next; cur != end; cur = cur->next)
    {
        const RpListedWorld *listedWorld = (const RpListedWorld *)((const RwUInt8 *)cur - RW_OFFSETOF(RpListedWorld, nextEl));

        if (listedWorld->world->privateFlags & rpWORLDSINGLEMALLOC)
        {
            /* We have a single malloced world, so check the pointer ranges */
            if ((const void *)sector >= (const void *)listedWorld->world)
            {
                /* Test against the end of the world */
                const void *endOfWorld = (const void *)((const RwUInt8 *)listedWorld->world + listedWorld->memorySize);

                if ((const void *)sector < endOfWorld)
                {
                    /* It's in range, this is the world */
                    return listedWorld->world;
                }
            }
            /* Not this world */
        }
        else
        {
            /* Traverse the tree to find the sector: RpWorldForAllWorldSectors(world, WorldFindSector, &findSectorData),
             * the callback stops the walk at the sector */
            RpSector *walk = listedWorld->world->rootSector;
            RpSector *sectorStack[rpWORLDMAXBSPDEPTH];
            RwInt32 stack = 0;

            do
            {
                if (walk->type < 0)
                {
                    if ((const RpWorldSector *)walk == sector)
                    {
                        /* It's this one */
                        return listedWorld->world;
                    }
                    walk = sectorStack[stack--];
                }
                else
                {
                    RpPlaneSector *planeSector = (RpPlaneSector *)walk;

                    walk = planeSector->leftSubTree;
                    sectorStack[++stack] = planeSector->rightSubTree;
                }
            }
            while (stack >= 0);
        }
    }

    /* Not found */
    return (RpWorld *)NULL;
}

/* RpWorldDestroy body (also RpWorldCreate's failure path, where the exe inlines it) */
#define WORLD_DESTROY(world)                                                        \
    {                                                                               \
        RwLLLink *cur_, *end_;                                                      \
        RpSector *sector_;                                                          \
        RpSector *sectorStack_[rpWORLDMAXBSPDEPTH];                                 \
        RwInt32 stack_ = 0;                                                         \
                                                                                    \
        /* _rpWorldUnregisterWorld: remove it from the list of worlds */            \
        end_ = &RPWORLDGLOBALS->worldListHead.link;                                 \
        for (cur_ = end_->next; cur_ != end_; cur_ = cur_->next)                    \
        {                                                                           \
            RpListedWorld *listedWorld_ = (RpListedWorld *)((RwUInt8 *)cur_ - RW_OFFSETOF(RpListedWorld, nextEl)); \
                                                                                    \
            if (listedWorld_->world == (world))                                     \
            {                                                                       \
                /* Remove it and get outta here */                                  \
                listedWorld_->nextEl.prev->next = listedWorld_->nextEl.next;        \
                listedWorld_->nextEl.next->prev = listedWorld_->nextEl.prev;        \
                RwFreeListFree(RPWORLDGLOBALS->worldListFreeList, listedWorld_);    \
                break;                                                              \
            }                                                                       \
        }                                                                           \
                                                                                    \
        /* _rpWorldLock: lock it so that there is no device dependent information */ \
        sector_ = (world)->rootSector;                                              \
        if (sector_)                                                                \
        {                                                                           \
            do                                                                      \
            {                                                                       \
                if (sector_->type < 0)                                              \
                {                                                                   \
                    RpWorldSector *worldSector_ = (RpWorldSector *)sector_;         \
                                                                                    \
                    /* Its an atomic sector */                                      \
                    if (worldSector_->mesh)                                         \
                    {                                                               \
                        _rpMeshDestroy(worldSector_->mesh);                         \
                        worldSector_->mesh = NULL;                                  \
                    }                                                               \
                    sector_ = sectorStack_[stack_--];                               \
                }                                                                   \
                else                                                                \
                {                                                                   \
                    /* Its a plane: left then right */                              \
                    RpPlaneSector *planeSector_ = (RpPlaneSector *)sector_;         \
                    sector_ = planeSector_->leftSubTree;                            \
                    sectorStack_[++stack_] = planeSector_->rightSubTree;            \
                }                                                                   \
            }                                                                       \
            while (stack_ >= 0);                                                    \
        }                                                                           \
                                                                                    \
        /* Material list */                                                         \
        _rpMaterialListDeinitialize(&(world)->matList);                             \
                                                                                    \
        /* Destroy the BSP tree */                                                  \
        if ((world)->privateFlags & rpWORLDSINGLEMALLOC)                            \
        {                                                                           \
            /* Its all in one big chunk */                                          \
            if ((world)->rootSector)                                                \
                _rpWorldSectorDeinstanceAll((world)->rootSector);                   \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            /* This calls destructors on sectors too */                             \
            if ((world)->rootSector)                                                \
                _rpWorldSectorDestroyRecurse((world)->rootSector);                  \
        }                                                                           \
                                                                                    \
        /* Call the destructor */                                                   \
        _rwPluginRegistryDeInitObject(&worldTKList, (world));                       \
        RwFree((world));                                                            \
    }

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
RpWorldDestroy(RpWorld *world)
{
    WORLD_DESTROY(world);

    /* Done */
    return TRUE;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpWorld *
RpWorldCreate(RwBBox *boundingBox)
{
    RpWorld *world;
    RpWorldSector *worldSector;
    RpListedWorld *listedWorld;
    RwInt32 i;

    world = (RpWorld *)RwMalloc(worldTKList.sizeOfStruct, rwID_WORLD | rwMEMHINTDUR_EVENT);
    if (!world)
    {
        RWERROR_SET((E_RW_NOMEM, worldTKList.sizeOfStruct));
        return (RpWorld *)NULL;
    }

    /* Setup the world type: rwObjectInitialize(world, rpWORLD, 0) */
    world->type = rpWORLD;
    world->subType = 0;
    world->objFlags = 0;
    world->privateFlags = 0;
    world->parent = NULL;
    _rpMaterialListInitialize(&world->matList);

    /* RpWorldSetFlags(world, 0), the default rendering order */
    world->flags = 0;
    world->renderOrder = rpWORLDRENDERBACK2FRONT;

    /* Set the atomic sector */
    worldSector = (RpWorldSector *)RwMalloc(sectorTKList.sizeOfStruct, rwID_WORLD | rwMEMHINTDUR_EVENT);
    if (!worldSector)
    {
        RWERROR_SET((E_RW_NOMEM, sizeof(sectorTKList.sizeOfStruct)));
        RwFree(world);
        return (RpWorld *)NULL;
    }

    /* Mark it as atomic */
    worldSector->type = rwSECTORATOMIC;

    /* Not instanced */
    worldSector->repEntry = NULL;
    worldSector->mesh = NULL;

    /* Set so it contains nothing */
    worldSector->collAtomicsInWorldSector.link.next = &worldSector->collAtomicsInWorldSector.link;
    worldSector->collAtomicsInWorldSector.link.prev = &worldSector->collAtomicsInWorldSector.link;
    worldSector->lightsInWorldSector.link.next = &worldSector->lightsInWorldSector.link;
    worldSector->lightsInWorldSector.link.prev = &worldSector->lightsInWorldSector.link;

    worldSector->numVertices = 0;
    worldSector->numTriangles = 0;
    worldSector->vertices = NULL;
    worldSector->triangles = NULL;
    worldSector->normals = NULL;

    /* Set up texture coords */
    for (i = 0; i < 8; i++)
    {
        worldSector->texCoords[i] = NULL;
    }

    worldSector->preLitLum = NULL;
    worldSector->boundingBox.inf = boundingBox->inf;
    worldSector->boundingBox.sup = boundingBox->sup;
    worldSector->tightBoundingBox.inf = boundingBox->inf;
    worldSector->tightBoundingBox.sup = boundingBox->sup;

    /* Use the default world sector object pipeline */
    worldSector->pipeline = NULL;

    world->rootSector = (RpSector *)worldSector;

    /* Set the number of texture coordinate sets in the world to zero */
    world->numTexCoordSets = 0;

    /* Set the world's offset to zero */
    world->worldOrigin.x = world->worldOrigin.y = world->worldOrigin.z = (RwReal)0.0f;

    /* Set the world's bounding boxes */
    world->boundingBox.inf = boundingBox->inf;
    world->boundingBox.sup = boundingBox->sup;

    /* There are no atomics or clumps in the world */
    world->clumpList.link.next = &world->clumpList.link;
    world->clumpList.link.prev = &world->clumpList.link;
    world->numClumpsInWorld = 0;
    world->currentClumpLink = &world->clumpList.link;

    /* Initialize (non directional) lights linked list, directional lights in world */
    world->lightList.link.next = &world->lightList.link;
    world->lightList.link.prev = &world->lightList.link;
    world->directionalLightList.link.next = &world->directionalLightList.link;
    world->directionalLightList.link.prev = &world->directionalLightList.link;

    /* Reset its instance callback: RpWorldSetSectorRenderCallBack(world, NULL) */
    world->renderCallBack = (void *)_rpSectorDefaultRenderCallBack;

    /* use the default world sector object pipeline */
    world->pipeline = NULL;

    /* _rpWorldRegisterWorld(world, worldTKList.sizeOfStruct): add it to the list of worlds */
    listedWorld = (RpListedWorld *)RwFreeListAlloc(RPWORLDGLOBALS->worldListFreeList, rwMEMHINTDUR_GLOBAL | rwID_WORLDMODULE);
    if (listedWorld)
    {
        listedWorld->world = world;
        listedWorld->memorySize = worldTKList.sizeOfStruct;
        listedWorld->nextEl.next = RPWORLDGLOBALS->worldListHead.link.next;
        listedWorld->nextEl.prev = &RPWORLDGLOBALS->worldListHead.link;
        RPWORLDGLOBALS->worldListHead.link.next->prev = &listedWorld->nextEl;
        RPWORLDGLOBALS->worldListHead.link.next = &listedWorld->nextEl;
    }

    /* Call the constructors */
    _rwPluginRegistryInitObject(&worldTKList, world);
    _rwPluginRegistryInitObject(&sectorTKList, worldSector);

    /* Give the device a look in */
    if (!RpWorldUnlock(world))
    {
        /* Failed to unlock, fail the world creation */
        WORLD_DESTROY(world);
        return (RpWorld *)NULL;
    }

    /* Done */
    return world;
}
