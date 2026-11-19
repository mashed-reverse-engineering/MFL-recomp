/* ptank GEN position instancer, constant center and size, per particle rotation (MFL.exe 0x00547800): RenderWare Graphics 3.7 (Criterion Software),
 * rpPTankGENInsPosCCCSPR (constant center, constant size, per particle 2D rotation).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), structs declared here (codfh ptank.h), SinCos inlined as the
 * statement macro SINCOS (angle through a local).  SinCos is the source's own inline-assembly helper (fsincos, both results stored as floats), as in
 * an_codfh_rpPTankGENInsPosNCPSPR.c: for |angle| >= 2^63 fsincos pushes nothing and the two fstp pop one entry too many, which
 * sin() / cos() in C cannot do, so the assembly stays for cl / x86 and the NOASM branch is the portable form.  Verified by tools/diffemu (tier 2). */
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
#define RPPTANKSIZE2DROTATE 6

#if !defined(_MSC_VER) || !defined(_M_IX86)
#define NOASM
extern double sin(double);
extern double cos(double);
#endif

/* SinCos as a statement macro: the exe has the source's __inline helper inlined into the loop, and cl reassociates this function's float sums
 * around it (vertex x: ((-pRight - pUp) + centerWS) + src, y: (... + src) + centerWS); with the helper called (tier 2 is /Ob0) the sums came out
 * in another order, 1 ulp off at 0x7f (attempt 1, agents/ncc_w11/notes.md).  angle must be a local variable (an __asm operand). */
#if defined(NOASM)
#define SINCOS(angle, sine, cosine) do { (sine) = (RwReal)sin(angle); (cosine) = (RwReal)cos(angle); } while (0)
#else
#define SINCOS(angle, sine, cosine) __asm fld dword ptr angle __asm fsincos __asm fstp dword ptr cosine __asm fstp dword ptr sine
#endif

/* TIER2: unvalidated 2026-11-18 authority=diffemu */
void rpPTankGENInsPosCCCSPR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    const RwReal sizeX = ptankGlobal->cSize.x * 0.5f;
    const RwReal sizeY = ptankGlobal->cSize.y * 0.5f;
    RwV3d pRight, pUp;
    RwInt32 i;
    RwReal angle;
    RwReal sinA;
    RwReal cosA;
    RwV3d *src;
    RwV3d *dst;
    RwUInt8 *uSrc;
    RwUInt32 uStride;
    RwUInt8 *uRotation;
    RwUInt32 uRStride;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwV3d centerWS;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uRotation = ptankGlobal->clusters[RPPTANKSIZE2DROTATE].data;
    uRStride = ptankGlobal->clusters[RPPTANKSIZE2DROTATE].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    for (i = 0; i < pCount; i++)
    {
        angle = *((RwReal *)uRotation);
        SINCOS(angle, sinA, cosA);
        uRotation += uRStride;

        /* Calculate world space size vectors */
        pRight.x = (+cosA) * right->x + (-sinA) * up->x;
        pRight.y = (+cosA) * right->y + (-sinA) * up->y;
        pRight.z = (+cosA) * right->z + (-sinA) * up->z;

        pUp.x = (+sinA) * right->x + (+cosA) * up->x;
        pUp.y = (+sinA) * right->y + (+cosA) * up->y;
        pUp.z = (+sinA) * right->z + (+cosA) * up->z;

        centerWS.x = ptankGlobal->cCenter.x * pRight.x + ptankGlobal->cCenter.y * pUp.x;
        centerWS.y = ptankGlobal->cCenter.x * pRight.y + ptankGlobal->cCenter.y * pUp.y;
        centerWS.z = ptankGlobal->cCenter.x * pRight.z + ptankGlobal->cCenter.y * pUp.z;

        pRight.x *= sizeX;
        pRight.y *= sizeX;
        pRight.z *= sizeX;

        pUp.x *= sizeY;
        pUp.y *= sizeY;
        pUp.z *= sizeY;

        src = (RwV3d *)uSrc;

        /* vtx 0 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + centerWS.x + (-pRight.x - pUp.x);
        dst->y = src->y + centerWS.y + (-pRight.y - pUp.y);
        dst->z = src->z + centerWS.z + (-pRight.z - pUp.z);
        uDst += uDStride;

        /* vtx 1 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + centerWS.x + (+pRight.x - pUp.x);
        dst->y = src->y + centerWS.y + (+pRight.y - pUp.y);
        dst->z = src->z + centerWS.z + (+pRight.z - pUp.z);
        uDst += uDStride;

        /* vtx 2 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + centerWS.x + (+pRight.x + pUp.x);
        dst->y = src->y + centerWS.y + (+pRight.y + pUp.y);
        dst->z = src->z + centerWS.z + (+pRight.z + pUp.z);
        uDst += uDStride;

        /* vtx 3 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + centerWS.x + (-pRight.x + pUp.x);
        dst->y = src->y + centerWS.y + (-pRight.y + pUp.y);
        dst->z = src->z + centerWS.z + (-pRight.z + pUp.z);
        uDst += uDStride;

        uSrc += uStride;
    }
}
