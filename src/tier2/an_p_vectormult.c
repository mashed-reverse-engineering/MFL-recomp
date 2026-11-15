/* VectorMultPoint / VectorMultPoints / VectorMultVector / VectorMultVectors (MFL.exe 004d5010 / 004d4f50 / 004d5160 / 004d50b0): the generic
 * x87 point / vector transforms that rwVectorOpen / _rwVectorSetMultFn install as the RwV3dTransformPoint(s) / Vector(s) function pointers.
 * Written from the MFL.exe disassembly (lane P W8 / W9); RenderWare SDK types from rw37_slim.h.  Compared with MFL.exe by differential
 * emulation (tier 2) at FPU control word 0x7f and 0x27f.
 * compile: /Op  (float consistency, as the original was built: each assignment to a named float local is rounded to single precision
 * through memory, which matters at 0x27f).  The rounded partial sums are the named locals ty / tz / sy (sz); every other partial sum is
 * part of one expression and stays on the x87 stack.  The input point is read completely before the output is written (in-place safe),
 * and the matrix fields the original loads before its first output store are copied before ours too (the same result when the output
 * overlaps the matrix; a copy is exact). */
#include "rw37_slim.h"

/* one point: the original's rounding points are x * right.y, x * right.z and the y sum before pos.y */
#define VECTORMULTPOINT(out, in, m)                                                     \
    {                                                                                  \
        RwReal x = (in)->x, y = (in)->y, z = (in)->z;                                  \
        RwReal upz = (m)->up.z, atz = (m)->at.z;      /* read before the first store */ \
        RwReal ty, tz, sy;                                                             \
                                                                                       \
        ty = x * (m)->right.y;                                                         \
        tz = x * (m)->right.z;                                                         \
        sy = (m)->at.y * z + ((m)->up.y * y + ty);                                     \
        (out)->x = ((m)->at.x * z + ((m)->up.x * y + x * (m)->right.x)) + (m)->pos.x;  \
        (out)->y = (m)->pos.y + sy;                                                    \
        (out)->z = (m)->pos.z + (atz * z + (upz * y + tz));                            \
    }

/* one vector: the original's rounding points are x * right.y, x * right.z and the z sum after up.z */
#define VECTORMULTVECTOR(out, in, m)                                                    \
    {                                                                                  \
        RwReal x = (in)->x, y = (in)->y, z = (in)->z;                                  \
        RwReal upy = (m)->up.y;      /* read before the first store */                 \
        RwReal ty, tz, sz;                                                             \
                                                                                       \
        ty = x * (m)->right.y;                                                         \
        tz = x * (m)->right.z;                                                         \
        sz = (m)->up.z * y + tz;                                                       \
        (out)->x = (m)->at.x * z + ((m)->up.x * y + x * (m)->right.x);                 \
        (out)->y = (m)->at.y * z + (upy * y + ty);                                     \
        (out)->z = (m)->at.z * z + sz;                                                 \
    }

/* TIER2: unvalidated authority=diffemu */
RwV3d * VectorMultPoint(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix)
{
    VECTORMULTPOINT(pointOut, pointIn, matrix);
    return (pointOut);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwV3d * VectorMultPoints(RwV3d *pointsOut, const RwV3d *pointsIn, RwInt32 numPoints, const RwMatrix *matrix)
{
    RwV3d *out = pointsOut;

    while (--numPoints >= 0)
    {
        VECTORMULTPOINT(out, pointsIn, matrix);
        pointsIn++;
        out++;
    }
    return (pointsOut);
}

/* TIER2: unvalidated authority=diffemu */
RwV3d * VectorMultVector(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix)
{
    VECTORMULTVECTOR(vectorOut, vectorIn, matrix);
    return (vectorOut);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwV3d * VectorMultVectors(RwV3d *vectorsOut, const RwV3d *vectorsIn, RwInt32 numPoints, const RwMatrix *matrix)
{
    RwV3d *out = vectorsOut;

    while (--numPoints >= 0)
    {
        VECTORMULTVECTOR(out, vectorsIn, matrix);
        vectorsIn++;
        out++;
    }
    return (vectorsOut);
}
