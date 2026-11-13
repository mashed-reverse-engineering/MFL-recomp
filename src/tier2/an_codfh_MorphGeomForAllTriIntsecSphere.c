/* MorphGeomForAllTriIntsecSphere (MFL.exe 0054d390). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwBool
RtIntersectionSphereTriangle(const RwSphere *sphere,
                             const RwV3d *v0,
                             const RwV3d *v1,
                             const RwV3d *v2,
                             RwV3d *normal,
                             RwReal *distance);
/* ---- end of the generated declarations ---- */


typedef struct
{
    RwV3d               tri[3];  
    RwV3d              *vertsA;
    RwV3d              *vertsB;
    RwReal              fac;

} TriLerp;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(TriLerp, vertsA) == 0x24 && RW_OFFSETOF(TriLerp, vertsB) == 0x28 && RW_OFFSETOF(TriLerp, fac) == 0x2c);      /* MFL.exe 0x54d3b9 / 0x54d3d0 / 0x54d3e4 / 0x54d405: lerp.tri at esp+0x48, vertsA +0x6c, vertsB +0x70, fac +0x74 */
#define V3dLerp(_v,_va,_vb,_blend) MACRO_START { RwV3dSub(_v, _vb, _va); RwV3dScale(_v, _v, _blend); RwV3dAdd(_v, _v, _va); } MACRO_STOP
#define TriLerpInitMacro(_lerp,_geom,_interp,_v0,_v1,_v2) MACRO_START { RWASSERT((_interp)->startMorphTarget < (_geom)->numMorphTargets); RWASSERT((_interp)->endMorphTarget < (_geom)->numMorphTargets); (_lerp)->vertsA = (_geom)->morphTarget[(_interp)->startMorphTarget].verts; (_lerp)->vertsB = (_geom)->morphTarget[(_interp)->endMorphTarget].verts; (_lerp)->fac = (_interp)->recipTime * (_interp)->position; { (_v0) = &(_lerp)->tri[0]; (_v1) = &(_lerp)->tri[1]; (_v2) = &(_lerp)->tri[2]; } } MACRO_STOP
#define TriLerpUpdateMacro(_lerp,_triangle,_v0,_v1,_v2) MACRO_START { if ((_lerp)->vertsA == (_lerp)->vertsB) { (_v0) = (_lerp)->vertsA + (_triangle)->vertIndex[0]; (_v1) = (_lerp)->vertsA + (_triangle)->vertIndex[1]; (_v2) = (_lerp)->vertsA + (_triangle)->vertIndex[2]; } else { RwV3d *va, *vb; va = (_lerp)->vertsA + (_triangle)->vertIndex[0]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[0]; V3dLerp(&(_lerp)->tri[0], va, vb, (_lerp)->fac); va = (_lerp)->vertsA + (_triangle)->vertIndex[1]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[1]; V3dLerp(&(_lerp)->tri[1], va, vb, (_lerp)->fac); va = (_lerp)->vertsA + (_triangle)->vertIndex[2]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[2]; V3dLerp(&(_lerp)->tri[2], va, vb, (_lerp)->fac); } } MACRO_STOP
/* TIER2: unvalidated authority=diffemu */
void MorphGeomForAllTriIntsecSphere(RpGeometry *geom, RpInterpolator *interp, RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data)
{
    RwReal          recipRad;
    TriLerp         lerp;
    RwV3d          *v0, *v1, *v2;
    RwInt32         i;
    RpTriangle     *tri;

    recipRad = 1.0f / sphere->radius;

    TriLerpInitMacro(&lerp, geom, interp, v0, v1, v2);

    for (i=0, tri=geom->triangles; i < geom->numTriangles; ++i, ++tri)
    {
        RwReal  dist;
        RpCollisionTriangle collTri;

        TriLerpUpdateMacro(&lerp, tri, v0, v1, v2);

        if (RtIntersectionSphereTriangle(
                sphere, v0, v1, v2, &collTri.normal, &dist))
        {
            collTri.point = (*v0);
            collTri.index = i;
            collTri.vertices[0] = v0;
            collTri.vertices[1] = v1;
            collTri.vertices[2] = v2;

            dist *= recipRad;
            if (!callBack(is, &collTri, dist, data))
            {
                
                return;
            }
        }
    }

    return;
}
