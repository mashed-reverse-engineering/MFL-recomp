/* RpAtomicForAllIntersections (MFL.exe 0054cbe0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper AtomicForAllTriIntsecLine
 * inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwMatrix *
RwMatrixInvert(RwMatrix * matrixOut,
               const RwMatrix * matrixIn);
extern RwV3d *RwV3dTransformPoints(RwV3d * pointsOut,
                                   const RwV3d * pointsIn,
                                   RwInt32 numPoints,
                                   const RwMatrix * matrix);
struct RwFrame
{
    RwObject object;

    RwLLLink inDirtyListLink;

    RwMatrix modelling;
    RwMatrix ltm;

    RwLinkList objectList;

    struct RwFrame *child;
    struct RwFrame *next;
    struct RwFrame *root;


};
extern RwMatrix *
RwFrameGetLTM(RwFrame * frame);
extern const RwSphere *
RpAtomicGetWorldBoundingSphere(RpAtomic * atomic);
enum RpIntersectType
{
    rpINTERSECTNONE = 0,
    rpINTERSECTLINE,
    rpINTERSECTPOINT,
    rpINTERSECTSPHERE,
    rpINTERSECTBOX,
    rpINTERSECTATOMIC,
    rpINTERSECTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpIntersectType RpIntersectType;
typedef union RpIntersectData RpIntersectData;
union RpIntersectData
{
    RwLine line;
    RwV3d point;
    RwSphere sphere;
    RwBBox box;
    void *object;

};
struct RpIntersection
{
    RpIntersectData t;

    RpIntersectType type;

};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwFrame, modelling) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwFrame, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwFrame, inDirtyListLink) == 8);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RwFrame, objectList) == 0x90);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RwFrame, child) == 0x98);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RwFrame, next) == 0x9c);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RwFrame, root) == 0xa0);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RwFrame, ltm) == 0x50);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_row_8, sizeof(RwFrame) == 0xa4);      /* exe: exe static frameTKList.sizeOfStruct at 0x0062e514 */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpIntersection, type) == 0x18 && sizeof(RpIntersectData) == 0x18);      /* MFL.exe 0x54cbe8 mov eax,[esi+0x18] (RpAtomicForAllIntersections: switch on intersection->type) */
/* ---- end of the generated declarations ---- */

extern void GeometryForAllTriIntsecLine(RpGeometry *geometry, RwLine *line, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data);
extern void MorphGeomForAllTriIntsecLine(RpGeometry *geom, RpInterpolator *interp, RwLine *line, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data);
extern void AtomicForAllTriIntsecSphere(RpAtomic *atomic, const RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data);
/* TIER2: unvalidated authority=diffemu */
RpAtomic * RpAtomicForAllIntersections(RpAtomic * atomic, RpIntersection * intersection, RpIntersectionCallBackGeometryTriangle callBack, void *data)
{

    switch (intersection->type)
    {
    case rpINTERSECTLINE:
        {
            /* AtomicForAllTriIntsecLine, inlined as in MFL.exe: the line in the atomic's local space */
            RpGeometry     *geom = atomic->geometry;
            RwMatrix        inverseLTM;
            RwLine          localLine;

            RwMatrixInvert(&inverseLTM, RwFrameGetLTM(RpAtomicGetFrame(atomic)));
            RwV3dTransformPoints(&localLine.start, &intersection->t.line.start, 2, &inverseLTM);

            if (geom->numMorphTargets > 1)
            {
                MorphGeomForAllTriIntsecLine(
                    geom, &atomic->interpolator, &localLine, intersection, callBack, data);
            }
            else
            {
                GeometryForAllTriIntsecLine(geom, &localLine, intersection, callBack, data);
            }
            return (atomic);
        }

    case rpINTERSECTSPHERE:
        {
            AtomicForAllTriIntsecSphere(
                atomic, &intersection->t.sphere, intersection, callBack, data);
            return (atomic);
        }

    case rpINTERSECTATOMIC:
        {
            RpAtomic *intsecAtomic = (RpAtomic *)intersection->t.object;
            RwSphere sphere;

            sphere = *RpAtomicGetWorldBoundingSphere(intsecAtomic);

            AtomicForAllTriIntsecSphere(
                atomic, &sphere, intersection, callBack, data);
            return (atomic);
        }

    default:
        {

            return (atomic);
        }
    }
}
