/* AtomicForAllTriIntsecSphere (MFL.exe 0054d2d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwSqrt(_result,_x) rwSqrtMacro(_result, _x)
extern RwMatrix *
RwMatrixInvert(RwMatrix * matrixOut,
               const RwMatrix * matrixIn);
#define rwSqrtMacro(_root,_input) ( *(_root) = _rwSqrt(_input) )
extern RwV3d *RwV3dTransformPoints(RwV3d * pointsOut,
                                   const RwV3d * pointsIn,
                                   RwInt32 numPoints,
                                   const RwMatrix * matrix);
extern RwReal _rwSqrt(const RwReal num);
extern RwMatrix *
RwFrameGetLTM(RwFrame * frame);
/* ---- end of the generated declarations ---- */


extern void GeometryForAllTriIntsecSphere(RpGeometry *geometry, const RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data);
extern void MorphGeomForAllTriIntsecSphere(RpGeometry *geom, RpInterpolator *interp, RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data);
/* TIER2: unvalidated authority=diffemu */
void AtomicForAllTriIntsecSphere(RpAtomic *atomic, const RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data)
{
    RpGeometry         *geom;
    RwSphere            localSphere;
    RwMatrix            inverseLTM;
    RwReal              scaleSq, scale;

    geom = atomic->geometry;

    RwMatrixInvert(&inverseLTM, RwFrameGetLTM(RpAtomicGetFrame(atomic)));

    RwV3dTransformPoints(&localSphere.center, &sphere->center, 1, &inverseLTM);

    scaleSq = RwV3dDotProduct(&inverseLTM.at, &inverseLTM.at);
    rwSqrt(&scale, scaleSq);
    localSphere.radius = scale * sphere->radius;

    if (geom->numMorphTargets > 1)
    {
        MorphGeomForAllTriIntsecSphere(
            geom, &atomic->interpolator, &localSphere, is, callBack, data);
    }
    else
    {
        GeometryForAllTriIntsecSphere(geom, &localSphere, is, callBack, data);
    }

    return;
}
