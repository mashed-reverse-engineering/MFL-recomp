/* GeometryForAllTriIntsecSphere (MFL.exe 0054c400). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * SimpleGeomForAllTriIntsecSphere inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define GETCOORD(vect,y) (*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define rwInvSqrtMacro(_recip,_input) ( *(_recip) = _rwInvSqrt(_input) )
#define RwV3dCrossProductMacro(o,a,b) MACRO_START { (o)->x = (( (((a)->y) * ( (b)->z))) - ( (((a)->z) * ( (b)->y)))); (o)->y = (( (((a)->z) * ( (b)->x))) - ( (((a)->x) * ( (b)->z)))); (o)->z = (( (((a)->x) * ( (b)->y))) - ( (((a)->y) * ( (b)->x)))); } MACRO_STOP
extern RwReal _rwInvSqrt(const RwReal num);
extern RwBool
RtIntersectionSphereTriangle(const RwSphere *sphere,
                             const RwV3d *v0,
                             const RwV3d *v1,
                             const RwV3d *v2,
                             RwV3d *normal,
                             RwReal *distance);
#define rpCOLLTREE_MAXDEPTH 32
#define rpCOLLSECTOR_CONTENTS_SPLIT 0xff
#define rpCOLLSECTOR_TYPE_AXISMASK (0x08 | 0x04)
typedef struct RpCollSector RpCollSector;
typedef struct RpCollTree RpCollTree;
struct RpCollSector
{
    RwUInt8 type;
    RwUInt8 contents;
    RwUInt16 index;
    RwReal value;
};
typedef struct RpCollSplit RpCollSplit;
struct RpCollSplit
{
    RpCollSector leftSector;
    RpCollSector rightSector;
};
struct RpCollTree
{
    RwUInt32 flags;
    RwBBox bbox;
    RwUInt16 numEntries;
    RwUInt16 numSplits;
    RpCollSplit *splits;
    RwUInt16 *map;
};
typedef struct RpCollSectorBBoxItNodeInfo RpCollSectorBBoxItNodeInfo;
struct RpCollSectorBBoxItNodeInfo
{
    RpCollSector *sector;
    RwUInt32 clipFlags;
};
struct RpCollSectorBBoxIt
{
    RpCollTree *tree;
    RwBBox bbox;

    RwBBox clipBBox;

    RpCollSector dummy;
    RpCollSectorBBoxItNodeInfo stack[rpCOLLTREE_MAXDEPTH+1];
    RpCollSectorBBoxItNodeInfo *curr;
};
enum RpCollBBoxClipFlags
{
    rpCOLLBBOXCLIP_INFX = 0x01,
    rpCOLLBBOXCLIP_SUPX = 0x02,
    rpCOLLBBOXCLIP_INFY = 0x04,
    rpCOLLBBOXCLIP_SUPY = 0x08,
    rpCOLLBBOXCLIP_INFZ = 0x10,
    rpCOLLBBOXCLIP_SUPZ = 0x20,

    rpCOLLBBOXCLIP_MASK = 0x3f,

    rpCOLLBBOXCLIPFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpCollBBoxClipFlags RpCollBBoxClipFlags;
typedef struct RpCollSectorBBoxIt RpCollSectorBBoxIt;
#define _rpCollSectorBBoxItFinishedMacro(_it) ((_it)->curr <= (_it)->stack)
#define RpCollSectorBBoxItFinished(_it) _rpCollSectorBBoxItFinishedMacro(_it)
typedef struct RpCollisionData RpCollisionData;
struct RpCollisionData
{
    RpCollTree *tree;
};
#define _rpCollisionGeometryGetDataMacro(_geometry) RWPLUGINOFFSET(RpCollisionData, _geometry, _rpCollisionGeometryDataOffset)
#define RpCollisionGeometryGetCollTree(_geometry) (_rpCollisionGeometryGetDataMacro(_geometry)->tree)
extern RwInt32 _rpCollisionGeometryDataOffset;
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
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpCollTree, numEntries) == 0x1c);      /* an_codfh_colldata.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpCollTree, splits) == 0x20);      /* an_codfh_colldata.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpCollTree, map) == 0x24);      /* an_codfh_colldata.c */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RpCollSplit) == 0x10);      /* an_codfh_colldata.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpCollTree, numSplits) == 0x1e);      /* an_codfh_rpCollTreeStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpCollisionData, tree) == 0);      /* MFL.exe 0x54da51 / 0x54da56 (RpCollisionGeometryBuildData): mov eax,[_rpCollisionGeometryDataOffset + geometry] = the old tree (the struct's only member) */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpIntersection, type) == 0x18 && sizeof(RpIntersectData) == 0x18);      /* MFL.exe 0x54cbe8 mov eax,[esi+0x18] (RpAtomicForAllIntersections: switch on intersection->type) */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpCollSector, contents) == 1 && RW_OFFSETOF(RpCollSector, index) == 2 && RW_OFFSETOF(RpCollSector, value) == 4);      /* MFL.exe 0x54c66d / 0x54c675 (it.dummy at esp+0xa8: index +0xaa, contents +0xa9); 0x54c774 / 0x54c783 / 0x54c79d (contents, index, split->...value) */
RW_LAYOUT_ASSERT(rw37_row_8, sizeof(RpCollSectorBBoxItNodeInfo) == 8 && RW_OFFSETOF(RpCollSectorBBoxItNodeInfo, clipFlags) == 4);      /* MFL.exe 0x54c768 sub edi,0x8 (curr--) / 0x54c761 stack[0].clipFlags at esp+0xb4 / 0x54c77e */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpCollSectorBBoxIt, clipBBox) == 0x1c && RW_OFFSETOF(RpCollSectorBBoxIt, dummy) == 0x34 && RW_OFFSETOF(RpCollSectorBBoxIt, stack) == 0x3c && RW_OFFSETOF(RpCollSectorBBoxIt, curr) == 0x144);      /* MFL.exe 0x54c669..0x54c76b (GeometryForAllTriIntsecSphere, it at esp+0x74): bbox copy to +0x78, clipBBox +0x90, dummy +0xa8, stack[0].sector +0xb0, curr +0x1b8 */
/* ---- end of the generated declarations ---- */

