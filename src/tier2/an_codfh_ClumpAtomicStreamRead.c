/* ClumpAtomicStreamRead (MFL.exe 0x004f71a0): RenderWare Graphics 3.7 (Criterion Software), reads one atomic of a clump (struct chunk
 * with frame / geometry index and flags, an inline geometry chunk when the clump has no geometry list, extension chunks).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 is a no-op and gone), RWFUNCTION / RWASSERT / RWRETURN expanded, RWERROR
 * expanded as the release build has it (_rwerror + RwErrorSet, plugin ID rwID_WORLDPLUGIN), memset as a zero initialiser,
 * RpAtomicSetFlags / RpAtomicSetFrame / RpAtomicSetGeometry / RpAtomicDestroy written out (the exe inlines them here; no codfh
 * geometry lock), lastSeenRightsPluginId / lastSeenExtraData are the exe's globals, external linkage (the exe's symbol), the structs
 * declared here (codfh baclump.h / bageomet.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct _rpAtomicBinary {
    RwInt32 frameIndex;
    RwInt32 geomIndex;
    RwInt32 flags;
    RwInt32 unused;
} _rpAtomicBinary;
typedef struct rwFrameList {
    RwFrame **frames;
    RwInt32 numFrames;
} rwFrameList;
typedef struct rpGeometryList {
    RpGeometry **geometries;
    RwInt32 numGeoms;
} rpGeometryList;
struct RpMorphTarget {                  /* the RpGeometry / RpMorphTarget typedefs: mfl_tier2.h */
    RpGeometry *parentGeom;
    RwSphere boundingSphere;
    RwV3d *verts;
    RwV3d *normals;
};
struct RpGeometry {                     /* only the morph target array is used here */
    RwUInt8 head[0x5c];
    RpMorphTarget *morphTarget;
};
typedef struct RpAtomic {
    RwUInt8 type;                       /* RwObjectHasFrame.object */
    RwUInt8 subType;
    RwUInt8 flags;
    RwUInt8 privateFlags;
    RwFrame *parent;
    void *lFrameNext;                   /* RwObjectHasFrame.lFrame */
    void *lFramePrev;
    void *sync;                         /* RwObjectHasFrame.sync */
    RwResEntry *repEntry;
    RpGeometry *geometry;
    RwSphere boundingSphere;
} RpAtomic;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpClumpGlobals {
    void *atomicFreeList;               /* RwFreeList * */
} rpClumpGlobals;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
RW_LAYOUT_ASSERT(rw_assert_atombin_size, sizeof(_rpAtomicBinary) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_geom_morph, RW_OFFSETOF(struct RpGeometry, morphTarget) == 0x5c);
RW_LAYOUT_ASSERT(rw_assert_morph_sphere, RW_OFFSETOF(RpMorphTarget, boundingSphere) == 4);
RW_LAYOUT_ASSERT(rw_assert_atom_rep, RW_OFFSETOF(RpAtomic, repEntry) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_atom_geom, RW_OFFSETOF(RpAtomic, geometry) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_atom_sphere, RW_OFFSETOF(RpAtomic, boundingSphere) == 0x1c);

#define rwID_STRUCT 0x1
#define rwID_GEOMETRY 0xf
#define rwID_WORLDPLUGIN 0x2
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rpATOMICPRIVATEWORLDBOUNDDIRTY 0x01
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define E_RW_READ ((RwInt32)0x8000001a)

/* RwFreeListFree: RwGlobals.freeListFree 0x11c (rw37.h) */
typedef void *(*RwFreeListFreeFunc)(void *, void *);
#define RwFreeListFree RW_GLOBAL(0x11c, RwFreeListFreeFunc)

/*007d2128*/ extern void *RwEngineInstance;
/*007d5330*/ extern RwModuleInfo clumpModule;
/*0062ec04*/ extern RwPluginRegistry atomicTKList;
/*007d532c*/ extern RwUInt32 g_clumpatomicstreamread_7d532c;      /* lastSeenRightsPluginId */
/*007d5304*/ extern RwUInt32 g_clumpatomicstreamread_7d5304;      /* lastSeenExtraData */
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004f68f0*/ extern RpAtomic *RpAtomicCreate(void);
/*004d1860*/ extern void *_rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
/*004d18b0*/ extern void *_rwObjectHasFrameReleaseFrame(void *object);
/*004f9050*/ extern RpGeometry *RpGeometryAddRef(RpGeometry *geometry);
/*004f9060*/ extern RwBool RpGeometryDestroy(RpGeometry *geometry);
/*004f94f0*/ extern RpGeometry *RpGeometryStreamRead(RwStream *stream);
/*004f4560*/ extern void *RpAtomicGetWorld(const RpAtomic *atomic);
/*004d0f70*/ extern RwFrame *RwFrameUpdateObjects(RwFrame *frame);
/*004ea870*/ extern RwBool RwResourcesFreeResEntry(RwResEntry *entry);
/*004e9d40*/ extern RwBool _rwPluginRegistryDeInitObject(const RwPluginRegistry *reg, void *object);
/*004f1ea0*/ extern const void *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object);
/*004f1fd0*/ extern RwBool _rwPluginRegistryInvokeRights(const RwPluginRegistry *reg, RwUInt32 id, void *obj, RwUInt32 extraData);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define lastSeenRightsPluginId g_clumpatomicstreamread_7d532c
#define lastSeenExtraData g_clumpatomicstreamread_7d5304
#define RWCLUMPGLOBAL(var) (((rpClumpGlobals *)((RwUInt8 *)RwEngineInstance + clumpModule.globalsOffset))->var)
#define RWERROR_SET(code) { RwError error; error.pluginID = rwID_WORLDPLUGIN; error.errorCode = _rwerror(code); RwErrorSet(&error); }

