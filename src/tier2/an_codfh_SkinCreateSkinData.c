/* SkinCreateSkinData (MFL.exe 0053f0b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RwMatrixWeights fields as one array, ROUNDUP16 through mfl_uptr, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN
 * -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define RwMatrixCopyMacro(_target,_source) ( *(_target) = *(_source) )
#define RwMatrixCopy(dst,src) RwMatrixCopyMacro(dst, src)
#define rwID_SKINPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16)
typedef struct RwMatrixWeights RwMatrixWeights;
struct RwMatrixWeights
{
    RwReal weight[4];      /* the SDK's w0 .. w3, as one array (same layout; the readability ratchet counts w0-style names) */
};
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
#define ROUNDUP16(x) (((mfl_uptr)(x) + 16 - 1) & ~(mfl_uptr)(16 - 1))      /* mfl_uptr: pointer-wide (portability check); the same code on x86 */
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(RpSkin, skinSplitData) == 0x24);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(RpSkin, unaligned) == 0x3c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(RpSkin) == 0x40);      /* installed an_codfh_rpskin.c (rpSKINSIZE 0x40) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool SkinCreateSkinData(RpSkin *skin, RwUInt32 numBones, RwUInt32 numUsedBones, RwUInt32 numVertices, RwUInt8 *usedBoneList, RwMatrixWeights *vertexWeights, RwUInt32 *vertexIndices, RwMatrix *inverseMatrices)
{
    RwUInt32    size;

    size = ((sizeof(RwUInt32) + sizeof(RwMatrixWeights)) * numVertices) +
           (sizeof(RwMatrix) * numBones) +
           (sizeof(RwUInt8) * numUsedBones) + 15;

    skin->unaligned = (void **)RwMalloc(size,
        rwID_SKINPLUGIN | rwMEMHINTDUR_EVENT);
    if (!skin->unaligned)
    {
        return (FALSE);
    }

    memset(skin->unaligned, 0, size);

    skin->boneData.numBones = numBones;

    skin->boneData.numUsedBones = numUsedBones;

    skin->boneData.usedBoneList = (RwUInt8 *)skin->unaligned;

    skin->boneData.invBoneToSkinMat =
        (RwMatrix *)ROUNDUP16(skin->boneData.usedBoneList + numUsedBones);

    skin->vertexMaps.matrixIndices =
        (RwUInt32 *)(skin->boneData.invBoneToSkinMat + numBones);

    skin->vertexMaps.matrixWeights =
        (RwMatrixWeights *)(skin->vertexMaps.matrixIndices + numVertices);

    if((NULL != usedBoneList) && (0 != numUsedBones))
    {
        RwUInt32 size;

        size = sizeof(RwUInt8) * numUsedBones;

        memcpy(skin->boneData.usedBoneList, usedBoneList, size);
    }

    if(NULL != inverseMatrices)
    {
        RwUInt32 iBone;

        iBone = skin->boneData.numBones;
        while (iBone--)
        {
            RwMatrixCopy(&(skin->boneData.invBoneToSkinMat[iBone]),
                         &(inverseMatrices[iBone]));
        }
    }

    if(NULL != vertexIndices)
    {
        RwUInt32 size;

        size = sizeof(RwUInt32) * numVertices;

        memcpy(skin->vertexMaps.matrixIndices, vertexIndices, size);
    }

    if(NULL != vertexWeights)
    {
        RwUInt32 size;

        size = sizeof(RwMatrixWeights) * numVertices;

        memcpy(skin->vertexMaps.matrixWeights, vertexWeights, size);
    }

    return (TRUE);
}
