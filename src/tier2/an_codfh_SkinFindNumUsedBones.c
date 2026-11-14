/* SkinFindNumUsedBones (MFL.exe 0053efe0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RwMatrixWeights fields as one array, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional
 * compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared
 * extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(RpSkin, skinSplitData) == 0x24);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(RpSkin, unaligned) == 0x3c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(RpSkin) == 0x40);      /* installed an_codfh_rpskin.c (rpSKINSIZE 0x40) */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void SkinFindNumUsedBones(RpSkin *skin, RwUInt32 *indices, RwMatrixWeights *weights, RwUInt8 *usedBoneList, RwUInt32 *numUsedBones, RwUInt32 numVertices)
{
    RwUInt32    i;

    *numUsedBones = 0;

    for (i = 0; i < numVertices; i++)
    {
        RwUInt32    j;

        for (j = 0; j < skin->vertexMaps.maxWeights; j++)
        {
            if (0 != *((RwUInt32 *)((&weights[i].weight[0]) + j)))
            {
                RwUInt32    k;
                RwUInt32    index;
                RwBool      newBone = TRUE;

                index = (indices[i] >> (8 * j)) & 0xFF;

                for (k = 0; k < *numUsedBones; k++)
                {
                    if (index == usedBoneList[k])
                    {
                        newBone = FALSE;
                        break;
                    }
                }

                if (newBone)
                {
                    usedBoneList[*numUsedBones] = index;
                    (*numUsedBones)++;
                }
            }
        }
    }

    return;
}
