/* ptank GEN position instancer, constant size, per particle rotation (MFL.exe 0x005472a0): RenderWare Graphics 3.7 (Criterion Software),
 * rpPTankGENInsPosNCCSPR (no center, constant size, per particle 2D rotation; the air strike missile smoke trail, particle feature word 0x895).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), structs declared here (codfh ptank.h).
 * SinCos is the source's own inline-assembly helper (fsincos, both results stored as floats through the pointers), as in
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
RW_LAYOUT_ASSERT(rw_assert_ptank_csize, RW_OFFSETOF(RpPTankData, cSize) == 0x70);

#define RPPTANKSIZEPOSITION 0
#define RPPTANKSIZE2DROTATE 6

#if !defined(_MSC_VER) || !defined(_M_IX86)
#define NOASM
extern double sin(double);
extern double cos(double);
#endif

#define SinCos rpPTankSinCos_r      /* _r: a helper of this file (tier 2 is built /Ob0, so it is called, not inlined) */
static void rpPTankSinCos_r(float angle, float *sine, float *cosine)
{
#if defined(NOASM)
    *sine = (RwReal)sin(angle);
    *cosine = (RwReal)cos(angle);
#else
    __asm
    {
        fld        dword ptr angle
        fsincos
        mov        eax, sine
        mov        edx, cosine
        fstp    dword ptr [edx]
        fstp    dword ptr [eax]
    }
#endif
}

/* TIER2: unvalidated authority=diffemu */
void rpPTankGENInsPosNCCSPR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    const RwReal sizeX = ptankGlobal->cSize.x * 0.5f;
    const RwReal sizeY = ptankGlobal->cSize.y * 0.5f;
    RwV3d pRight, pUp;
    RwInt32 i;
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

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uRotation = ptankGlobal->clusters[RPPTANKSIZE2DROTATE].data;
    uRStride = ptankGlobal->clusters[RPPTANKSIZE2DROTATE].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    for (i = 0; i < pCount; i++)
    {
        SinCos(*((RwReal *)uRotation), &sinA, &cosA);

        uRotation += uRStride;

        /* Calculate world space size vectors */
        pRight.x = (+cosA) * right->x + (-sinA) * up->x;
        pRight.y = (+cosA) * right->y + (-sinA) * up->y;
        pRight.z = (+cosA) * right->z + (-sinA) * up->z;

        pUp.x = (+sinA) * right->x + (+cosA) * up->x;
        pUp.y = (+sinA) * right->y + (+cosA) * up->y;
        pUp.z = (+sinA) * right->z + (+cosA) * up->z;

        pRight.x *= sizeX;
        pRight.y *= sizeX;
        pRight.z *= sizeX;

        pUp.x *= sizeY;
        pUp.y *= sizeY;
        pUp.z *= sizeY;

        src = (RwV3d *)uSrc;

        /* vtx 0 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + (-pRight.x - pUp.x);
        dst->y = src->y + (-pRight.y - pUp.y);
        dst->z = src->z + (-pRight.z - pUp.z);
        uDst += uDStride;

        /* vtx 1 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + (+pRight.x - pUp.x);
        dst->y = src->y + (+pRight.y - pUp.y);
        dst->z = src->z + (+pRight.z - pUp.z);
        uDst += uDStride;

        /* vtx 2 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + (+pRight.x + pUp.x);
        dst->y = src->y + (+pRight.y + pUp.y);
        dst->z = src->z + (+pRight.z + pUp.z);
        uDst += uDStride;

        /* vtx 3 */
        dst = (RwV3d *)uDst;
        dst->x = src->x + (-pRight.x + pUp.x);
        dst->y = src->y + (-pRight.y + pUp.y);
        dst->z = src->z + (-pRight.z + pUp.z);
        uDst += uDStride;

        uSrc += uStride;
    }
}
