/* RpPTankAtomicCreate (MFL.exe 00545170). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * rpPTankValidateFlag inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
RpAtomic*
_rpPTankAtomicCreateCustom(RwInt32 maxParticleNum,
                RwUInt32 dataFlags, RwUInt32 platFlags,
                RpPTankCallBacks *callbacks);
/* ---- end of the generated declarations ---- */

extern RpPTankCallBacks defaultCB;
#define FAVORITEORGANISATIONMODE (rpPTANKDFLAGARRAY)
/* TIER2: unvalidated authority=diffemu */
RpAtomic* RpPTankAtomicCreate(RwInt32 maxParticleNum, RwUInt32 dataFlags, RwUInt32 platFlags)
{
    RpAtomic *atomic;
    /* rpPTankValidateFlag(dataFlags), inlined as in MFL.exe: dataFlags is made consistent in place */
    RwBool gotPosition;
    RwBool gotColor;
    RwBool gotSize;
    RwBool gotMatrix;
    RwBool gotNormal;
    RwBool got2DRotate;
    RwBool gotVtxColor;
    RwBool gotVtx2TexCoords;
    RwBool gotVtx4TexCoords;

    RwBool gotCMatrix;
    RwBool gotC2DRotate;
    RwBool gotCVtxColor;
    RwBool gotCVtx2TexCoords;
    RwBool gotCVtx4TexCoords;

    gotPosition = (dataFlags & rpPTANKDFLAGPOSITION ) == rpPTANKDFLAGPOSITION;
    gotColor = (dataFlags & rpPTANKDFLAGCOLOR ) == rpPTANKDFLAGCOLOR;
    gotSize = (dataFlags & rpPTANKDFLAGSIZE ) == rpPTANKDFLAGSIZE;
    gotMatrix = (dataFlags & rpPTANKDFLAGMATRIX ) == rpPTANKDFLAGMATRIX;
    gotNormal = (dataFlags & rpPTANKDFLAGNORMAL ) == rpPTANKDFLAGNORMAL;
    got2DRotate = (dataFlags & rpPTANKDFLAG2DROTATE ) == rpPTANKDFLAG2DROTATE;
    gotVtxColor = (dataFlags & rpPTANKDFLAGVTXCOLOR ) == rpPTANKDFLAGVTXCOLOR;

    gotVtx2TexCoords =
        (dataFlags & rpPTANKDFLAGVTX2TEXCOORDS ) == rpPTANKDFLAGVTX2TEXCOORDS;

    gotVtx4TexCoords =
        (dataFlags & rpPTANKDFLAGVTX4TEXCOORDS ) == rpPTANKDFLAGVTX4TEXCOORDS;

    gotCMatrix =
            (dataFlags & rpPTANKDFLAGCNSMATRIX ) == rpPTANKDFLAGCNSMATRIX;

    gotC2DRotate =
            (dataFlags & rpPTANKDFLAGCNS2DROTATE ) == rpPTANKDFLAGCNS2DROTATE;

    gotCVtxColor =
            (dataFlags & rpPTANKDFLAGCNSVTXCOLOR ) == rpPTANKDFLAGCNSVTXCOLOR;

    gotCVtx2TexCoords = (dataFlags & rpPTANKDFLAGCNSVTX2TEXCOORDS ) ==
                                                  rpPTANKDFLAGCNSVTX2TEXCOORDS;

    gotCVtx4TexCoords = (dataFlags & rpPTANKDFLAGCNSVTX4TEXCOORDS ) ==
                                                  rpPTANKDFLAGCNSVTX4TEXCOORDS;

    if(gotMatrix == TRUE && TRUE == gotCMatrix)
    {
        
        dataFlags &= rpPTANKDFLAGCNSMATRIX;
    }

    if(got2DRotate == TRUE && TRUE == gotC2DRotate)
    {
        
        dataFlags &= rpPTANKDFLAGCNS2DROTATE;
    }

    if(gotVtxColor == TRUE && TRUE == gotCVtxColor)
    {
        
        dataFlags &= rpPTANKDFLAGCNSVTXCOLOR;

    }

    if(gotVtx2TexCoords == TRUE && TRUE == gotCVtx2TexCoords)
    {
        
        dataFlags &= rpPTANKDFLAGCNSVTX2TEXCOORDS;

    }

    if(gotVtx4TexCoords == TRUE && TRUE == gotCVtx4TexCoords)
    {
        
        dataFlags &= rpPTANKDFLAGCNSVTX4TEXCOORDS;
    }

    if( TRUE == gotMatrix && TRUE == gotPosition )
    {
        
        dataFlags &= ~rpPTANKDFLAGPOSITION;

    }

    if( TRUE == gotMatrix && TRUE == gotSize )
    {
        
        dataFlags &= ~rpPTANKDFLAGSIZE;

    }

    if( TRUE == gotMatrix && TRUE == got2DRotate )
    {
        
        dataFlags &= ~rpPTANKDFLAG2DROTATE;

    }

    if( FALSE == gotMatrix && FALSE == gotPosition )
    {
        
        dataFlags |= rpPTANKDFLAGPOSITION;
    }

    if ( 0 == (dataFlags & ( rpPTANKDFLAGSTRUCTURE | rpPTANKDFLAGARRAY)) )
    {
        dataFlags |= FAVORITEORGANISATIONMODE;
    }

    atomic = _rpPTankAtomicCreateCustom(maxParticleNum,
                dataFlags, platFlags,
                &defaultCB);

    return (atomic);
}
