/* bageomet create (MFL.exe 0x004f8e30): RenderWare Graphics 3.7 (Criterion Software): RpGeometryCreate.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bageomet.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), RwDebugSendMessage (debug build only)
 * gone, RWERROR((E_RP_WORLD_TOOMANYVERTICES)) expanded as the release build has it (_rwerror(6) + RwErrorSet, plugin ID
 * rwID_WORLDPLUGIN); GeometryFormatGetFlagsMacro / GeometryFormatGetNumTexCoordSetsMacro / rwObjectInitialize / RpGeometrySetFlags
 * written out, RpGeometryAddMorphTarget as the 3.7 RpGeometryAddMorphTargets(geometry, 1); memset of the texture coordinate pointers
 * as a loop; RwMalloc / RwFree as the engine's memoryFuncs slots (mfl_tier2.h); the struct declared here (RW 3.7 RpGeometry) with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RpMaterialList {
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;

typedef struct RpMeshHeader RpMeshHeader;

struct RpGeometry {      /* typedef name RpGeometry comes from mfl_tier2.h */
    RwUInt8 type;                       /* 0x00 RwObject */
    RwUInt8 subType;
    RwUInt8 objFlags;
    RwUInt8 privateFlags;
    void *parent;
    RwUInt32 flags;                     /* 0x08 */
    RwUInt16 lockedSinceLastInst;       /* 0x0c */
    short refCount;                     /* 0x0e */
    RwInt32 numTriangles;               /* 0x10 */
    RwInt32 numVertices;                /* 0x14 */
    RwInt32 numMorphTargets;            /* 0x18 */
    RwInt32 numTexCoordSets;            /* 0x1c */
    RpMaterialList matList;             /* 0x20 */
    RpTriangle *triangles;              /* 0x2c */
    RwRGBA *preLitLum;                  /* 0x30 */
    RwTexCoords *texCoords[8];          /* 0x34 */
    RpMeshHeader *mesh;                 /* 0x54 */
    RwResEntry *repEntry;               /* 0x58 */
    RpMorphTarget *morphTarget;         /* 0x5c */
};
struct RpMorphTarget {      /* typedef name RpMorphTarget comes from mfl_tier2.h */
    RpGeometry *parentGeom;             /* 0x00 */
    RwSphere boundingSphere;            /* 0x04 */
    RwV3d *verts;                       /* 0x14 */
    RwV3d *normals;                     /* 0x18 */
};
typedef struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
} RwPluginRegistry;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_geom_flags, RW_OFFSETOF(RpGeometry, flags) == 8);
RW_LAYOUT_ASSERT(rw_assert_geom_ref, RW_OFFSETOF(RpGeometry, refCount) == 0xe);
RW_LAYOUT_ASSERT(rw_assert_geom_ntex, RW_OFFSETOF(RpGeometry, numTexCoordSets) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_geom_matlist, RW_OFFSETOF(RpGeometry, matList) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_geom_texc, RW_OFFSETOF(RpGeometry, texCoords) == 0x34);
RW_LAYOUT_ASSERT(rw_assert_geom_morph, RW_OFFSETOF(RpGeometry, morphTarget) == 0x5c);
RW_LAYOUT_ASSERT(rw_assert_geom_repentry, RW_OFFSETOF(RpGeometry, repEntry) == 0x58);
RW_LAYOUT_ASSERT(rw_assert_morph_sphere, RW_OFFSETOF(RpMorphTarget, boundingSphere) == 4);

#define rpGEOMETRY 8
#define rpGEOMETRYTEXTURED 0x00000004
#define rpGEOMETRYPRELIT 0x00000008
#define rpGEOMETRYTEXTURED2 0x00000080
#define rpGEOMETRYNATIVE 0x01000000
#define rpGEOMETRYNATIVEFLAGSMASK 0x0F000000
#define rpGEOMETRYFLAGSMASK 0x000000FF
#define rpGEOMETRYLOCKALL 0x0fff
#define rwID_GEOMETRY 0x0f
#define rwID_WORLDPLUGIN 0x2
#define rwMEMHINTDUR_EVENT 0x00030000
#define E_RP_WORLD_TOOMANYVERTICES 6      /* world plugin error code (the exe pushes 6) */

