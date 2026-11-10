/* RpGeometryStreamRead (MFL.exe 0x004f94f0): RenderWare Graphics 3.7 (Criterion Software), reads a geometry (struct chunk with the
 * prelight / texture coordinate / triangle / morph target data, material list chunk, extension chunks) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bageomet.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 / RwMemFloat32ToReal are no-ops and gone; RwStreamReadReal is the exe's
 * RwStreamReadInt32, the linker folded the two identical functions), no codfh 3.4 compatibility code (surface properties after the
 * header, _rpMaterialSetDefaultSurfaceProperties: dead after the version check, not in the exe), RWAPIFUNCTION / RWASSERT /
 * RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN),
 * RpGeometryDestroy written out at every call site the way the exe inlines it (GeometryAnnihilate inlined at some sites, called at
 * the others), RpMorphTargetSetBoundingSphere / RpGeometryGetMorphTarget written out, the structs declared here (codfh bageomet.h /
 * bamatlst.h / baerr.h) with compile-time layout asserts.  Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpGeometry {
    RwUInt32 format;
    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numMorphTargets;
} _rpGeometry;
typedef struct _rpMorphTarget {
    RwSphere boundingSphere;
    RwBool pointsPresent;
    RwBool normalsPresent;
} _rpMorphTarget;
typedef struct _rpTriangle {
    RwUInt32 vertex01;                  /* V0 index in top 16 bits, V1 index in bottom 16 bits */
    RwUInt32 vertex2Mat;                /* V2 index in top 16 bits, material index in bottom 16 bits */
} _rpTriangle;
struct RpMorphTarget {                  /* RpTriangle / the RpGeometry and RpMorphTarget typedefs: mfl_tier2.h */
    RpGeometry *parentGeom;
    RwSphere boundingSphere;
    RwV3d *verts;
    RwV3d *normals;
};
typedef struct RpMaterialList {
    void **materials;                   /* RpMaterial ** */
    RwInt32 numMaterials;
    RwInt32 space;
} RpMaterialList;
struct RpGeometry {
    RwUInt8 type;                       /* RwObject */
    RwUInt8 subType;
    RwUInt8 objFlags;
    RwUInt8 privateFlags;
    void *parent;
    RwUInt32 flags;
    RwUInt16 lockedSinceLastInst;
    short refCount;
    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numMorphTargets;
    RwInt32 numTexCoordSets;
    RpMaterialList matList;
    RpTriangle *triangles;
    RwRGBA *preLitLum;
    RwTexCoords *texCoords[8];          /* rwMAXTEXTURECOORDS */
    void *mesh;                         /* RpMeshHeader * */
    RwResEntry *repEntry;
    RpMorphTarget *morphTarget;
};
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
RW_LAYOUT_ASSERT(rw_assert_rpgeom_size, sizeof(_rpGeometry) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_rpmorph_size, sizeof(_rpMorphTarget) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_tri_size, sizeof(RpTriangle) == sizeof(_rpTriangle));
RW_LAYOUT_ASSERT(rw_assert_morph_size, sizeof(RpMorphTarget) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_morph_verts, RW_OFFSETOF(RpMorphTarget, verts) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_geom_flags, RW_OFFSETOF(RpGeometry, flags) == 8);
RW_LAYOUT_ASSERT(rw_assert_geom_ref, RW_OFFSETOF(RpGeometry, refCount) == 0xe);
RW_LAYOUT_ASSERT(rw_assert_geom_ntri, RW_OFFSETOF(RpGeometry, numTriangles) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_geom_ntc, RW_OFFSETOF(RpGeometry, numTexCoordSets) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_geom_matlist, RW_OFFSETOF(RpGeometry, matList) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_geom_tris, RW_OFFSETOF(RpGeometry, triangles) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_geom_texc, RW_OFFSETOF(RpGeometry, texCoords) == 0x34);
RW_LAYOUT_ASSERT(rw_assert_geom_rep, RW_OFFSETOF(RpGeometry, repEntry) == 0x58);
RW_LAYOUT_ASSERT(rw_assert_geom_morph, RW_OFFSETOF(RpGeometry, morphTarget) == 0x5c);

#define rwID_STRUCT 0x1
#define rwID_MATLIST 0x8
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rpGEOMETRYPRELIT 0x00000008
#define rpGEOMETRYNATIVE 0x01000000
#define rpGEOMETRYLOCKALL 0x0fff
#define E_RW_BADVERSION ((RwInt32)0x80000004)