#define _rpCollSectorGetEntriesMacro(_tree,_sector,_start,_count) MACRO_START { RpCollSplit *split = (_tree)->splits + (_sector)->index; RpCollSector *leftLeaf, *rightLeaf; leftLeaf = &split->leftSector; while (leftLeaf->contents == rpCOLLSECTOR_CONTENTS_SPLIT) leftLeaf = &(_tree)->splits[leftLeaf->index].leftSector; rightLeaf = &split->rightSector; while (rightLeaf->contents == rpCOLLSECTOR_CONTENTS_SPLIT) rightLeaf = &(_tree)->splits[rightLeaf->index].rightSector; *(_start) = leftLeaf->index; *(_count) = (RwUInt16) (rightLeaf->index + rightLeaf->contents - leftLeaf->index); } MACRO_STOP
#define _rpCollBBoxBBoxTestMacro(_bb1,_bb2) ( ((_bb1)->inf.x <= (_bb2)->sup.x) && ((_bb1)->inf.y <= (_bb2)->sup.y) && ((_bb1)->inf.z <= (_bb2)->sup.z) && ((_bb1)->sup.x >= (_bb2)->inf.x) && ((_bb1)->sup.y >= (_bb2)->inf.y) && ((_bb1)->sup.z >= (_bb2)->inf.z) )
#define _rpCollBBoxInBBoxClipFlagsMacro(_bb,_clipbb) ( (((_bb)->inf.x < (_clipbb)->inf.x) ? rpCOLLBBOXCLIP_INFX:0) | (((_bb)->inf.y < (_clipbb)->inf.y) ? rpCOLLBBOXCLIP_INFY:0) | (((_bb)->inf.z < (_clipbb)->inf.z) ? rpCOLLBBOXCLIP_INFZ:0) | (((_bb)->sup.x > (_clipbb)->sup.x) ? rpCOLLBBOXCLIP_SUPX:0) | (((_bb)->sup.y > (_clipbb)->sup.y) ? rpCOLLBBOXCLIP_SUPY:0) | (((_bb)->sup.z > (_clipbb)->sup.z) ? rpCOLLBBOXCLIP_SUPZ:0) )

#define _rpCollItInitAsFinishedMacro(_it) ((_it)->curr = (_it)->stack)

#define _rpCollBBoxClipItInitMacro(_it,_root,_rootBB,_testBB,_clipBB,_flags) MACRO_START { (_it)->bbox = *(_testBB); (_it)->clipBBox = *(_clipBB); (_it)->stack[0].sector = (_root); (_it)->stack[0].clipFlags = _rpCollBBoxInBBoxClipFlagsMacro(_rootBB, _clipBB); (_it)->curr = (_it)->stack + 1; } MACRO_STOP

