/* ptank GEN instancers (MFL.exe 0x00546ca0 - 0x00548470): RenderWare Graphics 3.7 (Criterion Software), rpptank generic instancing:
 * rpPTankGENInstance (calls the per-atomic instance callbacks) and the leaf instancers for positions (no center: constant size, per
 * particle size, per particle matrix), colour per particle and UVs (2 / 4 per particle, 2 constant).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build); CONSTANTFLAGS is 0 on D3D9, so the
 * RpPTankAtomicGetMaximumParticlesCount branch of _rpPTankGENInstance is gone (the exe has none either); the exe names (no leading
 * underscore: rpPTankGENInstance); RPATOMICPTANKPLUGINDATA as the pointer stored at g_ptank_plugin_offset (_rpPTankAtomicDataOffset)
 * in the atomic; RwV3dScale / Add / Sub expanded; structs declared here (codfh ptank.h) with compile-time layout asserts.
 * Left original: _rpPTankGENInstanceSelectCB (stores the addresses of rw_trap.txt instancers), rpPTankGENInsPosNCPSPR (inline-asm
 * fsincos stores float sine / cosine; sin / cos in C stay at x87 precision: 1-ulp DIFF) and the trapped instancers.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

extern void *memset(void *, int, size_t);

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
    RwReal cRotate;                     /* 0x78 */
    RwRGBA cColor;                      /* 0x7c */
    RwRGBA cVtxColor[4];                /* 0x80 */
    RwTexCoords cUV[4];                 /* 0x90 */
    RwMatrix cMatrix;                   /* 0xb0 */
} RpPTankData;

typedef struct RpPTANKInstanceSetupData {
    RwBool instancePositions;
    RwBool instanceUVs;
    RwBool instanceColors;
    RwBool instanceNormals;
    RpPTankLockStruct positionOut;      /* 0x10 */
    RpPTankLockStruct UVOut;            /* 0x18 */
    RpPTankLockStruct colorsOut;        /* 0x20 */
    RpPTankLockStruct normalsOut;       /* 0x28 */
    RwV3d right;                        /* 0x30 */
    RwV3d up;                           /* 0x3c */
} RpPTANKInstanceSetupData;

typedef void (*RpPTankGENInstanceSetupCallback)(RpPTANKInstanceSetupData *data, RpAtomic *atomic, RpPTankData *ptankGlobal,
                                                RwInt32 actPCount, RwUInt32 instFlags);
typedef void (*RpPTankGENInstancePosCallback)(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount,
                                              RpPTankData *ptankGlobal);
typedef void (*RpPTankGENInstanceCallback)(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal);
typedef void (*RpPTankGENInstanceEndingCallback)(RpPTANKInstanceSetupData *data, RpAtomic *atomic, RpPTankData *ptankGlobal,
                                                 RwInt32 actPCount, RwUInt32 instFlags);

typedef struct RpPTankAtomicExtPrv {
    RwInt32 maxPCount;
    RwInt32 actPCount;
    RwBool isAStructure;
    void *rawdata;
    void *defaultRenderCB;
    void *ptankCallBacks[4];            /* 0x14: alloc / create / instance / render */
    RpPTankGENInstanceSetupCallback insSetupCB;       /* 0x24 */
    RpPTankGENInstancePosCallback insPosCB;           /* 0x28 */
    RpPTankGENInstanceCallback insUVCB;               /* 0x2c */
    RpPTankGENInstanceCallback insColorsCB;           /* 0x30 */
    RpPTankGENInstanceCallback insNormalsCB;          /* 0x34 */
    RpPTankGENInstanceEndingCallback insEndingCB;     /* 0x38 */
    RwUInt32 lockFlags;
    RwUInt32 instFlags;
    RwUInt32 platFlags;
    RpPTankData publicData;             /* 0x48 */
} RpPTankAtomicExtPrv;

