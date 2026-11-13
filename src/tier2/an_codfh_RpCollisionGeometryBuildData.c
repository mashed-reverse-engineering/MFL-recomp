/* RpCollisionGeometryBuildData (MFL.exe 0054da20). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collbuild.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RpCollTree RpCollTree;
extern void
_rpCollTreeDestroy(RpCollTree *tree);
typedef struct RpCollisionData RpCollisionData;
struct RpCollisionData
{
    RpCollTree *tree;
};
#define _rpCollisionGeometryGetDataMacro(_geometry) RWPLUGINOFFSET(RpCollisionData, _geometry, _rpCollisionGeometryDataOffset)
#define RpCollisionGeometryGetCollTree(_geometry) (_rpCollisionGeometryGetDataMacro(_geometry)->tree)
extern RwInt32 _rpCollisionGeometryDataOffset;
extern RpGeometry *
_rpCollisionGeometrySetCollTree(RpGeometry *geometry,
                                RpCollTree *collTree);
typedef struct RpCollisionBuildParam RpCollisionBuildParam;
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpCollisionData, tree) == 0);      /* MFL.exe 0x54da51 / 0x54da56 (RpCollisionGeometryBuildData): mov eax,[_rpCollisionGeometryDataOffset + geometry] = the old tree (the struct's only member) */
/* ---- end of the generated declarations ---- */

/* _rpCollTreeDestroy is folded in MFL.exe: 0x53e3b0 is also RtAnimInterpolatorDestroy (call at 0x54da5e) */
extern RpCollTree * BuildTriListCollTree(void *object, RwUInt32 numVerts, RwV3d *verts, RwUInt32 numTris, RpTriangle *tris, RpTriangle *sortTris, RpCollisionBuildParam *param);
/* TIER2: unvalidated authority=diffemu */
RpGeometry * RpCollisionGeometryBuildData(RpGeometry *geometry, RpCollisionBuildParam *param)
{
    RpCollTree  *oldTree, *collTree;

    collTree = BuildTriListCollTree(
        geometry,
        geometry->numVertices,
        geometry->morphTarget->verts,
        geometry->numTriangles,
        geometry->triangles,
        geometry->triangles, 
        param);

    if (!collTree)
    {
        return ((RpGeometry *)NULL);
    }

    oldTree = RpCollisionGeometryGetCollTree(geometry);
    if (oldTree)
    {
        _rpCollTreeDestroy(oldTree);
    }

    _rpCollisionGeometrySetCollTree(geometry, collTree);

    return (geometry);
}
