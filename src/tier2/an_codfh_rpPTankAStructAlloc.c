/* rpPTankAStructAlloc (MFL.exe 00545940). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). Hand changes: the pointer is aligned through mfl_uptr (was RwUInt32), datasize is static. */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwID_PTANKPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2f)
#define RPPTANKSIZEPOSITION 0
#define RPPTANKSIZEMATRIX 1
#define RPPTANKSIZENORMAL 2
#define RPPTANKSIZESIZE 3
#define RPPTANKSIZECOLOR 4
#define RPPTANKSIZEVTXCOLOR 5
#define RPPTANKSIZE2DROTATE 6
#define RPPTANKSIZEVTX2TEXCOORDS 7
#define RPPTANKSIZEVTX4TEXCOORDS 8
/* ---- end of the generated declarations ---- */

#define ALIGNPAD 16
/* MFL.exe has this table at 0x005f5a48; the function's own uses are constant folded there as here (lea / shl, no load), so the definition
   stays, static: another file carrying the table cannot clash with it */
static const RwInt32 datasize[] =
{
    sizeof(RwV3d),
    sizeof(RwMatrix),
    sizeof(RwV3d),
    sizeof(RwV2d),
    sizeof(RwRGBA),
    4*sizeof(RwRGBA),
    sizeof(RwReal),
    2*sizeof(RwTexCoords),
    4*sizeof(RwTexCoords)
};
/* TIER2: unvalidated authority=diffemu */
RwBool rpPTankAStructAlloc(RpPTankAtomicExtPrv *ptankGlobal, RwUInt32 dataFlags,RwUInt32 platFlags __RWUNUSED__)
{
    RwBool result = FALSE;

    RwBool gotPosition;
    RwBool gotColor;
    RwBool gotSize;
    RwBool gotMatrix;
    RwBool gotNormal;
    RwBool got2DRotate;
    RwBool gotVtxColor;
    RwBool gotVtx2TexCoords;
    RwBool gotVtx4TexCoords;

    RwUInt32 size;
    RwUInt32 tmpSize;
    RwUInt8 *data;
    RwUInt8 *alignedData;

    gotPosition = (dataFlags & rpPTANKDFLAGPOSITION ) == rpPTANKDFLAGPOSITION;
    gotColor = (dataFlags & rpPTANKDFLAGCOLOR ) == rpPTANKDFLAGCOLOR;
    gotSize = (dataFlags & rpPTANKDFLAGSIZE ) == rpPTANKDFLAGSIZE;
    gotMatrix = (dataFlags & rpPTANKDFLAGMATRIX ) == rpPTANKDFLAGMATRIX;
    gotNormal = (dataFlags & rpPTANKDFLAGNORMAL ) == rpPTANKDFLAGNORMAL;
    got2DRotate = (dataFlags & rpPTANKDFLAG2DROTATE ) == rpPTANKDFLAG2DROTATE;
    gotVtxColor = (dataFlags & rpPTANKDFLAGVTXCOLOR ) == rpPTANKDFLAGVTXCOLOR;
    gotVtx2TexCoords = (dataFlags & rpPTANKDFLAGVTX2TEXCOORDS ) ==
                                                     rpPTANKDFLAGVTX2TEXCOORDS;
    gotVtx4TexCoords = (dataFlags & rpPTANKDFLAGVTX4TEXCOORDS ) ==
                                                     rpPTANKDFLAGVTX4TEXCOORDS;

    tmpSize = 0;
    ptankGlobal->publicData.format.dataFlags &= 0xFFFFFE00;
    ptankGlobal->publicData.format.numClusters = 0;
    ptankGlobal->lockFlags = 0;

    if( TRUE == gotPosition )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGPOSITION;
        tmpSize += datasize[RPPTANKSIZEPOSITION]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotMatrix )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGMATRIX;
        tmpSize += datasize[RPPTANKSIZEMATRIX]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotNormal )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGNORMAL;
        tmpSize += datasize[RPPTANKSIZENORMAL]*ptankGlobal->maxPCount;
    }

    if( TRUE ==  gotSize )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGSIZE;
        tmpSize += datasize[RPPTANKSIZESIZE]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotColor )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGCOLOR;
        tmpSize += datasize[RPPTANKSIZECOLOR]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotVtxColor )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTXCOLOR;
        tmpSize += datasize[RPPTANKSIZEVTXCOLOR]*ptankGlobal->maxPCount;
    }

    if( TRUE == got2DRotate )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAG2DROTATE;
        tmpSize += datasize[RPPTANKSIZE2DROTATE]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotVtx2TexCoords )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTX2TEXCOORDS;
        tmpSize += datasize[RPPTANKSIZEVTX2TEXCOORDS]*ptankGlobal->maxPCount;
    }

    if( TRUE == gotVtx4TexCoords )
    {
        ptankGlobal->publicData.format.numClusters++;
        ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTX4TEXCOORDS;
        tmpSize += datasize[RPPTANKSIZEVTX4TEXCOORDS]*ptankGlobal->maxPCount;
    }

    size = tmpSize + ALIGNPAD;

    data = (RwUInt8 *)RwMalloc(size,
        rwID_PTANKPLUGIN | rwMEMHINTDUR_EVENT);

    if( NULL != data )
    {

        alignedData = (RwUInt8*)(((mfl_uptr)data
                        + (ALIGNPAD - 1)) & ~(ALIGNPAD - 1));
        tmpSize = 0;

        if( TRUE == gotPosition )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZEPOSITION].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZEPOSITION].stride
                    = sizeof(RwV3d);

            tmpSize += datasize[RPPTANKSIZEPOSITION]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotMatrix )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZEMATRIX].data
                    = alignedData + tmpSize;
            ptankGlobal->publicData.clusters[RPPTANKSIZEMATRIX].stride
                    = sizeof(RwMatrix);

            tmpSize += datasize[RPPTANKSIZEMATRIX]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotNormal )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZENORMAL].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZENORMAL].stride
                    = sizeof(RwV3d);

            tmpSize += datasize[RPPTANKSIZENORMAL]*ptankGlobal->maxPCount;
        }

        if( TRUE ==  gotSize )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZESIZE].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZESIZE].stride
                    = sizeof(RwV2d);

            tmpSize += datasize[RPPTANKSIZESIZE]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotColor )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZECOLOR].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZECOLOR].stride
                    = sizeof(RwRGBA);

            tmpSize += datasize[RPPTANKSIZECOLOR]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotVtxColor )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZEVTXCOLOR].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZEVTXCOLOR].stride
                    = 4*sizeof(RwRGBA);

            tmpSize += datasize[RPPTANKSIZEVTXCOLOR]*ptankGlobal->maxPCount;
        }

        if( TRUE == got2DRotate )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZE2DROTATE].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZE2DROTATE].stride
                    = sizeof(RwReal);

            tmpSize += datasize[RPPTANKSIZE2DROTATE]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotVtx2TexCoords )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZEVTX2TEXCOORDS].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZEVTX2TEXCOORDS].stride
                    = 2*sizeof(RwTexCoords);

            tmpSize+=datasize[RPPTANKSIZEVTX2TEXCOORDS]*ptankGlobal->maxPCount;
        }

        if( TRUE == gotVtx4TexCoords )
        {
            ptankGlobal->publicData.clusters[RPPTANKSIZEVTX4TEXCOORDS].data
                    = alignedData + tmpSize;

            ptankGlobal->publicData.clusters[RPPTANKSIZEVTX4TEXCOORDS].stride
                    = 4*sizeof(RwTexCoords);

            tmpSize+=datasize[RPPTANKSIZEVTX4TEXCOORDS]*ptankGlobal->maxPCount;
        }

        ptankGlobal->publicData.format.stride = 0;
        ptankGlobal->rawdata = data;

        result = TRUE;
    }

    return (result);
}