#define _rpCollBBoxClipItStepMacro(_it,_axis,_left,_right,_leftHi,_rightLo) MACRO_START { if ((_rightLo) > GETCOORD((_it)->bbox.sup, _axis)) { (_it)->curr->sector = (_left); } else { (_it)->curr->sector = (_right); if ((_leftHi) >= GETCOORD((_it)->bbox.inf, _axis)) { (_it)->curr++; (_it)->curr->sector = (_left); (_it)->curr->clipFlags = ((_it)->curr - 1)->clipFlags; if ((_leftHi) <= GETCOORD((_it)->clipBBox.sup, _axis)) { (_it)->curr->clipFlags &= ~(rpCOLLBBOXCLIP_SUPX << ((_axis)>>1)); } if ((_rightLo) >= GETCOORD((_it)->clipBBox.inf, _axis)) { ((_it)->curr - 1)->clipFlags &= ~(rpCOLLBBOXCLIP_INFX << ((_axis)>>1)); } } } } MACRO_STOP

#define _rpCollSectorItInitCollTreeMacro(_it,_tree) { (_it)->tree = (_tree); (_it)->dummy.index = 0; (_it)->dummy.contents = rpCOLLSECTOR_CONTENTS_SPLIT; if ((_tree)->numSplits == 0) { (_it)->dummy.contents = (RwUInt8) (_tree)->numEntries; } }
#define _rpCollSectorBBoxItInitMacro(_it,_tree,_bbox,_clipBBox) MACRO_START { if (_rpCollBBoxBBoxTestMacro(&(_tree)->bbox, _bbox)) { _rpCollSectorItInitCollTreeMacro(_it, _tree); _rpCollBBoxClipItInitMacro( _it, &(_it)->dummy, &(_tree)->bbox, _bbox, _clipBBox, flags); } else { _rpCollItInitAsFinishedMacro(_it); } } MACRO_STOP
#define _rpCollSectorBBoxItFindNextEntriesMacro(_it,_start,_count,_clip) MACRO_START { (_it)->curr--; for ( ;; ) { RpCollSector *sector = (_it)->curr->sector; RpCollSplit *split; RwUInt32 axis; if (sector->contents != rpCOLLSECTOR_CONTENTS_SPLIT) { *(_start) = sector->index; *(_count) = sector->contents; *(_clip) = (_it)->curr->clipFlags; break; } if ((_it)->curr->clipFlags == 0) { _rpCollSectorGetEntriesMacro( (_it)->tree, sector, _start, _count); *(_clip) = 0; break; } split = (_it)->tree->splits + sector->index; axis = split->leftSector.type & rpCOLLSECTOR_TYPE_AXISMASK; _rpCollBBoxClipItStepMacro(_it, axis, &split->leftSector, &split->rightSector, split->leftSector.value, split->rightSector.value); } } MACRO_STOP
#define CUBEINSPHEREFACTOR 0.57f
#define IntervalDistMacro(_v,_a,_b) (((_v)<(_a)) ? ((_a)-(_v)) : (((_v)>(_b)) ? ((_v)-(_b)) : 0))
#define SphereBBoxTestMacro(_result,_sph,_bb,_distSq) MACRO_START { RwReal dist; dist = IntervalDistMacro((_sph)->center.x, (_bb)->inf.x, (_bb)->sup.x); *(_distSq) = dist*dist; dist = IntervalDistMacro((_sph)->center.y, (_bb)->inf.y, (_bb)->sup.y); *(_distSq) += dist*dist; dist = IntervalDistMacro((_sph)->center.z, (_bb)->inf.z, (_bb)->sup.z); *(_distSq) += dist*dist; (_result) = (*(_distSq) <= ((_sph)->radius * (_sph)->radius)); } MACRO_STOP
#define SphereBBoxMacro(_bb,_center,_rad) MACRO_START { (_bb)->inf = (_bb)->sup = *(_center); (_bb)->inf.x -= (_rad); (_bb)->inf.y -= (_rad); (_bb)->inf.z -= (_rad); (_bb)->sup.x += (_rad); (_bb)->sup.y += (_rad); (_bb)->sup.z += (_rad); } MACRO_STOP
#define TriangleNormalMacro(_normal,_v0,_v1,_v2) MACRO_START { RwV3d vTmp, vTmp2; RwReal recipLength; RwReal lengthSq; RwV3dSubMacro(&vTmp, (_v1), (_v0)); RwV3dSubMacro(&vTmp2, (_v2), (_v0)); RwV3dCrossProductMacro((_normal), &vTmp, &vTmp2); lengthSq = RwV3dDotProduct((_normal), (_normal)); rwInvSqrtMacro(&recipLength, lengthSq); RwV3dScaleMacro((_normal), (_normal), recipLength); } MACRO_STOP
/* TIER2: unvalidated authority=diffemu */
void GeometryForAllTriIntsecSphere(RpGeometry *geometry, const RwSphere *sphere, RpIntersection *is, RpIntersectionCallBackGeometryTriangle callBack, void *data)
{
    RpCollTree         *tree;

    tree = RpCollisionGeometryGetCollTree(geometry);
    if (tree)
    {
        RwReal              recipRadius, innerSize, distSq;
        RwBBox              bbox;
        RwBBox              innerBBox;
        RwV3d              *verts;
        RpTriangle         *tris;
        RpCollSectorBBoxIt  it;
        RwBool              result;

        SphereBBoxTestMacro(result, sphere, &tree->bbox, &distSq);
        if (!result)
        {
            return;
        }

        recipRadius = 1/sphere->radius;
        innerSize = CUBEINSPHEREFACTOR * sphere->radius;
        SphereBBoxMacro(&bbox, &sphere->center, sphere->radius);
        SphereBBoxMacro(&innerBBox, &sphere->center, innerSize);

        verts = geometry->morphTarget->verts;
        tris = geometry->triangles;

        _rpCollSectorBBoxItInitMacro(&it, tree, &bbox, &innerBBox);
        while (!RpCollSectorBBoxItFinished(&it))
        {
            RwUInt16    i, start, count;
            RwUInt32    clip;

            _rpCollSectorBBoxItFindNextEntriesMacro(
                &it, &start, &count, &clip);
            for (i = start; i < (start + count); ++i)
            {
                RwUInt32    index = tree->map ? tree->map[i] : i;
                RpTriangle *tri = tris + index;
                RwV3d      *v0 = verts + tri->vertIndex[0];
                RwV3d      *v1 = verts + tri->vertIndex[1];
                RwV3d      *v2 = verts + tri->vertIndex[2];
                RwReal      dist;
                RpCollisionTriangle collTri;

                if (!clip)
                {
                    
                    RwV3d  offset;

                    TriangleNormalMacro(&collTri.normal, v0, v1, v2);
                    RwV3dSub(&offset, &sphere->center, v0);
                    dist = RwV3dDotProduct(&offset, &collTri.normal);
                }
                else if (!RtIntersectionSphereTriangle(
                            sphere, v0, v1, v2, &collTri.normal, &dist))
                {
                    continue;
                }

                dist *= recipRadius;

                collTri.point = (*v0);
                collTri.index = index;
                collTri.vertices[0] = v0;
                collTri.vertices[1] = v1;
                collTri.vertices[2] = v2;

                if (!callBack(is, &collTri, dist, data))
                {
                    
                    return;
                }
            }
        }
    }
    else
    {
        
        /* no collision tree: every triangle is tested (SimpleGeomForAllTriIntsecSphere, inlined as in MFL.exe) */
        RwReal              recipRadius;
        RwV3d              *verts;
        RpTriangle         *tri;
        RwInt32             i;

        recipRadius = 1/sphere->radius;

        verts = geometry->morphTarget->verts;
        for (i=0, tri=geometry->triangles; i < geometry->numTriangles; ++i, ++tri)
        {
            RwV3d *v0 = verts + tri->vertIndex[0];
            RwV3d *v1 = verts + tri->vertIndex[1];
            RwV3d *v2 = verts + tri->vertIndex[2];
            RpCollisionTriangle collTri;
            RwReal              dist;

            if (RtIntersectionSphereTriangle(
                    sphere, v0, v1, v2, &collTri.normal, &dist))
            {
                dist *= recipRadius;

                collTri.point = (*v0);
                collTri.index = i;
                collTri.vertices[0] = v0;
                collTri.vertices[1] = v1;
                collTri.vertices[2] = v2;

                if (!callBack(is, &collTri, dist, data))
                {
                    return;
                }
            }
        }
    }

    return;
}
