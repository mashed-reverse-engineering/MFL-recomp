/* rpmorph plugin callbacks (MFL.exe 0x005550a0 - 0x00555577): RenderWare Graphics 3.7 (Criterion Software), morph target plugin:
 * RpMorphGeometrySetInterpolator, RpMorphPluginAttach, the atomic copy (AtomicAnimCopy), the geometry destructor (GeomAnimDtor)
 * and the geometry copy / stream write / stream size (GeomAnimCopy / GeomAnimWriteStream / GeomAnimSizeStream: rw_trap rows,
 * ported so the attach does not reference int3 code; RwMemRealToFloat32 / RwMemLittleEndian32 are no-ops on x86).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/morph/rpmorph.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), the RpInterpolatorSet* macros of rpworld.h
 * expanded in AtomicAnimCopy (flags |= dirty instance | dirty sphere, RwFrameUpdateObjects of the atomic's frame unless
 * rpINTERPOLATORNOFRAMEDIRTY), RwFree through the engine function table (+0x10c), the atomic / interpolator / extension
 * fields declared here by the exe's offsets with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define TRUE 1
#define FALSE 0
#define rwID_MORPHPLUGIN 0x105
#define rpINTERPOLATORDIRTYINSTANCE 0x01
#define rpINTERPOLATORDIRTYSPHERE 0x02
#define rpINTERPOLATORNOFRAMEDIRTY 0x04

typedef short RwInt16;
typedef struct RpMorphInterpolator {
    RwInt32 flags;
    RwInt16 startMorphTarget;
    RwInt16 endMorphTarget;
    RwReal time;
    RwReal recipTime;
    struct RpMorphInterpolator *next;
} RpMorphInterpolator;
typedef struct RpMorphAtomicAnim {
    RpMorphInterpolator *interp;
    RwReal position;
    void *lastGeom;                         /* RpGeometry * */
} RpMorphAtomicAnim;
typedef struct RpMorphGeometryAnim {
    RwInt32 numInterpolators;
    RwInt32 currentInterpolator;
    RpMorphInterpolator *allInterps;
} RpMorphGeometryAnim;
typedef struct RpInterpolator {             /* rpworld.h */
    RwInt32 flags;
    RwInt16 startMorphTarget;
    RwInt16 endMorphTarget;
    RwReal time;
    RwReal recipTime;
    RwReal position;
} RpInterpolator;
typedef struct RpAtomicHead {               /* RpAtomic: object.parent (the frame), geometry and the interpolator */
    RwUInt8 type, subType, flags, privateFlags;
    void *frame;                            /* object.parent */
    RwUInt8 pad[0x18 - 8];
    void *geometry;
    RwUInt8 pad2[0x4c - 0x1c];
    RpInterpolator interpolator;
} RpAtomicHead;
RW_LAYOUT_ASSERT(rw_assert_morph_interp, sizeof(RpMorphInterpolator) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_morph_geom, RW_OFFSETOF(RpMorphGeometryAnim, allInterps) == 8);
RW_LAYOUT_ASSERT(rw_assert_morph_atomic, RW_OFFSETOF(RpAtomicHead, geometry) == 0x18 && RW_OFFSETOF(RpAtomicHead, interpolator) == 0x4c);
RW_LAYOUT_ASSERT(rw_assert_morph_rpinterp, RW_OFFSETOF(RpInterpolator, position) == 0x10);

