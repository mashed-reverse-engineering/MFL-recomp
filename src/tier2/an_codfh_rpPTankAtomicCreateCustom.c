/* rpPTankAtomicCreateCustom (MFL.exe 00545310). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * rpPTankSStructAlloc inlined by hand (MFL.exe has it inlined), rpPTankAtomicRenderCB declared extern (MFL.exe 0x545ca0), the alignment cast
 * through mfl_uptr, the datasize table static, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwMatrixType
{
    rwMATRIXTYPENORMAL = 0x00000001,
    rwMATRIXTYPEORTHOGONAL = 0x00000002,
    rwMATRIXTYPEORTHONORMAL = 0x00000003,
    rwMATRIXTYPEMASK = 0x00000003,
    rwMATRIXTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixType RwMatrixType;
enum RwMatrixFlag
{
    rwMATRIXINTERNALIDENTITY = 0x00020000,
    rwMATRIXFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixFlag RwMatrixFlag;
#define RwMatrixSetIdentityMacro(m) MACRO_START { (m)->right.x = (m)->up.y = (m)->at.z = (RwReal)((1.0)); (m)->right.y = (m)->right.z = (m)->up.x = (RwReal)((0.0)); (m)->up.z = (m)->at.x = (m)->at.y = (RwReal)((0.0)); (m)->pos.x = (m)->pos.y = (m)->pos.z = (RwReal)((0.0)); rwMatrixSetFlags((m), rwMatrixGetFlags(m) | (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL)); } MACRO_STOP
#define rwMatrixSetFlags(m,flagsbit) ((m)->flags = (flagsbit))
#define rwMatrixGetFlags(m) ((m)->flags)
#define RwMatrixSetIdentity(m) RwMatrixSetIdentityMacro(m)
#define RwV2dAssignMacro(_target,_source) ( *(_target) = *(_source) )
#define RwV2dAssign(o,a) RwV2dAssignMacro(o, a)
#define RwRenderStateGetMacro(_state,_value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateGet(_state, _value))
#define RwRenderStateGet(_state,_value) RwRenderStateGetMacro(_state, _value)
extern RpMaterial *RpMaterialSetTexture(RpMaterial *material, RwTexture *texture);
#define RpGeometryGetMaterialMacro(_geometry,_num) (((_geometry)->matList.materials)[(_num)])
#define RpGeometryGetMaterial(_geometry,_num) RpGeometryGetMaterialMacro(_geometry, _num)
#define RpAtomicGetRenderCallBackMacro(_atomic) ((_atomic)->renderCallBack)
#define RpAtomicGetRenderCallBack(_atomic) RpAtomicGetRenderCallBackMacro(_atomic)
extern RwBool
RpAtomicDestroy(RpAtomic * atomic);
extern RpAtomic *
RpAtomicCreate(void);
#define rwID_PTANKPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2f)
enum RpPTankInstanceFlags
{
    rpPTANKIFLAGNONE = ((int)0x00000000),
    rpPTANKIFLAGPOSITION = ((int)0x00000001),
    rpPTANKIFLAGCOLOR = ((int)0x00000002),
    rpPTANKIFLAGSIZE = ((int)0x00000004),
    rpPTANKIFLAGMATRIX = ((int)0x00000008),
    rpPTANKIFLAGNORMAL = ((int)0x00000010),
    rpPTANKIFLAG2DROTATE = ((int)0x00000020),
    rpPTANKIFLAGVTXCOLOR = ((int)0x00000040),
    rpPTANKIFLAGVTX2TEXCOORDS = ((int)0x00000080),
    rpPTANKIFLAGVTX4TEXCOORDS = ((int)0x00000100),

    rpPTANKIFLAGCNSCOLOR = ((int)0x00002000),
    rpPTANKIFLAGCNSSIZE = ((int)0x00004000),
    rpPTANKIFLAGCNSMATRIX = ((int)0x00008000),
    rpPTANKIFLAGCNSNORMAL = ((int)0x00010000),
    rpPTANKIFLAGCNS2DROTATE = ((int)0x00020000),
    rpPTANKIFLAGCNSVTXCOLOR = ((int)0x00040000),
    rpPTANKIFLAGCNSVTX2TEXCOORDS = ((int)0x00080000),
    rpPTANKIFLAGCNSVTX4TEXCOORDS = ((int)0x00100000),

    rpPTANKIFLAGACTNUMCHG = ((int)0x00800000),
    rpPTANKIFLAGCENTER = ((int)0x01000000),

    rpPTANKIFLAGALPHABLENDING = ((int)0x10000000),
    rpPTANKIFLAGALL = ((int)0xFFFFFFFF),

    RPPTANKINSTANCEFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpPTankInstanceFlags RpPTankInstanceFlags;
#define RPPTANKSIZEPOSITION 0
#define RPPTANKSIZEMATRIX 1
#define RPPTANKSIZENORMAL 2
#define RPPTANKSIZESIZE 3
#define RPPTANKSIZECOLOR 4
#define RPPTANKSIZEVTXCOLOR 5
#define RPPTANKSIZE2DROTATE 6
#define RPPTANKSIZEVTX2TEXCOORDS 7
#define RPPTANKSIZEVTX4TEXCOORDS 8
struct RpPTANKInstanceSetupData
{
    RwBool instancePositions;
    RwBool instanceUVs;
    RwBool instanceColors;
    RwBool instanceNormals;

    RpPTankLockStruct positionOut;
    RpPTankLockStruct UVOut;
    RpPTankLockStruct colorsOut;
    RpPTankLockStruct normalsOut;

    RwV3d right;
    RwV3d up;

};
#define RpPTankAtomicSetTextureMacro(atm_,tex_) MACRO_START{RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(atm_),0), tex_);}MACRO_STOP
#define RpPTankAtomicSetTexture(atm_,tex_) RpPTankAtomicSetTextureMacro(atm_,tex_)
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpPTANKInstanceSetupData) == 0x48);      /* an_codfh_ptank.c */
/* ---- end of the generated declarations ---- */

