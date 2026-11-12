/* WorldSectorStreamRead (MFL.exe 0x004fa060): RenderWare Graphics 3.7 (Criterion Software), reads an atomic world sector (struct
 * chunk with the vertex / normal / prelight / texture coordinate / triangle data, extension chunks) of a binary world from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/babinwor.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone; RwStreamReadReal / RwStreamReadInt16
 * are the exe's RwStreamReadInt32, the linker folded the identical functions), no codfh world-arena abort check, no RWDEBUG sort
 * check, RWFUNCTION / RWASSERT / RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID
 * rwID_WORLDPLUGIN), BinaryWorldMalloc / rwLinkListInitialize / rwObjectTestPrivateFlags written out (the exe inlines them), external
 * linkage (the exe's symbol), the structs declared here (codfh babinwor.h / basector.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpWorldSector {         /* RpWorldChunkInfoSector */
    RwInt32 matListWindowBase;
    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwV3d inf;
    RwV3d sup;
    RwBool collSectorPresent;           /* unused but retains same struct size */
    RwBool unused;
} _rpWorldSector;
typedef struct rpPolygon {              /* pre-3.6.0.2 triangle layout */
    RwUInt16 matIndex;
    RwUInt16 vertIndex[3];
} rpPolygon;
typedef struct RwLinkList {
    void *next;                         /* RwLLLink link: next, prev */
    void *prev;
} RwLinkList;
typedef struct RpWorldSector {
    RwInt32 type;
    RpTriangle *triangles;
    RwV3d *vertices;
    RwUInt32 *normals;                  /* RpVertexNormal * */
    RwTexCoords *texCoords[8];          /* rwMAXTEXTURECOORDS */
    RwRGBA *preLitLum;
    RwResEntry *repEntry;
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
typedef struct RpWorld {                /* the head: RwObject (privateFlags at +3), flags at +8, numTexCoordSets at +0x20 */
    RwUInt8 type;
    RwUInt8 subType;
    RwUInt8 objFlags;
    RwUInt8 privateFlags;
    void *parent;
    RwUInt32 flags;
    RwUInt8 pad[0x14];
    RwInt32 numTexCoordSets;
} RpWorld;
typedef struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
} RwPluginRegistry;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RpSector RpSector;
RW_LAYOUT_ASSERT(rw_assert_rpsector_size, sizeof(_rpWorldSector) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_poly_size, sizeof(rpPolygon) == sizeof(RpTriangle));
RW_LAYOUT_ASSERT(rw_assert_sector_texc, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_sector_prelit, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_sector_coll, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);
RW_LAYOUT_ASSERT(rw_assert_sector_tight, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);
RW_LAYOUT_ASSERT(rw_assert_sector_mesh, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);
RW_LAYOUT_ASSERT(rw_assert_sector_mlwb, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);
RW_LAYOUT_ASSERT(rw_assert_sector_ntri, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);
RW_LAYOUT_ASSERT(rw_assert_world_flags, RW_OFFSETOF(RpWorld, flags) == 8);
RW_LAYOUT_ASSERT(rw_assert_world_ntc, RW_OFFSETOF(RpWorld, numTexCoordSets) == 0x20);

#define TRUE 1
#define FALSE 0
#define rwID_STRUCT 0x1
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwLIBRARYVERSION36002 0x36002
#define rwSECTORATOMIC (-1)
#define rpWORLDPRELIT 0x00000008
#define rpWORLDNORMALS 0x00000010
#define rpWORLDNATIVE 0x01000000
#define rpWORLDSINGLEMALLOC 0x01
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_NOMEM ((RwInt32)0x80000013)

