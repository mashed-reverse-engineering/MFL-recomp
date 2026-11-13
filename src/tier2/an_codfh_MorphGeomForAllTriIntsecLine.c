/* MorphGeomForAllTriIntsecLine (MFL.exe 0054ccf0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwInvSqrtMacro(_recip,_input) ( *(_recip) = _rwInvSqrt(_input) )
#define RwV3dCrossProductMacro(o,a,b) MACRO_START { (o)->x = (( (((a)->y) * ( (b)->z))) - ( (((a)->z) * ( (b)->y)))); (o)->y = (( (((a)->z) * ( (b)->x))) - ( (((a)->x) * ( (b)->z)))); (o)->z = (( (((a)->x) * ( (b)->y))) - ( (((a)->y) * ( (b)->x)))); } MACRO_STOP
extern RwReal _rwInvSqrt(const RwReal num);
#define RTINTSECEPSILON (RwReal)(1e-8)
#define RTINTSECEDGEEPS (RwReal)(1e-5)
#define RtIntersectionLineTriangleMacro(_result,_lineStart,_lineDelta,_v0,_v1,_v2,_distance) MACRO_START { RwV3d edge1, edge2, tVec, pVec, qVec; RwReal det; RwV3dSubMacro(&edge1, (_v1), (_v0)); RwV3dSubMacro(&edge2, (_v2), (_v0)); RwV3dCrossProductMacro(&pVec, (_lineDelta), &edge2); det = RwV3dDotProductMacro(&edge1, &pVec); (_result) = (det > RTINTSECEPSILON); if ((_result)) { RwReal lo, hi, u; lo = - det*RTINTSECEDGEEPS; hi = det - lo; RwV3dSubMacro(&tVec, (_lineStart), (_v0)); u = RwV3dDotProductMacro(&tVec, &pVec); (_result) = (u >= lo && u <= hi); if ((_result)) { RwReal v; RwV3dCrossProductMacro(&qVec, &tVec, &edge1); v = RwV3dDotProductMacro((_lineDelta), &qVec); (_result) = (v >= lo && (u + v) <= hi); if ((_result)) { *(_distance) = RwV3dDotProductMacro(&edge2, &qVec); (_result) = (*(_distance) >= lo && *(_distance) <= hi); if ((_result)) { *(_distance) = ((*(_distance)) / (det)); } } } } } MACRO_STOP
/* ---- end of the generated declarations ---- */


#define TriangleNormalMacro(_normal,_v0,_v1,_v2) MACRO_START { RwV3d vTmp, vTmp2; RwReal recipLength; RwReal lengthSq; RwV3dSubMacro(&vTmp, (_v1), (_v0)); RwV3dSubMacro(&vTmp2, (_v2), (_v0)); RwV3dCrossProductMacro((_normal), &vTmp, &vTmp2); lengthSq = RwV3dDotProduct((_normal), (_normal)); rwInvSqrtMacro(&recipLength, lengthSq); RwV3dScaleMacro((_normal), (_normal), recipLength); } MACRO_STOP
#define CollisionTriangleInitMacro(_ct,_ind,_v0,_v1,_v2) MACRO_START { (_ct)->point = *(_v0); (_ct)->index = (_ind); (_ct)->vertices[0] = (_v0); (_ct)->vertices[1] = (_v1); (_ct)->vertices[2] = (_v2); TriangleNormalMacro(&(_ct)->normal, _v0, _v1, _v2); } MACRO_STOP
typedef struct
{
    RwV3d               tri[3];  
    RwV3d              *vertsA;
    RwV3d              *vertsB;
    RwReal              fac;

} TriLerp;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(TriLerp, vertsA) == 0x24 && RW_OFFSETOF(TriLerp, vertsB) == 0x28 && RW_OFFSETOF(TriLerp, fac) == 0x2c);      /* MFL.exe 0x54d3b9 / 0x54d3d0 / 0x54d3e4 / 0x54d405 (MorphGeomForAllTriIntsecSphere, same frame): lerp.tri at esp+0x48, vertsA +0x6c, vertsB +0x70, fac +0x74 */
#define V3dLerp(_v,_va,_vb,_blend) MACRO_START { RwV3dSub(_v, _vb, _va); RwV3dScale(_v, _v, _blend); RwV3dAdd(_v, _v, _va); } MACRO_STOP
#define TriLerpInitMacro(_lerp,_geom,_interp,_v0,_v1,_v2) MACRO_START { RWASSERT((_interp)->startMorphTarget < (_geom)->numMorphTargets); RWASSERT((_interp)->endMorphTarget < (_geom)->numMorphTargets); (_lerp)->vertsA = (_geom)->morphTarget[(_interp)->startMorphTarget].verts; (_lerp)->vertsB = (_geom)->morphTarget[(_interp)->endMorphTarget].verts; (_lerp)->fac = (_interp)->recipTime * (_interp)->position; { (_v0) = &(_lerp)->tri[0]; (_v1) = &(_lerp)->tri[1]; (_v2) = &(_lerp)->tri[2]; } } MACRO_STOP
#define TriLerpUpdateMacro(_lerp,_triangle,_v0,_v1,_v2) MACRO_START { if ((_lerp)->vertsA == (_lerp)->vertsB) { (_v0) = (_lerp)->vertsA + (_triangle)->vertIndex[0]; (_v1) = (_lerp)->vertsA + (_triangle)->vertIndex[1]; (_v2) = (_lerp)->vertsA + (_triangle)->vertIndex[2]; } else { RwV3d *va, *vb; va = (_lerp)->vertsA + (_triangle)->vertIndex[0]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[0]; V3dLerp(&(_lerp)->tri[0], va, vb, (_lerp)->fac); va = (_lerp)->vertsA + (_triangle)->vertIndex[1]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[1]; V3dLerp(&(_lerp)->tri[1], va, vb, (_lerp)->fac); va = (_lerp)->vertsA + (_triangle)->vertIndex[2]; vb = (_lerp)->vertsB + (_triangle)->vertIndex[2]; V3dLerp(&(_lerp)->tri[2], va, vb, (_lerp)->fac); } } MACRO_STOP
/* TIER2: unvalidated authority=diffemu */
void MorphGeomForAllTriIntsecLine(RpGeometry *geom, RpInterpolator *interp, RwLine *line, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data)
{
    TriLerp         lerp;
    RwV3d           delta;
    RwInt32         i;
    RwV3d          *v0, *v1, *v2;
    RpTriangle     *tri;

    RwV3dSub(&delta, &line->end, &line->start);

    TriLerpInitMacro(&lerp, geom, interp, v0, v1, v2);

    for (i=0, tri=geom->triangles; i < geom->numTriangles; ++i, ++tri)
    {
        RwReal  dist;
        RwBool  result;
        RpCollisionTriangle collTri;

        TriLerpUpdateMacro(&lerp, tri, v0, v1, v2);

        RtIntersectionLineTriangleMacro(
            result, &line->start, &delta, v0, v1, v2, &dist);

        if (result)
        {
            CollisionTriangleInitMacro(&collTri, i, v0, v1, v2);
            if (!callBack(is, &collTri, dist, data))
            {
                
                return;
            }
        }
    }

    return;
}