#define ALIGNPAD 16
static const RwInt32 datasize[] =      /* MFL.exe .rdata 0x5f5a48; the uses are folded into constants */
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
extern RwBool rpPTankAStructAlloc(RpPTankAtomicExtPrv *ptankGlobal, RwUInt32 dataFlags,RwUInt32 platFlags __RWUNUSED__);
extern RpAtomic * rpPTankAtomicRenderCB(RpAtomic *atomic);
/* TIER2: unvalidated authority=diffemu */
RpAtomic* rpPTankAtomicCreateCustom(RwInt32 maxParticleNum, RwUInt32 dataFlags, RwUInt32 platFlags, RpPTankCallBacks *callbacks)
{
    RwBool aResult = FALSE;
    RpAtomic *atomic;

    atomic = RpAtomicCreate();

    if(atomic)
    {
        RpPTankAtomicExtPrv *ptankGlobal = (RpPTankAtomicExtPrv *)
                                         RwMalloc(sizeof(RpPTankAtomicExtPrv),
                                           rwID_PTANKPLUGIN | rwMEMHINTDUR_EVENT);

        if( ptankGlobal )
        {

            ptankGlobal->defaultRenderCB = RpAtomicGetRenderCallBack(atomic);
            RpAtomicSetRenderCallBack(atomic, rpPTankAtomicRenderCB);
            ptankGlobal->maxPCount = maxParticleNum;
            ptankGlobal->actPCount = 0;

            ptankGlobal->publicData.format.dataFlags = dataFlags;

            ptankGlobal->platFlags = platFlags;
            ptankGlobal->instFlags = rpPTANKIFLAGALL;
            ptankGlobal->lockFlags = 0;

            ptankGlobal->rawdata = NULL;

            RPATOMICPTANKPLUGINDATA(atomic) = ptankGlobal;

            memcpy(&ptankGlobal->ptankCallBacks,
                                callbacks,
                                sizeof(RpPTankCallBacks));

            if( ptankGlobal->ptankCallBacks.alloc )
            {
                ptankGlobal->rawdata =
                    ptankGlobal->ptankCallBacks.alloc(&ptankGlobal->publicData,
                                                      ptankGlobal->maxPCount,
                                                      dataFlags, platFlags);
            }
            else if( rpPTANKDFLAGSTRUCTURE ==
                                      (dataFlags & rpPTANKDFLAGSTRUCTURE) )
            {
                /* rpPTankSStructAlloc(ptankGlobal, dataFlags, platFlags), inlined as in MFL.exe */
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

                if( TRUE == gotPosition )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGPOSITION;
                    tmpSize += datasize[RPPTANKSIZEPOSITION];
                }

                if( TRUE == gotMatrix )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGMATRIX;
                    tmpSize += datasize[RPPTANKSIZEMATRIX];
                }

                if( TRUE == gotNormal )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGNORMAL;
                    tmpSize += datasize[RPPTANKSIZENORMAL];
                }

                if( TRUE ==  gotSize )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGSIZE;
                    tmpSize += datasize[RPPTANKSIZESIZE];
                }

                if( TRUE == gotColor )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGCOLOR;
                    tmpSize += datasize[RPPTANKSIZECOLOR];
                }

                if( TRUE == gotVtxColor )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTXCOLOR;
                    tmpSize += datasize[RPPTANKSIZEVTXCOLOR];
                }

                if( TRUE == got2DRotate )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAG2DROTATE;
                    tmpSize += datasize[RPPTANKSIZE2DROTATE];
                }

                if( TRUE == gotVtx2TexCoords )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTX2TEXCOORDS;
                    tmpSize += datasize[RPPTANKSIZEVTX2TEXCOORDS];
                }

                if( TRUE == gotVtx4TexCoords )
                {
                    ptankGlobal->publicData.format.numClusters++;
                    ptankGlobal->publicData.format.dataFlags |= rpPTANKDFLAGVTX4TEXCOORDS;
                    tmpSize += datasize[RPPTANKSIZEVTX4TEXCOORDS];
                }

                if ( gotMatrix )
                {
                    tmpSize = (tmpSize + (rwMATRIXALIGNMENT - 1))
                                    & ~(rwMATRIXALIGNMENT - 1);
                }

                ptankGlobal->publicData.format.stride = tmpSize;

                size = (tmpSize*ptankGlobal->maxPCount) + ALIGNPAD;

                data = (RwUInt8 *)RwMalloc(size, rwMEMHINTDUR_EVENT);

                if( NULL != data )
                {
        
                    alignedData = (RwUInt8*)(((mfl_uptr)data
                                    + (ALIGNPAD - 1)) & ~(mfl_uptr)(ALIGNPAD - 1));
                    tmpSize = 0;

                    if( TRUE == gotPosition )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZEPOSITION].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZEPOSITION].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZEPOSITION];
                    }

                    if( TRUE == gotMatrix )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZEMATRIX].data
                                = alignedData + tmpSize;
                        ptankGlobal->publicData.clusters[RPPTANKSIZEMATRIX].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZEMATRIX];
                    }

                    if( TRUE == gotNormal )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZENORMAL].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZENORMAL].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZENORMAL];
                    }

                    if( TRUE ==  gotSize )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZESIZE].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZESIZE].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZESIZE];
                    }

                    if( TRUE == gotColor )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZECOLOR].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZECOLOR].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZECOLOR];
                    }

                    if( TRUE == gotVtxColor )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTXCOLOR].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTXCOLOR].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZEVTXCOLOR];
                    }

                    if( TRUE == got2DRotate )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZE2DROTATE].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZE2DROTATE].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize += datasize[RPPTANKSIZE2DROTATE];
                    }

                    if( TRUE == gotVtx2TexCoords )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTX2TEXCOORDS].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTX2TEXCOORDS].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize+=datasize[RPPTANKSIZEVTX2TEXCOORDS];
                    }

                    if( TRUE == gotVtx4TexCoords )
                    {
                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTX4TEXCOORDS].data
                                = alignedData + tmpSize;

                        ptankGlobal->publicData.clusters[RPPTANKSIZEVTX4TEXCOORDS].stride
                                = ptankGlobal->publicData.format.stride;

                        tmpSize+=datasize[RPPTANKSIZEVTX4TEXCOORDS];
                    }

                    ptankGlobal->rawdata = data;

                    aResult = TRUE;
                }
            }
            else
            {
                aResult = rpPTankAStructAlloc(ptankGlobal,
                                              dataFlags,platFlags);
            }

            if( (aResult == TRUE) || (NULL != ptankGlobal->rawdata) )
            {
                if( TRUE ==
                            ptankGlobal->ptankCallBacks.create(
                                        atomic, &ptankGlobal->publicData,
                                        ptankGlobal->maxPCount, dataFlags,
                                        platFlags))

                {
                    RwV2d center = { 0.0f, 0.0f };
                    RwV2d size = { 1.0f, 1.0f };
                    RwRGBA color = {0, 0, 0, 255};
                    RwTexCoords uv[4] = {{0.0f,0.0f},
                                    {0.0f,1.0f},
                                    {1.0f,1.0f},
                                    {1.0f,0.0f}};

                    RpPTankAtomicSetTexture(atomic, (RwTexture*)NULL);

                    RwV2dAssign(&(ptankGlobal->publicData.cSize), &size);

                    ptankGlobal->publicData.cRotate = 0.0f;

                    RwV2dAssign(&ptankGlobal->publicData.cCenter,&center);

                    RwMatrixSetIdentity(&ptankGlobal->publicData.cMatrix);

                    ptankGlobal->publicData.cColor = color;
                    ptankGlobal->publicData.cVtxColor[0] = color;
                    ptankGlobal->publicData.cVtxColor[1] = color;
                    ptankGlobal->publicData.cVtxColor[2] = color;
                    ptankGlobal->publicData.cVtxColor[3] = color;

                    ptankGlobal->publicData.cUV[0] = uv[0];
                    ptankGlobal->publicData.cUV[1] = uv[1];
                    ptankGlobal->publicData.cUV[2] = uv[2];
                    ptankGlobal->publicData.cUV[3] = uv[3];

                    RwRenderStateGet(rwRENDERSTATESRCBLEND,
                            (void *)&ptankGlobal->publicData.srcBlend);

                    RwRenderStateGet(rwRENDERSTATEDESTBLEND,
                            (void *)&ptankGlobal->publicData.dstBlend);

                    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE,
                            (void *)&ptankGlobal->publicData.vertexAlphaBlend);

                    return (atomic);
                }
            }
        }
    }

    RpAtomicDestroy(atomic);

    return (NULL);
}