/* RpAtomicSetGeometry(atomic, geometry, 0), inlined by the exe */
#define RpAtomicSetGeometryInline(atomic, geom)                                 \
    do {                                                                        \
        RpGeometry *geometry_ = (geom);                                         \
        if (geometry_ != (atomic)->geometry)                                    \
        {                                                                       \
            RwFrame *frame_;                                                    \
            if (geometry_)                                                      \
            {                                                                   \
                RpGeometryAddRef(geometry_);      /* Add ref the new geometry */ \
            }                                                                   \
            if ((atomic)->geometry)                                             \
            {                                                                   \
                RpGeometryDestroy((atomic)->geometry);      /* the old one */   \
            }                                                                   \
            (atomic)->geometry = geometry_;                                     \
            if (geometry_)                                                      \
            {                                                                   \
                (atomic)->boundingSphere = geometry_->morphTarget[0].boundingSphere; \
            }                                                                   \
            frame_ = (atomic)->parent;                                          \
            if (frame_ && RpAtomicGetWorld(atomic))                             \
            {                                                                   \
                RwFrameUpdateObjects(frame_);      /* ties get updated */       \
            }                                                                   \
        }                                                                       \
    } while (0)

/* RpAtomicDestroy, inlined by the exe */
#define RpAtomicDestroyInline(atomic)                                           \
    do {                                                                        \
        _rwPluginRegistryDeInitObject(&atomicTKList, (atomic));                 \
        if ((atomic)->repEntry)                                                 \
        {                                                                       \
            RwResourcesFreeResEntry((atomic)->repEntry);                        \
        }                                                                       \
        RpAtomicSetGeometryInline((atomic), NULL);                              \
        _rwObjectHasFrameReleaseFrame(atomic);                                  \
        RwFreeListFree(RWCLUMPGLOBAL(atomicFreeList), (atomic));                \
    } while (0)

/* TIER2: validated 2026-11-10 authority=diffemu */
RpAtomic *ClumpAtomicStreamRead(RwStream *stream, rwFrameList *fl, rpGeometryList *gl)
{
    RwBool status;
    RwUInt32 size;
    RwUInt32 version;

    status = RwStreamFindChunk(stream, rwID_STRUCT, &size, &version);
    if (!status)
    {
        RWERROR_SET(E_RW_READ);
        return NULL;
    }

    if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
    {
        RpAtomic *atom;
        _rpAtomicBinary a = { 0 };
        RpGeometry *geom;

        /* Read the atomic */
        status = (size == RwStreamRead(stream, &a, size));
        if (!status)
        {
            RWERROR_SET(E_RW_READ);
            return NULL;
        }

        atom = RpAtomicCreate();
        if (!atom)
        {
            return NULL;
        }

        /* Set the atomic types */
        atom->flags = (RwUInt8)a.flags;      /* RpAtomicSetFlags */
        if (fl->numFrames)
        {
            /* RpAtomicSetFrame */
            _rwObjectHasFrameSetFrame(atom, fl->frames[a.frameIndex]);
            atom->privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;
        }

        /* get the geometry */
        if (gl->numGeoms)
        {
            RpAtomicSetGeometryInline(atom, gl->geometries[a.geomIndex]);
        }
        else
        {
            status = RwStreamFindChunk(stream, rwID_GEOMETRY, NULL, &version);
            if (!status)
            {
                RpAtomicDestroyInline(atom);
                RWERROR_SET(E_RW_READ);
                return NULL;
            }

            if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
            {
                geom = RpGeometryStreamRead(stream);
                status = (NULL != geom);

                if (!status)
                {
                    RpAtomicDestroyInline(atom);
                    RWERROR_SET(E_RW_READ);
                    return NULL;
                }
            }
            else
            {
                RpAtomicDestroyInline(atom);
                RWERROR_SET(E_RW_BADVERSION);
                return NULL;
            }

            RpAtomicSetGeometryInline(atom, geom);

            /* Bring the geometry reference count back down, so that
             * when the atomic is destroyed, so is the geometry.
             */
            RpGeometryDestroy(geom);
        }

        /* A bit ugly. If a rights owner is spotted, this will be set */
        lastSeenRightsPluginId = 0;
        lastSeenExtraData = 0;

        /* Atomic extension data */
        status = (NULL != _rwPluginRegistryReadDataChunks(&atomicTKList, stream, atom));
        if (!status)
        {
            RWERROR_SET(E_RW_READ);
            return NULL;
        }

        if (lastSeenRightsPluginId)
        {
            /* Silently ignore errors if any */
            _rwPluginRegistryInvokeRights(&atomicTKList, lastSeenRightsPluginId, atom, lastSeenExtraData);
        }

        return atom;
    }

    RWERROR_SET(E_RW_BADVERSION);
    return NULL;
}