RW_LAYOUT_ASSERT(rw_assert_ptank_clusters, RW_OFFSETOF(RpPTankData, clusters) == 4);
RW_LAYOUT_ASSERT(rw_assert_ptank_format, RW_OFFSETOF(RpPTankData, format) == 0x50);
RW_LAYOUT_ASSERT(rw_assert_ptank_csize, RW_OFFSETOF(RpPTankData, cSize) == 0x70);
RW_LAYOUT_ASSERT(rw_assert_ptank_cuv, RW_OFFSETOF(RpPTankData, cUV) == 0x90);
RW_LAYOUT_ASSERT(rw_assert_ptank_setup_size, sizeof(RpPTANKInstanceSetupData) == 0x48);
RW_LAYOUT_ASSERT(rw_assert_ptank_setup_cb, RW_OFFSETOF(RpPTankAtomicExtPrv, insSetupCB) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_ptank_ending_cb, RW_OFFSETOF(RpPTankAtomicExtPrv, insEndingCB) == 0x38);
RW_LAYOUT_ASSERT(rw_assert_ptank_public, RW_OFFSETOF(RpPTankAtomicExtPrv, publicData) == 0x48);

#define RPPTANKSIZEPOSITION         0
#define RPPTANKSIZEMATRIX           1
#define RPPTANKSIZESIZE             3
#define RPPTANKSIZECOLOR            4
#define RPPTANKSIZE2DROTATE         6
#define RPPTANKSIZEVTX2TEXCOORDS    7
#define RPPTANKSIZEVTX4TEXCOORDS    8

#define RGBATOCOLOR(rgba) ((((RwUInt32)((rgba)->alpha)) << 24) | (((RwUInt32)((rgba)->red)) << 16) | \
                           (((RwUInt32)((rgba)->green)) << 8) | ((RwUInt32)((rgba)->blue)))

/*007da6bc*/ extern RwInt32 g_ptank_plugin_offset;      /* _rpPTankAtomicDataOffset */
#define RPATOMICPTANKPLUGINDATA(atomic) (*(RpPTankAtomicExtPrv **)((RwUInt8 *)(atomic) + g_ptank_plugin_offset))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool
rpPTankGENInstance(RpAtomic *atomic, RpPTankData *ptankGlobal, RwInt32 actPCount, RwUInt32 instFlags)
{
    RpPTANKInstanceSetupData instanceStr;
    RpPTankAtomicExtPrv *PTankPrv;
    RwInt32 instanceNum;

    instanceNum = actPCount;      /* instFlags & CONSTANTFLAGS is 0 on D3D9 */

    PTankPrv = RPATOMICPTANKPLUGINDATA(atomic);

    if (PTankPrv->insSetupCB)
    {
        memset(&instanceStr, 0, sizeof(RpPTANKInstanceSetupData));
        PTankPrv->insSetupCB(&instanceStr, atomic, ptankGlobal, instanceNum, instFlags);
    }

    if ((NULL != PTankPrv->insPosCB) && (NULL != instanceStr.positionOut.data))
    {
        PTankPrv->insPosCB(&instanceStr.positionOut, &instanceStr.right, &instanceStr.up, instanceNum, ptankGlobal);
    }

    if ((NULL != PTankPrv->insUVCB) && (NULL != instanceStr.UVOut.data))
    {
        PTankPrv->insUVCB(&instanceStr.UVOut, instanceNum, ptankGlobal);
    }

    if ((NULL != PTankPrv->insColorsCB) && (NULL != instanceStr.colorsOut.data))
    {
        PTankPrv->insColorsCB(&instanceStr.colorsOut, instanceNum, ptankGlobal);
    }

    if ((NULL != PTankPrv->insNormalsCB) && (NULL != instanceStr.normalsOut.data))
    {
        PTankPrv->insNormalsCB(&instanceStr.normalsOut, instanceNum, ptankGlobal);
    }

    if (NULL != PTankPrv->insEndingCB)
    {
        PTankPrv->insEndingCB(&instanceStr, atomic, ptankGlobal, instanceNum, instFlags);
    }

    return TRUE;
}

