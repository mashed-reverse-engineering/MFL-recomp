/* rpPTankGENInsPosCCPSNR (MFL.exe 00547a70). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RpPTankLockStruct RpPTankLockStruct;
typedef struct RpPTankData RpPTankData;
typedef struct RpPTankFormatDescriptor RpPTankFormatDescriptor;
struct  RpPTankLockStruct{
    RwUInt8 *data;
    RwInt32 stride;
};
#define RPPTANKSIZEPOSITION         0
#define RPPTANKSIZESIZE             3
struct RpPTankFormatDescriptor
{
    RwInt32 numClusters;
    RwInt32 stride;

    RwInt32 dataFlags;
};
struct RpPTankData
{
    void     *data;

    RpPTankLockStruct clusters[9];

    void *userData;

    RpPTankFormatDescriptor format;

    RwUInt32    srcBlend;
    RwUInt32    dstBlend;
    RwBool      vertexAlphaBlend;

    RwV2d           cCenter;
    RwV2d           cSize;
    RwReal          cRotate;
    RwRGBA          cColor;
    RwRGBA          cVtxColor[4];
    RwTexCoords     cUV[4];
    RwMatrix cMatrix;
};
/*007d2128*/ extern void *RwEngineInstance;

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void rpPTankGENInsPosCCPSNR(RpPTankLockStruct *dstCluster, RwV3d *right, RwV3d *up, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwV3d pRight,pUp;
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
    RwV3d   centerWS;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEPOSITION].data;
    uStride = ptankGlobal->clusters[RPPTANKSIZEPOSITION].stride;

    uSize = ptankGlobal->clusters[RPPTANKSIZESIZE].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZESIZE].stride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    centerWS.x = ptankGlobal->cCenter.x * right->x + ptankGlobal->cCenter.y * up->x;
    centerWS.y = ptankGlobal->cCenter.x * right->y + ptankGlobal->cCenter.y * up->y;
    centerWS.z = ptankGlobal->cCenter.x * right->z + ptankGlobal->cCenter.y * up->z;

    for (i = 0 ; i < pCount ; i++)
    {
        size = (RwV2d *)uSize;

        pRight.x = right->x * (size->x * 0.5f);
        pRight.y = right->y * (size->x * 0.5f);
        pRight.z = right->z * (size->x * 0.5f);

        pUp.x = up->x * (size->y * 0.5f);
        pUp.y = up->y * (size->y * 0.5f);
        pUp.z = up->z * (size->y * 0.5f);

        uSize += uSStride;

        src = (RwV3d*)uSrc;

        dst = (RwV3d*)uDst;
        dst->x = src->x + centerWS.x + (- pRight.x - pUp.x);
        dst->y = src->y + centerWS.y + (- pRight.y - pUp.y);
        dst->z = src->z + centerWS.z + (- pRight.z - pUp.z);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        dst->x = src->x + centerWS.x + (+ pRight.x - pUp.x);
        dst->y = src->y + centerWS.y + (+ pRight.y - pUp.y);
        dst->z = src->z + centerWS.z + (+ pRight.z - pUp.z);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        dst->x = src->x + centerWS.x + (+ pRight.x + pUp.x);
        dst->y = src->y + centerWS.y + (+ pRight.y + pUp.y);
        dst->z = src->z + centerWS.z + (+ pRight.z + pUp.z);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        dst->x = src->x + centerWS.x + (- pRight.x + pUp.x);
        dst->y = src->y + centerWS.y + (- pRight.y + pUp.y);
        dst->z = src->z + centerWS.z + (- pRight.z + pUp.z);
        uDst += uDStride;

        uSrc += uStride;
    }

    return;
}