/*007d2128*/ extern void *RwEngineInstance;
/*0062ec70*/ extern RwPluginRegistry geometryTKList;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*004f8e30*/ extern RpGeometry *RpGeometryCreate(RwInt32 numVerts, RwInt32 numTriangles, RwUInt32 format);
/*004f8910*/ extern RwInt32 RpGeometryAddMorphTargets(RpGeometry *geometry, RwInt32 mtcount);
/*004f8ba0*/ extern RpGeometry *RpGeometryLock(RpGeometry *geometry, RwInt32 lockMode);
/*004f8bd0*/ extern RpGeometry *RpGeometryUnlock(RpGeometry *geometry);
/*004f9a10*/ extern RwBool GeometryAnnihilate(RpGeometry *geometry);
/*004ea870*/ extern RwBool RwResourcesFreeResEntry(RwResEntry *entry);
/*004e9d40*/ extern RwBool _rwPluginRegistryDeInitObject(const RwPluginRegistry *reg, void *object);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*00503d90*/ extern RpMaterialList *_rpMaterialListDeinitialize(RpMaterialList *matList);
/*005040c0*/ extern RpMaterialList *_rpMaterialListStreamRead(RwStream *stream, RpMaterialList *matList);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RwStreamReadReal(stream, reals, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(reals), (numBytes))
#define RWERROR_SET(code) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror(code); RwErrorSet(&error); }

/* RpGeometryDestroy with GeometryAnnihilate written out (the refCount bump of GeometryAnnihilate cancels the decrement before it) */
#define RpGeometryDestroyInline(geometry)                                       \
    do {                                                                        \
        if (((geometry)->refCount - 1) <= 0)                                    \
        {                                                                       \
            if ((geometry)->repEntry)                                           \
            {                                                                   \
                RwResourcesFreeResEntry((geometry)->repEntry);                  \
            }                                                                   \
            --(geometry)->refCount;                                             \
            ++(geometry)->refCount;      /* GeometryAnnihilate */               \
            RpGeometryLock((geometry), rpGEOMETRYLOCKALL);                      \
            _rwPluginRegistryDeInitObject(&geometryTKList, (geometry));         \
            if ((geometry)->morphTarget)                                        \
            {                                                                   \
                RwFree((geometry)->morphTarget);                                \
                (geometry)->morphTarget = NULL;                                 \
            }                                                                   \
            _rpMaterialListDeinitialize(&(geometry)->matList);                  \
            --(geometry)->refCount;                                             \
            RwFree(geometry);                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            --(geometry)->refCount;                                             \
        }                                                                       \
    } while (0)

