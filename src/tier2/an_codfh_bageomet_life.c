/* bageomet lifecycle (MFL.exe 0x004f9060): RenderWare Graphics 3.7 (Criterion Software): RpGeometryDestroy.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bageomet.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build); the static GeometryAnnihilate and
 * RpGeometryLock(geometry, rpGEOMETRYLOCKALL) inlined as the exe has them (the temporary refCount bump of GeometryAnnihilate
 * cancels out: refCount is decremented once, after _rpMaterialListDeinitialize); RwFree as the engine's memoryFuncs.rwfree slot
 * (0x10c); the struct declared here (RW 3.7 RpGeometry) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
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
    RwUInt8 object[8];                  /* 0x00 RwObject */
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
RW_LAYOUT_ASSERT(rw_assert_geom_locked, RW_OFFSETOF(RpGeometry, lockedSinceLastInst) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_geom_ref, RW_OFFSETOF(RpGeometry, refCount) == 0xe);
RW_LAYOUT_ASSERT(rw_assert_geom_matlist, RW_OFFSETOF(RpGeometry, matList) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_geom_mesh, RW_OFFSETOF(RpGeometry, mesh) == 0x54);
RW_LAYOUT_ASSERT(rw_assert_geom_morph, RW_OFFSETOF(RpGeometry, morphTarget) == 0x5c);

#define rpGEOMETRYLOCKALL 0x0fff

typedef void (*RwFreeFn)(void *mem);

/*007d2128*/ extern void *RwEngineInstance;
/*0062ec70*/ extern RwUInt8 geometryTKList[];
#define RwFree (*(RwFreeFn *)((RwUInt8 *)RwEngineInstance + 0x10c))

/*004ea870*/ extern RwBool RwResourcesFreeResEntry(RwResEntry *entry);
/*00500e40*/ extern RwBool _rpMeshDestroy(RpMeshHeader *mesh);
/*004e9d40*/ extern void *_rwPluginRegistryDeInitObject(void *reg, void *object);
/*00503d90*/ extern RpMaterialList *_rpMaterialListDeinitialize(RpMaterialList *matList);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
RpGeometryDestroy(RpGeometry *geometry)
{
    /* Do we actually need to blow it away? */
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
        _rwPluginRegistryDeInitObject(geometryTKList, geometry);

        /* destroy morphTargets */
        if (geometry->morphTarget)
        {
            RwFree(geometry->morphTarget);
            geometry->morphTarget = (RpMorphTarget *)NULL;
        }

        _rpMaterialListDeinitialize(&geometry->matList);

        --geometry->refCount;

        RwFree(geometry);
    }
    else
    {
        --geometry->refCount;
    }

    return TRUE;
}
