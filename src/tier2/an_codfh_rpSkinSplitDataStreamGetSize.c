/* rpSkinSplitDataStreamGetSize (MFL.exe 00540830). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/bsplit.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwMatrixWeights RwMatrixWeights;
typedef struct RpSkin RpSkin;
/* ---- end of the generated declarations ---- */

struct SkinPlatformData
{
    RwUInt32 d3d9Private[2];
};
typedef void * SkinUnaligned;
typedef struct SkinPlatformData         SkinPlatformData;
typedef struct SkinSplitData SkinSplitData;
struct SkinSplitData
{
    RwUInt32            boneLimit;
    RwUInt32            numMeshes;
    RwUInt32            numRLE;
    RwUInt8             *matrixRemapIndices;
    RwUInt8             *meshRLECount;
    RwUInt8             *meshRLE;
};
typedef struct SkinBoneData SkinBoneData;
struct SkinBoneData
{
    RwUInt32            numBones;
    RwUInt32            numUsedBones;
    RwUInt8             *usedBoneList;
    RwMatrix            *invBoneToSkinMat;
};
typedef struct SkinVertexMaps SkinVertexMaps;
struct SkinVertexMaps
{
    RwUInt32            maxWeights;
    RwUInt32            *matrixIndices;
    RwMatrixWeights     *matrixWeights;
};
struct RpSkin
{
    SkinBoneData        boneData;
    SkinVertexMaps      vertexMaps;
    SkinPlatformData    platformData;
    SkinSplitData       skinSplitData;
    SkinUnaligned       *unaligned;
};
#define SkinSplitDataSize(_sk,_gd) ((sizeof(RwUInt8) * (_sk)->boneData.numBones) + (sizeof(RwUInt8) * (_gd)->numMeshes * 2) + (sizeof(RwUInt8) * (_gd)->numRLE * 2))
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(RpSkin, skinSplitData) == 0x24);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(RpSkin, unaligned) == 0x3c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(RpSkin) == 0x40);      /* installed an_codfh_rpskin.c (rpSKINSIZE 0x40) */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 rpSkinSplitDataStreamGetSize(const RpSkin *skin)
{
    RwUInt32    streamSize;

    streamSize = sizeof(RwUInt32) * 3;

    if (0 < skin->skinSplitData.numMeshes)
    {
        
        streamSize += SkinSplitDataSize(skin, &skin->skinSplitData);
    }

    return (streamSize);
}
