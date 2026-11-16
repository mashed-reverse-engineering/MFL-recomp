/* ptank GEN position instancer, constant center and size, no rotation (MFL.exe 0x005470e0): RenderWare Graphics 3.7 (Criterion Software),
 * rpPTankGENInsPosCCCSNR (constant center, constant size, no rotation).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RwV3dScale written as RwV3dScaleMacro (its release build form),
 * structs declared here (codfh ptank.h).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RpPTankLockStruct {
    RwUInt8 *data;
    RwInt32 stride;
} RpPTankLockStruct;

typedef struct RpPTankFormatDescriptor {
    RwInt32 numClusters;
    RwInt32 stride;
    RwInt32 dataFlags;
} RpPTankFormatDescriptor;

typedef struct RpPTankData {
    void *data;                         /* 0x00 */
    RpPTankLockStruct clusters[9];      /* 0x04 */
    void *userData;                     /* 0x4c */
    RpPTankFormatDescriptor format;     /* 0x50 */
    RwUInt32 srcBlend;                  /* 0x5c */
    RwUInt32 dstBlend;                  /* 0x60 */
    RwBool vertexAlphaBlend;            /* 0x64 */
    RwV2d cCenter;                      /* 0x68 */
    RwV2d cSize;                        /* 0x70 */
    RwReal cRotate;
    RwRGBA cColor;
    RwRGBA cVtxColor[4];
    RwTexCoords cUV[4];
    RwMatrix cMatrix;
} RpPTankData;
RW_LAYOUT_ASSERT(rw_assert_ptank_clusters, RW_OFFSETOF(RpPTankData, clusters) == 4);
RW_LAYOUT_ASSERT(rw_assert_ptank_ccenter, RW_OFFSETOF(RpPTankData, cCenter) == 0x68);
RW_LAYOUT_ASSERT(rw_assert_ptank_csize, RW_OFFSETOF(RpPTankData, cSize) == 0x70);

#define RPPTANKSIZEPOSITION 0

/* TIER2: unvalidated authority=diffemu */
void rpPTankGENInsPosCCCSNR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwV3d vectorSize[4];
    RwV3d centerWS;
    RwV3d *src;
    RwV3d *dst;
    RwUInt8 *uSrc;
    RwUInt32 uStride;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwV3d pRight, pUp;

    centerWS.x = ptankGlobal->cCenter.x * right->x + ptankGlobal->cCenter.y * up->x;
    centerWS.y = ptankGlobal->cCenter.x * right->y + ptankGlobal->cCenter.y * up->y;
    centerWS.z = ptankGlobal->cCenter.x * right->z + ptankGlobal->cCenter.y * up->z;

    RwV3dScaleMacro(&pRight, right, ptankGlobal->cSize.x * 0.5f);
    RwV3dScaleMacro(&pUp, up, ptankGlobal->cSize.x * 0.5f);      /* sic: cSize.x for both, as in the source */

    /* vtx 0 */
    vectorSize[0].x = (-pRight.x - pUp.x) + centerWS.x;
    vectorSize[0].y = (-pRight.y - pUp.y) + centerWS.y;
    vectorSize[0].z = (-pRight.z - pUp.z) + centerWS.z;

    /* vtx 1 */
    vectorSize[1].x = (+pRight.x - pUp.x) + centerWS.x;
    vectorSize[1].y = (+pRight.y - pUp.y) + centerWS.y;
    vectorSize[1].z = (+pRight.z - pUp.z) + centerWS.z;

    /* vtx 2 */
    vectorSize[2].x = (+pRight.x + pUp.x) + centerWS.x;
    vectorSize[2].y = (+pRight.y + pUp.y) + centerWS.y;
    vectorSize[2].z = (+pRight.z + pUp.z) + centerWS.z;

    /* vtx 3 */
    vectorSize[3].x = (-pRight.x + pUp.x) + centerWS.x;
    vectorSize[3].y = (-pRight.y + pUp.y) + centerWS.y;
    vectorSize[3].z = (-pRight.z + pUp.z) + centerWS.z;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    for (i = 0; i < pCount; i++)
    {
        src = (RwV3d *)uSrc;

        /* vtx 0 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[0].x;
        dst->y = src->y + vectorSize[0].y;
        dst->z = src->z + vectorSize[0].z;
        uDst += uDStride;

        /* vtx 1 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[1].x;
        dst->y = src->y + vectorSize[1].y;
        dst->z = src->z + vectorSize[1].z;
        uDst += uDStride;

        /* vtx 2 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[2].x;
        dst->y = src->y + vectorSize[2].y;
        dst->z = src->z + vectorSize[2].z;
        uDst += uDStride;

        /* vtx 3 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[3].x;
        dst->y = src->y + vectorSize[3].y;
        dst->z = src->z + vectorSize[3].z;
        uDst += uDStride;

        uSrc += uStride;
    }
}