/* RpGeometryDestroy calling GeometryAnnihilate */
#define RpGeometryDestroyCall(geometry)                                         \
    do {                                                                        \
        if (((geometry)->refCount - 1) <= 0)                                    \
        {                                                                       \
            if ((geometry)->repEntry)                                           \
            {                                                                   \
                RwResourcesFreeResEntry((geometry)->repEntry);                  \
            }                                                                   \
            --(geometry)->refCount;                                             \
            GeometryAnnihilate(geometry);                                       \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            --(geometry)->refCount;                                             \
        }                                                                       \
    } while (0)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpGeometry *RpGeometryStreamRead(RwStream *stream)
{
    RpGeometry *geometry;
    _rpGeometry geom;
    RwUInt32 version;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, NULL, &version))
    {
        return NULL;
    }

    if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
    {
        RWERROR_SET(E_RW_BADVERSION);
        return NULL;
    }

    if (RwStreamRead(stream, &geom, sizeof(geom)) != sizeof(geom))
    {
        return NULL;
    }

    /* RpGeometryCreate will take care of the format */
    geometry = RpGeometryCreate(geom.numVertices, geom.numTriangles, geom.format);
    if (!geometry)
    {
        return NULL;
    }

    /* Add sufficient morph targets for the geometry to fill in */
    if (geom.numMorphTargets > 1)
    {
        if (RpGeometryAddMorphTargets(geometry, geom.numMorphTargets - 1) < 0)
        {
            RpGeometryDestroyInline(geometry);
            return NULL;
        }
    }

    if (!(rpGEOMETRYNATIVE & geometry->flags))
    {
        if (geometry->numVertices)
        {
            /* Read prelighting information */
            if (geom.format & rpGEOMETRYPRELIT)
            {
                RwUInt32 sizeLum = geometry->numVertices * sizeof(RwRGBA);

                /* No conversion needed - it's made of chars */
                if (RwStreamRead(stream, geometry->preLitLum, sizeLum) != sizeLum)
                {
                    /* Failed, so tidy up */
                    RpGeometryDestroyInline(geometry);
                    return NULL;
                }
            }

            /* Read texture coordinate information */
            if (geometry->numTexCoordSets > 0)
            {
                RwInt32 i;
                const RwUInt32 sizeTC = geometry->numVertices * sizeof(RwTexCoords);

                /* Read vertex texture coordinates - reals, remember */
                for (i = 0; i < geometry->numTexCoordSets; ++i)
                {
                    if (!RwStreamReadReal(stream, geometry->texCoords[i], sizeTC))
                    {
                        /* Failed, so tidy up */
                        RpGeometryDestroyInline(geometry);
                        return NULL;
                    }
                }
            }

            /* Read in the triangles */
            if (geometry->numTriangles)
            {
                RpTriangle *destTri;
                RwInt32 numTris;
                RwUInt32 size;

                /* Load the triangle information */
                numTris = geometry->numTriangles;
                destTri = geometry->triangles;

                size = numTris * sizeof(_rpTriangle);
                if (RwStreamRead(stream, (void *)destTri, size) != size)
                {
                    RpGeometryDestroyInline(geometry);
                    return NULL;
                }

                /* These are unpacked "in-place" into RwUInt16 fields */
                while (numTris--)
                {
                    RwUInt16 hi, lo;
                    _rpTriangle *srceTri;

                    srceTri = (_rpTriangle *)destTri;

                    hi = (RwUInt16)(srceTri->vertex01 >> 16) & 0xFFFF;
                    lo = (RwUInt16)(srceTri->vertex01) & 0xFFFF;
                    destTri->vertIndex[0] = hi;
                    destTri->vertIndex[1] = lo;

                    hi = (RwUInt16)(srceTri->vertex2Mat >> 16) & 0xFFFF;
                    lo = (RwUInt16)(srceTri->vertex2Mat) & 0xFFFF;
                    destTri->vertIndex[2] = hi;
                    destTri->matIndex = lo;

                    ++destTri;
                }
            }
        }
    }

    /* Morph targets */
    {
        RwInt32 i;

        for (i = 0; i < geometry->numMorphTargets; ++i)
        {
            _rpMorphTarget kf;
            RpMorphTarget *morphTarget = &geometry->morphTarget[i];      /* RpGeometryGetMorphTarget */

            if (RwStreamRead(stream, &kf, sizeof(kf)) != sizeof(kf))
            {
                RpGeometryDestroyInline(geometry);
                return NULL;
            }

            morphTarget->boundingSphere = kf.boundingSphere;      /* RpMorphTargetSetBoundingSphere */

            /* Optimised for reading morph target vertices and normals at the same time */
            if (kf.pointsPresent && kf.normalsPresent)
            {
                if (!RwStreamReadReal(stream, morphTarget->verts, sizeof(RwV3d) * 2 * geometry->numVertices))
                {
                    RpGeometryDestroyInline(geometry);
                    return NULL;
                }
            }
            else
            {
                if (kf.pointsPresent)
                {
                    if (!RwStreamReadReal(stream, morphTarget->verts, sizeof(RwV3d) * geometry->numVertices))
                    {
                        RpGeometryDestroyInline(geometry);
                        return NULL;
                    }
                }
                if (kf.normalsPresent)
                {
                    if (!RwStreamReadReal(stream, morphTarget->normals, sizeof(RwV3d) * geometry->numVertices))
                    {
                        RpGeometryDestroyCall(geometry);
                        return NULL;
                    }
                }
            }
        }
    }

    if (!RwStreamFindChunk(stream, rwID_MATLIST, NULL, &version))
    {
        return NULL;
    }

    if ((version < rwLIBRARYBASEVERSION) || (version > rwLIBRARYCURRENTVERSION))
    {
        RpGeometryDestroyCall(geometry);
        RWERROR_SET(E_RW_BADVERSION);
        return NULL;
    }

    if (!_rpMaterialListStreamRead(stream, &geometry->matList))
    {
        RpGeometryDestroyCall(geometry);
        return NULL;
    }

    /* Read the plugin data whilst the geometry is locked */
    if (!_rwPluginRegistryReadDataChunks(&geometryTKList, stream, geometry))
    {
        RpGeometryDestroyCall(geometry);
        return NULL;
    }

    /* Release it so that it can be tailored */
    if (!RpGeometryUnlock(geometry))
    {
        RpGeometryDestroyInline(geometry);
        return NULL;
    }

    return geometry;
}