/*007d2128*/ extern void *RwEngineInstance;
/*007da7ac*/ extern RwInt32 atomicAnimOffset;
/*007da7b0*/ extern RwInt32 geomAnimOffset;
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *initCB, void *termCB);
/*004f7e80*/ extern RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f9110*/ extern RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f9140*/ extern RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004d0f70*/ extern void *RwFrameUpdateObjects(void *frame);
/*005553f0*/ extern void *AnimOpen(void *instance, RwInt32 offset, RwInt32 size);
/*00555400*/ extern void *AnimClose(void *instance, RwInt32 offset, RwInt32 size);
/*00555410*/ extern void *AtomicAnimCtorDtor(void *object, RwInt32 offset, RwInt32 size);
/*00555820*/ extern void *GeomAnimCtor(void *object, RwInt32 offset, RwInt32 size);
/*005556b0*/ extern RwStream *GeomAnimReadStream(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
/*004dc8b0*/ extern RwStream *RwStreamWriteInt32(RwStream *stream, const void *ints, RwUInt32 numBytes);
/*004dbfc0*/ extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);
void *AtomicAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
void *GeomAnimDtor(void *object, RwInt32 offset, RwInt32 size);
void *GeomAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
RwStream *GeomAnimWriteStream(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwInt32 GeomAnimSizeStream(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

typedef struct rpBinaryAnimInterpolator {      /* _rpBinaryAnimInterpolator: the stream form of an RpMorphInterpolator */
    RwInt32 flags;
    RwInt32 startMorphTarget;
    RwInt32 endMorphTarget;
    RwReal time;
    RwInt32 nextInterpIndex;
} rpBinaryAnimInterpolator;
#define rpMORPH_HINT 0x30105                /* rwID_MORPHPLUGIN | rwMEMHINTDUR_EVENT */

#define RWATOMICGETANIM(atomic) ((RpMorphAtomicAnim *)((RwUInt8 *)(atomic) + atomicAnimOffset))
#define RWATOMICGETCONSTANIM(atomic) ((const RpMorphAtomicAnim *)((const RwUInt8 *)(atomic) + atomicAnimOffset))
#define RPGEOMETRYGETANIM(geometry) ((RpMorphGeometryAnim *)((RwUInt8 *)(geometry) + geomAnimOffset))
/* rpworld.h: the interpolator is dirty; the atomic's frame gets an update unless the interpolator says not to */
#define RpInterpolatorDirty(ip, ap)                                                                             \
    ((ip)->flags |= (rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE),                                  \
     (!((ip)->flags & rpINTERPOLATORNOFRAMEDIRTY) ? ((ap)->frame ? RwFrameUpdateObjects((ap)->frame) : NULL) : NULL))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *RpMorphGeometrySetInterpolator(void *geometry, RwInt32 interpNum, RwInt32 startKey, RwInt32 endKey, RwReal time)
{
    RpMorphGeometryAnim *geomAnim = RPGEOMETRYGETANIM(geometry);
    RpMorphInterpolator *interpolator = &geomAnim->allInterps[interpNum];

    interpolator->flags = 0;
    interpolator->startMorphTarget = (RwInt16)startKey;
    interpolator->endMorphTarget = (RwInt16)endKey;
    interpolator->time = time;
    interpolator->recipTime = ((RwReal)(1.0)) / (time);

    if (interpNum == (geomAnim->numInterpolators - 1))
    {
        interpolator->next = &geomAnim->allInterps[0];
    }
    else
    {
        interpolator->next = interpolator + 1;
    }
    return geometry;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *AtomicAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size)
{
    RpMorphAtomicAnim *dstAtomicAnim = RWATOMICGETANIM(dstObject);
    const RpMorphAtomicAnim *srcAtomicAnim = RWATOMICGETCONSTANIM(srcObject);
    RpAtomicHead *dstAtomic = (RpAtomicHead *)dstObject;
    RpInterpolator *dstInterpolator = &dstAtomic->interpolator;
    const void *geometry = ((const RpAtomicHead *)srcObject)->geometry;

    /* If the geometry has changed underneath the atomic, don't copy anything */
    if (srcAtomicAnim->lastGeom != geometry)
    {
        dstAtomicAnim->interp = NULL;
        dstAtomicAnim->position = (RwReal)(0.0);
        dstAtomicAnim->lastGeom = NULL;
    }
    else
    {
        /* Duplicate all the fields - we assume that the atomic shares the geometry */
        dstAtomicAnim->interp = srcAtomicAnim->interp;
        dstAtomicAnim->position = srcAtomicAnim->position;
        dstAtomicAnim->lastGeom = srcAtomicAnim->lastGeom;

        /* Also update the interpolator in the destination if an interpolator is set up */
        if (dstAtomicAnim->interp)
        {
            /* RpInterpolatorSetStartMorphTarget */
            dstInterpolator->startMorphTarget = dstAtomicAnim->interp->startMorphTarget;
            RpInterpolatorDirty(dstInterpolator, dstAtomic);
            /* RpInterpolatorSetEndMorphTarget */
            dstInterpolator->endMorphTarget = dstAtomicAnim->interp->endMorphTarget;
            RpInterpolatorDirty(dstInterpolator, dstAtomic);
            /* RpInterpolatorSetScale */
            dstInterpolator->time = dstAtomicAnim->interp->time;
            dstInterpolator->recipTime = ((RwReal)(1.0)) / (dstAtomicAnim->interp->time);
            RpInterpolatorDirty(dstInterpolator, dstAtomic);
            /* RpInterpolatorSetValue */
            dstInterpolator->position = dstAtomicAnim->position;
            RpInterpolatorDirty(dstInterpolator, dstAtomic);
        }
    }
    return dstObject;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *GeomAnimDtor(void *object, RwInt32 offset, RwInt32 size)
{
    RpMorphGeometryAnim *geomAnim = RPGEOMETRYGETANIM(object);

    if (geomAnim->allInterps)
    {
        /* Free up the interpolator memory allocated in RpMorphGeometryCreateInterpolators */
        RwFree(geomAnim->allInterps);
    }
    geomAnim->allInterps = NULL;
    geomAnim->numInterpolators = 0;
    return object;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *GeomAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size)
{
    RpMorphGeometryAnim *dstGeomAnim = RPGEOMETRYGETANIM(dstObject);
    const RpMorphGeometryAnim *srcGeomAnim = RPGEOMETRYGETANIM(srcObject);

    /* Only need to do stuff if the src has interpolators - dst has already been ctor'ed */
    if (srcGeomAnim->numInterpolators > 0)
    {
        /* Duplicate the interpolators */
        RwUInt32 sizeOfInterps = srcGeomAnim->numInterpolators * sizeof(RpMorphInterpolator);

        dstGeomAnim->allInterps = (RpMorphInterpolator *)RwMalloc(sizeOfInterps, rpMORPH_HINT);
        if (!dstGeomAnim->allInterps)
        {
            /* Duplication failed due to malloc failure, send error back */
            return NULL;
        }

        /* Copy across the info itself */
        dstGeomAnim->numInterpolators = srcGeomAnim->numInterpolators;
        memcpy(dstGeomAnim->allInterps, srcGeomAnim->allInterps, sizeOfInterps);
    }
    return dstObject;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *GeomAnimWriteStream(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpMorphGeometryAnim *geomAnim = RPGEOMETRYGETANIM(object);
    RwInt32 numInterpolators;
    RwInt32 i;

    numInterpolators = geomAnim->numInterpolators;
    if (!RwStreamWriteInt32(stream, &numInterpolators, sizeof(numInterpolators)))
    {
        /* Failed to write to the stream, ooops */
        return NULL;
    }

    for (i = 0; i < numInterpolators; i++)
    {
        rpBinaryAnimInterpolator binAnimInterp;

        binAnimInterp.flags = geomAnim->allInterps[i].flags;
        binAnimInterp.startMorphTarget = geomAnim->allInterps[i].startMorphTarget;
        binAnimInterp.endMorphTarget = geomAnim->allInterps[i].endMorphTarget;
        binAnimInterp.time = geomAnim->allInterps[i].time;
        binAnimInterp.nextInterpIndex = geomAnim->allInterps[i].next - geomAnim->allInterps;

        /* RwMemRealToFloat32 / RwMemLittleEndian32: no-ops on x86 */
        if (!RwStreamWrite(stream, &binAnimInterp, sizeof(binAnimInterp)))
        {
            /* Failed to write to the stream, ooops */
            return NULL;
        }
    }

    /* OK, we're all done */
    return stream;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32 GeomAnimSizeStream(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpMorphGeometryAnim *geomAnim = RPGEOMETRYGETANIM(object);

    /* No interpolators, no morph data */
    if (0 != geomAnim->numInterpolators)
    {
        RwInt32 size;
        size = sizeof(RwInt32);             /* numInterpolators */
        size += geomAnim->numInterpolators * sizeof(rpBinaryAnimInterpolator);
        return size;
    }
    return 0;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool RpMorphPluginAttach(void)
{
    RwInt32 offset;

    offset = RwEngineRegisterPlugin(0, rwID_MORPHPLUGIN, (void *)AnimOpen, (void *)AnimClose);
    if (offset < 0)
    {
        return FALSE;
    }

    /* Extend into geometries */

    /* MORPH ANIMATION */
    atomicAnimOffset = RpAtomicRegisterPlugin(sizeof(RpMorphAtomicAnim), rwID_MORPHPLUGIN, (void *)AtomicAnimCtorDtor,
                                              (void *)AtomicAnimCtorDtor, (void *)AtomicAnimCopy);
    if (atomicAnimOffset < 0)
    {
        return FALSE;
    }

    /* No details to save from the atomic - everything'll work once we add some time */

    /* MORPH ANIMATION */
    geomAnimOffset = RpGeometryRegisterPlugin(sizeof(RpMorphGeometryAnim), rwID_MORPHPLUGIN, (void *)GeomAnimCtor,
                                              (void *)GeomAnimDtor, (void *)GeomAnimCopy);
    if (geomAnimOffset < 0)
    {
        return FALSE;
    }

    offset = RpGeometryRegisterPluginStream(rwID_MORPHPLUGIN, (void *)GeomAnimReadStream, (void *)GeomAnimWriteStream,
                                            (void *)GeomAnimSizeStream);
    if (offset < 0)
    {
        return FALSE;
    }
    return TRUE;
}
