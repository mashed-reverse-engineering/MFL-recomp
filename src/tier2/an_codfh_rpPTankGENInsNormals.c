/* rpPTankGENInsNormals (MFL.exe 00548550). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RpPTankLockStruct RpPTankLockStruct;
typedef struct RpPTankData RpPTankData;
typedef struct RpPTankFormatDescriptor RpPTankFormatDescriptor;
struct  RpPTankLockStruct{
    RwUInt8 *data;            
    RwInt32 stride;            
};
#define RPPTANKSIZENORMAL           2
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
void rpPTankGENInsNormals(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwUInt8 *uSrc;
    RwUInt32 uSStride;
    RwV3d *src,*dst;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    uSrc = ptankGlobal->clusters[RPPTANKSIZENORMAL].data;
    uSStride = ptankGlobal->clusters[RPPTANKSIZENORMAL].stride;

    for(i=0;i<pCount;i++)
    {
        src = (RwV3d *)uSrc;

        dst = (RwV3d *)uDst;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
        uDst += uDStride;

        dst = (RwV3d *)uDst;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
        uDst += uDStride;

        uSrc += uSStride;
    }

    return;
}