/*007d2128*/ extern void *RwEngineInstance;
/*0062ec70*/ extern RwPluginRegistry geometryTKList;

/*00503df0*/ extern RpMaterialList *_rpMaterialListInitialize(RpMaterialList *matList);
/*00503d90*/ extern RpMaterialList *_rpMaterialListDeinitialize(RpMaterialList *matList);
/*004f8910*/ extern RwInt32 RpGeometryAddMorphTargets(RpGeometry *geometry, RwInt32 mtcount);
/*004f8bd0*/ extern RpGeometry *RpGeometryUnlock(RpGeometry *geometry);
/*004ea870*/ extern RwBool RwResourcesFreeResEntry(RwResEntry *entry);
/*00500e40*/ extern RwBool _rpMeshDestroy(RpMeshHeader *mesh);
/*004e9d40*/ extern RwBool _rwPluginRegistryDeInitObject(const RwPluginRegistry *reg, void *object);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(const RwPluginRegistry *reg, void *object);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpGeometry *
RpGeometryCreate(RwInt32 numVerts, RwInt32 numTriangles, RwUInt32 format)
{
    RpGeometry *geometry;
    RwUInt8 *goffset;
    RwUInt32 gsize;
    RwUInt32 numTexCoordSets;
    RwInt32 flags;
    RwInt32 i;

    if ((numVerts < 0) || (numVerts >= 65536) || (numTriangles < 0))
    {
        if (numVerts >= 65536)
        {
            RwError error;

            error.pluginID = rwID_WORLDPLUGIN;
            error.errorCode = _rwerror(E_RP_WORLD_TOOMANYVERTICES);
            RwErrorSet(&error);
        }
        return (RpGeometry *)NULL;
    }

    flags = format & rpGEOMETRYFLAGSMASK;
    gsize = geometryTKList.sizeOfStruct;

    /* How many texture coordinate sets? */
    numTexCoordSets = (format & 0xff0000) ? ((format & 0xff0000) >> 16) :
                      ((flags & rpGEOMETRYTEXTURED2) ? 2 : ((flags & rpGEOMETRYTEXTURED) ? 1 : 0));

    /* Make sure backward compatibility flags are set correctly */
    flags = (flags & ~(rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2))
             | ((numTexCoordSets == 1) ? rpGEOMETRYTEXTURED :
               ((numTexCoordSets > 1) ? rpGEOMETRYTEXTURED2 : 0));

    if (!(rpGEOMETRYNATIVE & format))
    {
        if (flags & (RwInt32)rpGEOMETRYPRELIT)
        {
            gsize += sizeof(RwRGBA) * numVerts;
        }

        if (numTexCoordSets > 0)
        {
            /* Include space for tex coords */
            gsize += sizeof(RwTexCoords) * numVerts * numTexCoordSets;
        }

        gsize += sizeof(RpTriangle) * numTriangles;
    }

    geometry = (RpGeometry *)RwMalloc(gsize, rwID_GEOMETRY | rwMEMHINTDUR_EVENT);
    if (!geometry)
    {
        return (RpGeometry *)NULL;
    }

    /* Set up the material list */
    if (!_rpMaterialListInitialize(&geometry->matList))
    {
        /* Failed to initialise the geometry list */
        return (RpGeometry *)NULL;
    }

    /* Allocate initial array of morph targets (0) */
    geometry->morphTarget = NULL;

    /* Set up key frames */
    geometry->numMorphTargets = 0;

    /* Set up type: rwObjectInitialize(geometry, rpGEOMETRY, 0) */
    geometry->type = rpGEOMETRY;
    geometry->subType = 0;
    geometry->objFlags = 0;
    geometry->privateFlags = 0;
    geometry->parent = NULL;

    /* Set the instancing information */
    geometry->repEntry = NULL;

    /* Nothing locked since last time */
    geometry->lockedSinceLastInst = 0;

    /* Initialise ref count */
    geometry->refCount = 1;

    /* Set up the device information -> its locked! */
    geometry->mesh = NULL;

    /* Set up texture coords */
    geometry->numTexCoordSets = numTexCoordSets;
    for (i = 0; i < 8; i++)
    {
        geometry->texCoords[i] = NULL;
    }

    geometry->preLitLum = NULL;

    /* Set up the triangles */
    geometry->triangles = NULL;
    geometry->numTriangles = numTriangles;

    /* Set the geometries flags */
    geometry->flags = flags | (rpGEOMETRYNATIVEFLAGSMASK & format);
    geometry->numVertices = numVerts;

    if (!(rpGEOMETRYNATIVE & format))
    {
        /* step past structure to allocate arrays */
        goffset = (RwUInt8 *)geometry + geometryTKList.sizeOfStruct;

        /* Create prelight values if necessary */
        if ((flags & (RwInt32)rpGEOMETRYPRELIT) && numVerts)
        {
            geometry->preLitLum = (RwRGBA *)goffset;
            goffset += sizeof(RwRGBA *) * numVerts;
        }

        /* Create texture coordinates if necessary - in the right place */
        if (numTexCoordSets > 0 && numVerts)
        {
            RwUInt32 j;

            for (j = 0; j < numTexCoordSets; ++j)
            {
                geometry->texCoords[j] = (RwTexCoords *)goffset;
                goffset += sizeof(RwTexCoords) * numVerts;
            }
        }

        /* Set up the triangles */
        if (numTriangles)
        {
            geometry->triangles = (RpTriangle *)goffset;

            /* Setup all of the materials */
            for (i = 0; i < numTriangles; ++i)
            {
                geometry->triangles[i].matIndex = 0xFFFF;
            }
        }
    }

    /* Allocate one key frame, because geometry is useless without it */
    if (RpGeometryAddMorphTargets(geometry, 1) < 0)
    {
        _rpMaterialListDeinitialize(&geometry->matList);
        RwFree(geometry);
        return (RpGeometry *)NULL;
    }

    /* Initialize the plugin memory */
    _rwPluginRegistryInitObject(&geometryTKList, geometry);

    /* All done */
    return geometry;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpGeometry *
RpGeometryCreateSpace(RwReal radius)
{
    RpGeometry *geometry;

    geometry = RpGeometryCreate(0, 0, 0);
    if (geometry)
    {
        RpMorphTarget *morphTarget = &geometry->morphTarget[0];

        morphTarget->boundingSphere.center.x = (RwReal)(0.0);
        morphTarget->boundingSphere.center.y = (RwReal)(0.0);
        morphTarget->boundingSphere.center.z = (RwReal)(0.0);
        morphTarget->boundingSphere.radius = radius;
    }

    if (!RpGeometryUnlock(geometry))
    {
        /* RpGeometryDestroy(geometry): do we actually need to blow it away? */
        if ((geometry->refCount - 1) <= 0)
        {
            /* Blow away its instance copy if it has one */
            if (geometry->repEntry)
            {
                RwResourcesFreeResEntry(geometry->repEntry);
            }

            /* GeometryAnnihilate: RpGeometryLock(geometry, rpGEOMETRYLOCKALL) */
            geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;
            if (geometry->mesh)
            {
                _rpMeshDestroy(geometry->mesh);
                geometry->mesh = (RpMeshHeader *)NULL;
            }

            /* DeInitialize the plugin memory */
            _rwPluginRegistryDeInitObject(&geometryTKList, geometry);

            /* destroy morphTargets */
            if (geometry->morphTarget)
            {
                RwFree(geometry->morphTarget);
                geometry->morphTarget = NULL;
            }

            _rpMaterialListDeinitialize(&geometry->matList);

            --geometry->refCount;

            RwFree(geometry);
        }
        else
        {
            --geometry->refCount;
        }
        return (RpGeometry *)NULL;
    }

    return geometry;
}
