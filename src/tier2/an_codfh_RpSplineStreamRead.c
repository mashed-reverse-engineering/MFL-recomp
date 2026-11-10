/* RpSplineStreamRead (MFL.exe 0x00555890): RenderWare Graphics 3.7 (Criterion Software), reads a spline (name, vertex count, type,
 * control points) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/spline/rpspline.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative32 is a no-op and gone; RwStreamReadReal is the exe's RwStreamReadInt32, the
 * linker folded the identical functions), RWAPIFUNCTION / RWASSERT / RWRETURN expanded, RWERROR expanded as the release build has it
 * (_rwerror + RwErrorSet, plugin ID rwID_SPLINEPLUGIN), rwstrncpy expanded to the engine's string function slot, the structs
 * declared here (codfh rpspline.c / rpspline.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rpSPLINENAMELENGTH 32
typedef struct _rpSpline {
    RwChar name[rpSPLINENAMELENGTH];
    RwInt32 numVertices;
    RwInt32 splineType;
} _rpSpline;
typedef struct RpSpline {
    RwUInt8 type;                       /* tType (RwObject) */
    RwUInt8 tTypePad[0x17];
    RwChar caName[rpSPLINENAMELENGTH];
} RpSpline;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_rpspline_size, sizeof(_rpSpline) == 0x28);
RW_LAYOUT_ASSERT(rw_assert_spline_name, RW_OFFSETOF(RpSpline, caName) == 0x18);

#define rwID_SPLINE 0xc
#define rwID_SPLINEPLUGIN 0x102
#define rwMEMHINTDUR_FUNCTION 0x10000
#define E_RW_NOMEM ((RwInt32)0x80000013)

typedef RwChar *(*vecStrncpyFunc)(RwChar *, const RwChar *, size_t);
#define rwstrncpy RW_GLOBAL(0xd0, vecStrncpyFunc)      /* stringFuncs.vecStrncpy */

/*007d2128*/ extern void *RwEngineInstance;
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*005569b0*/ extern RpSpline *RpSplineCreate(RwInt32 numCtrlPoints, RwInt32 type, RwV3d *ctrlPoints);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RwStreamReadReal(stream, reals, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(reals), (numBytes))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RpSpline *RpSplineStreamRead(RwStream *stream)
{
    _rpSpline s;
    RpSpline *spline;
    RwV3d *points;
    RwInt32 numVertices;

    if (RwStreamRead(stream, &s, sizeof(s)) != sizeof(s))
    {
        return NULL;
    }

    numVertices = s.numVertices;

    /* We now know the name and number of vertices of the spline: allocate memory for the control points */
    if (!(points = (RwV3d *)RwMalloc(sizeof(RwV3d) * numVertices, rwID_SPLINEPLUGIN | rwMEMHINTDUR_FUNCTION)))
    {
        RwError error;

        error.pluginID = rwID_SPLINEPLUGIN;
        error.errorCode = _rwerror(E_RW_NOMEM, (sizeof(RwV3d) * numVertices));
        RwErrorSet(&error);
        return NULL;
    }

    /* Read in the points, all in one go */
    if (!RwStreamReadReal(stream, points, sizeof(RwV3d) * numVertices))
    {
        return NULL;
    }

    /* Create a spline of the correct type */
    spline = RpSplineCreate(numVertices, s.splineType, points);

    /* Set its name */
    rwstrncpy(spline->caName, s.name, rpSPLINENAMELENGTH);

    /* set the object type */
    spline->type = rwID_SPLINE;

    /* free up those control points */
    RwFree(points);
    points = NULL;

    return spline;
}