/*0062ef88*/ extern RwPluginRegistry sectorTKList;
/*007d53ac*/ extern RwUInt32 lastSeenSectRightsPluginId;
/*007d53a0*/ extern RwUInt32 lastSeenSectExtraData;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(const RwPluginRegistry *reg, void *object);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*004f1fd0*/ extern RwBool _rwPluginRegistryInvokeRights(const RwPluginRegistry *reg, RwUInt32 id, void *obj, RwUInt32 extraData);
/*004f5090*/ extern RpSector *_rpWorldSectorDeinstanceAll(RpSector *sector);
/*004f5140*/ extern RpSector *_rpWorldSectorDestroyRecurse(RpSector *sector);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RwStreamReadReal(stream, reals, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(reals), (numBytes))
#define RwStreamReadInt16(stream, shorts, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(shorts), (numBytes))
#define RWERROR_SET(args) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror args; RwErrorSet(&error); }
/* BinaryWorldMalloc: from the pool of pre-allocated memory */
#define BinaryWorldMalloc(addr, size) ((void *)((*(addr) += (size)) - (size)))

/* TIER2: validated 2026-11-10 authority=diffemu */
RpWorldSector *WorldSectorStreamRead(RwStream *stream, RwUInt8 **binaryWorldMallocAddr, RpWorld *world, RwUInt32 flags)
{
    RpWorldSector *worldSector;
    _rpWorldSector as;
    RwUInt32 version;
    RwInt32 i;
    RwBool result;

    result = TRUE;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, NULL, &version))
    {
        return NULL;
    }

    if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
    {
        RWERROR_SET((E_RW_BADVERSION));
        return NULL;
    }

    /* Read it */
    if (RwStreamRead(stream, &as, sizeof(as)) != sizeof(as))
    {
        return NULL;
    }

    /* Grab some memory */
    worldSector = (RpWorldSector *)BinaryWorldMalloc(binaryWorldMallocAddr, sectorTKList.sizeOfStruct);
    if (!worldSector)
    {
        RWERROR_SET((E_RW_NOMEM, (sectorTKList.sizeOfStruct)));
        return NULL;
    }

    /* Copy over values */
    worldSector->type = rwSECTORATOMIC;
    worldSector->tightBoundingBox.inf = as.inf;
    worldSector->tightBoundingBox.sup = as.sup;

    /* The 'Loose' bounding boxes are dynamically generated from the BSP planes at a later stage. */

    worldSector->matListWindowBase = (RwUInt16)as.matListWindowBase;
    worldSector->numTriangles = (RwUInt16)as.numTriangles;
    worldSector->numVertices = (RwUInt16)as.numVertices;
    worldSector->triangles = NULL;
    worldSector->vertices = NULL;
    worldSector->preLitLum = NULL;
    worldSector->normals = NULL;
    for (i = 0; i < 8; i++)
    {
        worldSector->texCoords[i] = NULL;
    }
    worldSector->pipeline = NULL;

    /* Not instanced !!! */
    worldSector->repEntry = NULL;
    worldSector->mesh = NULL;

    /* Set so it contains nothing (rwLinkListInitialize) */
    worldSector->collAtomicsInWorldSector.next = &worldSector->collAtomicsInWorldSector;
    worldSector->collAtomicsInWorldSector.prev = &worldSector->collAtomicsInWorldSector;
    worldSector->lightsInWorldSector.next = &worldSector->lightsInWorldSector;
    worldSector->lightsInWorldSector.prev = &worldSector->lightsInWorldSector;

    if (!(rpWORLDNATIVE & world->flags))
    {
        if (as.numVertices)
        {
            RwInt32 vertexSize = sizeof(RwV3d) * as.numVertices;

            /* Memory allocation draws from the pre-allocated pool */
            worldSector->vertices = (RwV3d *)BinaryWorldMalloc(binaryWorldMallocAddr, vertexSize);
            if (!worldSector->vertices)
            {
                /* Deallocation is performed in one block when we return */
                RWERROR_SET((E_RW_NOMEM, vertexSize));
                result = FALSE;
            }

            /* Read off vertices (reals) */
            if (result && (!RwStreamReadReal(stream, worldSector->vertices, vertexSize)))
            {
                result = FALSE;
            }

            if (result && (flags & rpWORLDNORMALS))
            {
                RwInt32 normalSize = sizeof(RwUInt32) * as.numVertices;

                worldSector->normals = (RwUInt32 *)BinaryWorldMalloc(binaryWorldMallocAddr, normalSize);
                if (!worldSector->normals)
                {
                    RWERROR_SET((E_RW_NOMEM, normalSize));
                    result = FALSE;
                }

                /* These are made of chars - don't need converting */
                if (result && (RwStreamRead(stream, worldSector->normals, normalSize) != (RwUInt32)normalSize))
                {
                    result = FALSE;
                }
            }

            /* Read off Prelighting info - conditional on whether the world has these */
            if (result && (flags & rpWORLDPRELIT))
            {
                RwInt32 preLitLumSize = sizeof(RwRGBA) * as.numVertices;

                worldSector->preLitLum = (RwRGBA *)BinaryWorldMalloc(binaryWorldMallocAddr, preLitLumSize);
                if (!worldSector->preLitLum)
                {
                    RWERROR_SET((E_RW_NOMEM, preLitLumSize));
                    result = FALSE;
                }

                if (result && (!RwStreamRead(stream, (void *)worldSector->preLitLum, preLitLumSize)))
                {
                    result = FALSE;
                }
            }

            /* Read off vertex texture cooords - conditional on whether the world has these */
            if (result && (world->numTexCoordSets > 0))
            {
                RwInt32 texCoordSize;

                texCoordSize = sizeof(RwTexCoords) * worldSector->numVertices;

                for (i = 0; i < world->numTexCoordSets; i++)
                {
                    worldSector->texCoords[i] = (RwTexCoords *)BinaryWorldMalloc(binaryWorldMallocAddr, texCoordSize);
                    if (result && (!worldSector->texCoords[i]))
                    {
                        RWERROR_SET((E_RW_NOMEM, texCoordSize));
                        result = FALSE;
                        break;
                    }

                    if (result && (!RwStreamReadReal(stream, worldSector->texCoords[i], texCoordSize)))
                    {
                        result = FALSE;
                        break;
                    }
                }
            }
        }

        if (result && as.numTriangles)
        {
            RwInt32 triangleSize = sizeof(RpTriangle) * as.numTriangles;

            /* Allocate all the memory first */
            worldSector->triangles = (RpTriangle *)BinaryWorldMalloc(binaryWorldMallocAddr, triangleSize);
            if (!worldSector->triangles)
            {
                RWERROR_SET((E_RW_NOMEM, triangleSize));
                result = FALSE;
            }

            /* Read off triangles */
            if (result && (!RwStreamReadInt16(stream, worldSector->triangles, (RwUInt32)triangleSize)))
            {
                result = FALSE;
            }

            if (result && (version < rwLIBRARYVERSION36002))
            {
                for (i = 0; i < as.numTriangles; ++i)
                {
                    rpPolygon poly;

                    poly = *((rpPolygon *)&worldSector->triangles[i]);

                    worldSector->triangles[i].vertIndex[0] = poly.vertIndex[0];
                    worldSector->triangles[i].vertIndex[1] = poly.vertIndex[1];
                    worldSector->triangles[i].vertIndex[2] = poly.vertIndex[2];
                    worldSector->triangles[i].matIndex = poly.matIndex;
                }
            }
        }
    }

    /* Call the constructor as late as we can */
    if (result)
    {
        _rwPluginRegistryInitObject(&sectorTKList, worldSector);

        lastSeenSectRightsPluginId = 0;
        lastSeenSectExtraData = 0;

        /* Read in the plugin info */
        if (result && (!_rwPluginRegistryReadDataChunks(&sectorTKList, stream, worldSector)))
        {
            /* Failed to read extension data */
            result = FALSE;
        }

        if (result && (lastSeenSectRightsPluginId))
        {
            /* Silently ignore errors if any */
            _rwPluginRegistryInvokeRights(&sectorTKList, lastSeenSectRightsPluginId, worldSector, lastSeenSectExtraData);
        }
    }

    /* Was there an error ? */
    if (result == FALSE)
    {
        if (world->privateFlags & rpWORLDSINGLEMALLOC)
        {
            _rpWorldSectorDeinstanceAll((RpSector *)worldSector);
        }
        else
        {
            _rpWorldSectorDestroyRecurse((RpSector *)worldSector);
        }

        return NULL;
    }

    return worldSector;
}
