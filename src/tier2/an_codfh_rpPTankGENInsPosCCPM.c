/* rpPTankGENInsPosCCPM (MFL.exe 00547fd0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: generated, then hand-checked: function cut out of the file, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RpPTankLockStruct RpPTankLockStruct;
typedef struct RpPTankData RpPTankData;
typedef struct RpPTankFormatDescriptor RpPTankFormatDescriptor;
struct  RpPTankLockStruct{
    RwUInt8 *data;
    RwInt32 stride;
};
#define RPPTANKSIZEMATRIX           1
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
#define RwMatrixGetRight(m)    (&(m)->right)
#define RwMatrixGetUp(m)       (&(m)->up)
#define RwMatrixGetPos(m)      (&(m)->pos)
#define __RWUNUSED__
#define RwV3dAdd(o, a, b)               RwV3dAddMacro(o, a, b)
#define RwV3dSub(o, a, b)               RwV3dSubMacro(o, a, b)
/*007d2128*/ extern void *RwEngineInstance;

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void rpPTankGENInsPosCCPM(RpPTankLockStruct *dstCluster, RwV3d *_right __RWUNUSED__, RwV3d *_up __RWUNUSED__, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    const RwReal factx = ptankGlobal->cCenter.x;
    const RwReal facty = ptankGlobal->cCenter.y;
    RwInt32 i;
    RwV3d *dst;
    RwMatrix *mtx;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZEMATRIX].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZEMATRIX].stride;

    for (i = 0 ; i < pCount ; i++)
    {
        const RwV3d *right, *up, *pos;
        RwV3d rightUp, rightDown;
        RwV3d origin;

        mtx   = (RwMatrix *)uSrc;
        right = RwMatrixGetRight(mtx);
        up    = RwMatrixGetUp(mtx);
        pos   = RwMatrixGetPos(mtx);

        rightUp.x   = (right->x + up->x) * 0.5f;
        rightUp.y   = (right->y + up->y) * 0.5f;
        rightUp.z   = (right->z + up->z) * 0.5f;
        rightDown.x = (right->x - up->x) * 0.5f;
        rightDown.y = (right->y - up->y) * 0.5f;
        rightDown.z = (right->z - up->z) * 0.5f;

        origin.x = pos->x + (right->x * factx) + (up->x * facty);
        origin.y = pos->y + (right->y * factx) + (up->y * facty);
        origin.z = pos->z + (right->z * factx) + (up->z * facty);

        uSrc += uSStride;

        dst = (RwV3d*)uDst;
        RwV3dAdd(dst, &origin, &rightDown);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        RwV3dSub(dst, &origin, &rightUp);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        RwV3dSub(dst, &origin, &rightDown);
        uDst += uDStride;

        dst = (RwV3d*)uDst;
        RwV3dAdd(dst, &origin, &rightUp);
        uDst += uDStride;
    }

    return;
}
