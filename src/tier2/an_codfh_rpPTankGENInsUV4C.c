/* rpPTankGENInsUV4C (MFL.exe 005484e0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/ptank.c @e578672,
 * not byte-identical, changes: generated, then hand-checked: function cut out of the file, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RpPTankLockStruct RpPTankLockStruct;
typedef struct RpPTankData RpPTankData;
typedef struct RpPTankFormatDescriptor RpPTankFormatDescriptor;
struct  RpPTankLockStruct{
    RwUInt8 *data;            
    RwInt32 stride;            
};
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
void rpPTankGENInsUV4C(RpPTankLockStruct *dstCluster, RwInt32 pCount, RpPTankData *ptankGlobal)
{
    RwInt32 i;
    RwTexCoords *dst;
    RwUInt8 *uDst;
    RwUInt32 uDStride;
    RwTexCoords *tex0,*tex1;
    RwTexCoords *tex2,*tex3;

    uDst = dstCluster->data;
    uDStride = dstCluster->stride;

    tex0 = &ptankGlobal->cUV[0];
    tex1 = &ptankGlobal->cUV[1];
    tex2 = &ptankGlobal->cUV[2];
    tex3 = &ptankGlobal->cUV[3];

    for (i = 0 ; i < pCount ; i++)
    {
        
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

    return;
}