/* rpPTankGENInsPosCCSSRR: CC center (NC none, CC constant), SS size (CS constant, PS per particle), RR rotation (NR none, PR per particle) */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsPosNCCSNR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwV3d vectorSize[4];
    RwV3d pRight, pUp;
    RwV3d *src;
    RwV3d *dst;
    RwUInt8 *uSrc;
    RwUInt32 uStride;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwReal s;

    s = ptankGlobal->cSize.x * 0.5f;
    pRight.x = right->x * s;
    pRight.y = right->y * s;
    pRight.z = right->z * s;
    s = ptankGlobal->cSize.y * 0.5f;
    pUp.x = up->x * s;
    pUp.y = up->y * s;
    pUp.z = up->z * s;

    vectorSize[0].x = (- pRight.x - pUp.x);
    vectorSize[0].y = (- pRight.y - pUp.y);
    vectorSize[0].z = (- pRight.z - pUp.z);

    vectorSize[1].x = (+ pRight.x - pUp.x);
    vectorSize[1].y = (+ pRight.y - pUp.y);
    vectorSize[1].z = (+ pRight.z - pUp.z);

    vectorSize[2].x = (+ pRight.x + pUp.x);
    vectorSize[2].y = (+ pRight.y + pUp.y);
    vectorSize[2].z = (+ pRight.z + pUp.z);

    vectorSize[3].x = (- pRight.x + pUp.x);
    vectorSize[3].y = (- pRight.y + pUp.y);
    vectorSize[3].z = (- pRight.z + pUp.z);

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    for (i = 0; i < pCount; i++)
    {
        src = (RwV3d *)uSrc;

        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[0].x;
        dst->y = src->y + vectorSize[0].y;
        dst->z = src->z + vectorSize[0].z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[1].x;
        dst->y = src->y + vectorSize[1].y;
        dst->z = src->z + vectorSize[1].z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[2].x;
        dst->y = src->y + vectorSize[2].y;
        dst->z = src->z + vectorSize[2].z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + vectorSize[3].x;
        dst->y = src->y + vectorSize[3].y;
        dst->z = src->z + vectorSize[3].z;
        uDst += uDStride;

        uSrc += uStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsPosNCPSNR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwV3d pRight, pUp;
    RwInt32 i;
    RwV3d *src;
    RwV3d *dst;
    RwV2d *size;
    RwUInt8 *uSrc;
    RwUInt32 uStride;
    RwUInt8 *uSize;
    RwUInt32 uSStride;
    RwUInt8 *uDst;
    RwUInt32 uDStride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uSize = ptankGlobal->clusters[RPPTANKSIZESIZE].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZESIZE].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    for (i = 0; i < pCount; i++)
    {
        size = (RwV2d *)uSize;

        pRight.x = right->x * (size->x * 0.5f);
        pRight.y = right->y * (size->x * 0.5f);
        pRight.z = right->z * (size->x * 0.5f);

        pUp.x = up->x * (size->y * 0.5f);
        pUp.y = up->y * (size->y * 0.5f);
        pUp.z = up->z * (size->y * 0.5f);

        uSize += uSStride;

        src = (RwV3d *)uSrc;

        dst = (RwV3d *)uDst;
        dst->x = src->x + (- pRight.x - pUp.x);
        dst->y = src->y + (- pRight.y - pUp.y);
        dst->z = src->z + (- pRight.z - pUp.z);
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + (+ pRight.x - pUp.x);
        dst->y = src->y + (+ pRight.y - pUp.y);
        dst->z = src->z + (+ pRight.z - pUp.z);
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + (+ pRight.x + pUp.x);
        dst->y = src->y + (+ pRight.y + pUp.y);
        dst->z = src->z + (+ pRight.z + pUp.z);
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x + (- pRight.x + pUp.x);
        dst->y = src->y + (- pRight.y + pUp.y);
        dst->z = src->z + (- pRight.z + pUp.z);
        uDst += uDStride;

        uSrc += uStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsPosNCPM(RpPTankLockStruct *dstCluster, RwV3d *_right, RwV3d *_up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwV3d *dst;
    const RwV3d *pos;
    RwMatrix *mtx;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEMATRIX].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZEMATRIX].stride;

    for (i = 0; i < pCount; i++)
    {
        RwV3d right, up;
        RwV3d rightUp, rightDown;

        mtx = (RwMatrix *)uSrc;
        right.x = mtx->right.x * 0.5f;
        right.y = mtx->right.y * 0.5f;
        right.z = mtx->right.z * 0.5f;
        up.x = mtx->up.x * 0.5f;
        up.y = mtx->up.y * 0.5f;
        up.z = mtx->up.z * 0.5f;

        rightUp.x = right.x + up.x;
        rightUp.y = right.y + up.y;
        rightUp.z = right.z + up.z;
        rightDown.x = right.x - up.x;
        rightDown.y = right.y - up.y;
        rightDown.z = right.z - up.z;

        pos = &mtx->pos;

        uSrc += uSStride;

        /* vtx 0 */
        dst = (RwV3d *)uDst;
        dst->x = pos->x + rightDown.x;
        dst->y = pos->y + rightDown.y;
        dst->z = pos->z + rightDown.z;
        uDst += uDStride;

        /* vtx 3 */
        dst = (RwV3d *)uDst;
        dst->x = pos->x - rightUp.x;
        dst->y = pos->y - rightUp.y;
        dst->z = pos->z - rightUp.z;
        uDst += uDStride;

        /* vtx 2 */
        dst = (RwV3d *)uDst;
        dst->x = pos->x - rightDown.x;
        dst->y = pos->y - rightDown.y;
        dst->z = pos->z - rightDown.z;
        uDst += uDStride;

        /* vtx 1 */
        dst = (RwV3d *)uDst;
        dst->x = pos->x + rightUp.x;
        dst->y = pos->y + rightUp.y;
        dst->z = pos->z + rightUp.z;
        uDst += uDStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsColorPP(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwRGBA *src;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;
    RwUInt32 color;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZECOLOR].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZECOLOR].stride;

    for (i = 0; i < pCount; i++)
    {
        src = (RwRGBA *)uSrc;
        color = RGBATOCOLOR(src);
        uSrc += uSStride;

        *(RwUInt32 *)uDst = color;
        uDst += uDStride;
        *(RwUInt32 *)uDst = color;
        uDst += uDStride;
        *(RwUInt32 *)uDst = color;
        uDst += uDStride;
        *(RwUInt32 *)uDst = color;
        uDst += uDStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsUV2(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwTexCoords *dst;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;
    RwTexCoords *tex0, *tex1;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEVTX2TEXCOORDS].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZEVTX2TEXCOORDS].stride;

    for (i = 0; i < pCount; i++)
    {
        tex0 = &((RwTexCoords *)uSrc)[0];
        tex1 = &((RwTexCoords *)uSrc)[1];

        uSrc += uSStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex0->u;
        dst->v = tex1->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex1->u;
        dst->v = tex1->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex1->u;
        dst->v = tex0->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex0->u;
        dst->v = tex0->v;
        uDst += uDStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsUV4(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwTexCoords *dst;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;
    RwTexCoords *tex0, *tex1;
    RwTexCoords *tex2, *tex3;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEVTX4TEXCOORDS].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZEVTX4TEXCOORDS].stride;

    for (i = 0; i < pCount; i++)
    {
        tex0 = &((RwTexCoords *)uSrc)[0];
        tex1 = &((RwTexCoords *)uSrc)[1];
        tex2 = &((RwTexCoords *)uSrc)[2];
        tex3 = &((RwTexCoords *)uSrc)[3];

        uSrc += uSStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex0->u;
        dst->v = tex0->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex1->u;
        dst->v = tex1->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex2->u;
        dst->v = tex2->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex3->u;
        dst->v = tex3->v;
        uDst += uDStride;
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void
rpPTankGENInsUV2C(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwTexCoords *dst;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwTexCoords *tex0, *tex1;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    tex0 = &ptankGlobal->cUV[0];
    tex1 = &ptankGlobal->cUV[1];

    for (i = 0; i < pCount; i++)
    {
        dst = (RwTexCoords *)uDst;
        dst->u = tex0->u;
        dst->v = tex1->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex1->u;
        dst->v = tex1->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex1->u;
        dst->v = tex0->v;
        uDst += uDStride;

        dst = (RwTexCoords *)uDst;
        dst->u = tex0->u;
        dst->v = tex0->v;
        uDst += uDStride;
    }
}
