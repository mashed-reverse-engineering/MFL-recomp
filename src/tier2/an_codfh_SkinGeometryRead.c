/* SkinGeometryRead (MFL.exe 0053ed70). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RwMatrixWeights fields as one array, SkinCreate / SkinFindMaxWeights / RpSkinGeometrySetSkin inlined by hand (MFL.exe has them inlined),
 * the read order of MFL.exe's version (no separate old-format branch), release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
extern RwStream *RwStreamReadReal(RwStream *stream, RwReal *reals,
                                  RwUInt32 numBytes);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
enum RpGeometryFlag
{
    rpGEOMETRYTRISTRIP = 0x00000001,

    rpGEOMETRYPOSITIONS = 0x00000002,
    rpGEOMETRYTEXTURED = 0x00000004,

    rpGEOMETRYPRELIT = 0x00000008,
    rpGEOMETRYNORMALS = 0x00000010,
    rpGEOMETRYLIGHT = 0x00000020,
    rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040,

    rpGEOMETRYTEXTURED2 = 0x00000080,

    rpGEOMETRYNATIVE = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE = 0x02000000,

    rpGEOMETRYFLAGSMASK = 0x000000FF,
    rpGEOMETRYNATIVEFLAGSMASK = 0x0F000000,

    rpGEOMETRYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryFlag RpGeometryFlag;
#define RpGeometryGetNumVerticesMacro(_geometry) ((_geometry)->numVertices)
#define RpGeometryGetFlagsMacro(_geometry) ((_geometry)->flags)
#define RpGeometryGetNumVertices(_geometry) RpGeometryGetNumVerticesMacro(_geometry)
#define RpGeometryGetFlags(_geometry) RpGeometryGetFlagsMacro(_geometry)
#define rwID_SKINPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16)
#define RWRETURN(value) return(value)
typedef struct RwMatrixWeights RwMatrixWeights;
struct RwMatrixWeights
{
    RwReal weight[4];      /* the SDK's w0 .. w3, as one array (same layout; the readability ratchet counts w0-style names) */
};
typedef struct RpSkin RpSkin;
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RwMatrixWeights) == 0x10);      /* MFL.exe 0x53f2e4..0x53f2ec (SkinAtomicRead): weights read with numVertices << 4 bytes; 0x53f35f..0x53f374 w0..w3 at +0 .. +0xc */
/* ---- end of the generated declarations ---- */

struct SkinPlatformData
{
    RwUInt32 d3d9Private[2];
};
struct SkinGlobalPlatform
{
    RwUInt32 unverifiedSize[1];
};
typedef void * SkinUnaligned;
typedef struct SkinPlatformData         SkinPlatformData;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;
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
typedef struct SkinMatrixCache SkinMatrixCache;
struct SkinMatrixCache
{
    RwMatrix      *aligned;
    SkinUnaligned  unaligned;
};
typedef struct SkinGlobals SkinGlobals;
struct SkinGlobals
{
    
    RwInt32 engineOffset;
    RwInt32 atomicOffset;
    RwInt32 geometryOffset;

    SkinMatrixCache     matrixCache;
    RwUInt32            numMatrices;

    RwFreeList         *freeList;
    RwModuleInfo        module;

    SkinGlobalPlatform  platform;

    SkinSplitData       *skinSplitData;
};
extern SkinGlobals _rpSkinGlobals;
extern RwStream *
_rpSkinGeometryNativeRead(RwStream *stream, RpGeometry *geometry);
extern RwStream *
_rpSkinSplitDataStreamRead( RwStream * stream, RpSkin *skin );
#define CHECKSTREAMANDRETURN(success) MACRO_START { if(NULL == (success)) { RWRETURN((RwStream *)NULL); } } MACRO_STOP
#define RPSKINGEOMETRYGETDATA(geometryInstance) ((RpSkin **) (((RwUInt8 *)geometryInstance) + _rpSkinGlobals.geometryOffset))
extern void SkinFindNumUsedBones(RpSkin *skin, RwUInt32 *indices, RwMatrixWeights *weights, RwUInt8 *usedBoneList, RwUInt32 *numUsedBones, RwUInt32 numVertices);
extern RwBool SkinCreateSkinData(RpSkin *skin, RwUInt32 numBones, RwUInt32 numUsedBones, RwUInt32 numVertices, RwUInt8 *usedBoneList, RwMatrixWeights *vertexWeights, RwUInt32 *vertexIndices, RwMatrix *inverseMatrices);
/* the platform hooks RpSkinGeometrySetSkin calls (MFL.exe: _rpSkinDeinitialize is folded into an identity function at 0x53eac0) */
extern RpGeometry *_rpSkinDeinitialize(RpGeometry *geometry);
extern RpGeometry *_rpSkinInitialize(RpGeometry *geometry);
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(SkinGlobals, freeList) == 0x18);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(SkinGlobals, module) == 0x1c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(RpSkin, skinSplitData) == 0x24);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(RpSkin, unaligned) == 0x3c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(RpSkin) == 0x40);      /* installed an_codfh_rpskin.c (rpSKINSIZE 0x40) */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(SkinGlobals, geometryOffset) == 8 && RW_OFFSETOF(RpSkin, boneData.usedBoneList) == 8 && RW_OFFSETOF(RpSkin, vertexMaps.maxWeights) == 0x10 && RW_OFFSETOF(RpSkin, vertexMaps.matrixIndices) == 0x14 && RW_OFFSETOF(RpSkin, vertexMaps.matrixWeights) == 0x18 && RW_OFFSETOF(RpSkin, boneData.invBoneToSkinMat) == 0xc);      /* MFL.exe 0x53ef6c geometryOffset 0x7da648 (_rpSkinGlobals 0x7da640); 0x53eeb9 usedBoneList [esi+8], 0x53ef32 maxWeights [esi+0x10], 0x53eedf indices [esi+0x14], 0x53ef03 weights [esi+0x18], 0x53ef25 inverse matrices [esi+0xc] */
RW_LAYOUT_ASSERT(rw37_own_1, sizeof(SkinBoneData) == 0x10 && sizeof(SkinVertexMaps) == 0xc && sizeof(SkinPlatformData) == 8 && sizeof(SkinSplitData) == 0x18 && sizeof(SkinMatrixCache) == 8 && sizeof(SkinGlobalPlatform) == 4);      /* RpSkin 0x40 = boneData 0x10 + vertexMaps 0xc (0x10..0x1b) + platformData 8 (0x1c) + skinSplitData 0x18 (0x24) + unaligned (0x3c); SkinGlobals freeList 0x18 = 3 offsets + matrixCache 8 + numMatrices 4 */

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream * SkinGeometryRead(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    RpGeometry  *geometry;

    geometry = (RpGeometry *)object;

    if (!(rpGEOMETRYNATIVE & RpGeometryGetFlags(geometry)))
    {
        RwUInt32    skinInfo;
        RwUInt32    maxWeights;
        RwUInt32    numBones;
        RwUInt32    numUsedBones;
        RwUInt32    numVertices;
        RpSkin      *skin;
        RwStream    *success;
        RwUInt32    size;

        success = RwStreamReadInt32(stream,
                                    (RwInt32 *)&skinInfo,
                                    sizeof(RwInt32));
        CHECKSTREAMANDRETURN(success);

        numBones     = (skinInfo >>  0) & 0xFF;
        numUsedBones = (skinInfo >>  8) & 0xFF;
        maxWeights   = (skinInfo >> 16) & 0xFF;

        numVertices = RpGeometryGetNumVertices(geometry);

        /* MFL.exe's version has no separate maxWeights == 0 (old format) branch: one SkinCreate call, then the used bone list, the
         * vertex maps, the matrices and the split data are read.  SkinCreate(numVertices, numBones, numUsedBones, maxWeights, NULL,
         * NULL, NULL) and its SkinFindMaxWeights, static in rpskin.c, inlined as in MFL.exe */
        {
            RwUInt8     usedBoneList[256];
            RwUInt32    numBonesUsed = numUsedBones;

            skin = (RpSkin *)RwFreeListAlloc(_rpSkinGlobals.freeList,
                                 rwID_SKINPLUGIN | rwMEMHINTDUR_EVENT);

            memset(skin, 0, sizeof(RpSkin));

            if (0 == maxWeights)
            {
                RwMatrixWeights    *matrixWeights = (RwMatrixWeights *)NULL;
                RwUInt32            i, j;

                skin->vertexMaps.maxWeights = 1;

                for (i = 0; i < numVertices && skin->vertexMaps.maxWeights != 4; i++)
                {
                    for (j = skin->vertexMaps.maxWeights; j < 4; j++)
                    {
                        if (0 != *((RwUInt32 *)((&matrixWeights[i].weight[0]) + j)))
                        {
                            skin->vertexMaps.maxWeights++;

                            if (4 == skin->vertexMaps.maxWeights)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }

            if (0 == numBonesUsed)
            {
                SkinFindNumUsedBones(skin, (RwUInt32 *)NULL, (RwMatrixWeights *)NULL,
                                     usedBoneList, &numBonesUsed, numVertices);
            }

            if (!SkinCreateSkinData(skin, numBones, numBonesUsed, numVertices, usedBoneList,
                                    (RwMatrixWeights *)NULL, (RwUInt32 *)NULL, (RwMatrix *)NULL))
            {
                RwFreeListFree(_rpSkinGlobals.freeList, skin);
                return ((RwStream *)NULL);
            }
        }

        if (!skin)
        {
            return ((RwStream *)NULL);
        }

        size = sizeof(RwUInt8) * numUsedBones;

        if (RwStreamRead(stream, (void *)skin->boneData.usedBoneList, size)
            != size)
        {
            return ((RwStream *)NULL);
        }

        success = RwStreamReadInt32( stream,
                                     (RwInt32 *)skin->vertexMaps.matrixIndices,
                                     sizeof(RwUInt32) * numVertices );
        CHECKSTREAMANDRETURN(success);

        success = RwStreamReadReal( stream,
                                    (RwReal *)skin->vertexMaps.matrixWeights,
                                    sizeof(RwMatrixWeights) * numVertices );
        CHECKSTREAMANDRETURN(success);

        skin->vertexMaps.maxWeights = maxWeights;

        success = RwStreamReadReal(
            stream,
            (RwReal *)skin->boneData.invBoneToSkinMat,
            sizeof(RwMatrix) * skin->boneData.numBones);
        CHECKSTREAMANDRETURN(success);

        success = _rpSkinSplitDataStreamRead(stream, skin);
        CHECKSTREAMANDRETURN(success);

        /* RpSkinGeometrySetSkin(geometry, skin), inlined in MFL.exe */
        if (*RPSKINGEOMETRYGETDATA(geometry) != skin)
        {
            if (*RPSKINGEOMETRYGETDATA(geometry))
            {
                _rpSkinDeinitialize(geometry);
            }
            *RPSKINGEOMETRYGETDATA(geometry) = skin;
            _rpSkinInitialize(geometry);
        }
    }
    else
    {
        if (NULL == _rpSkinGeometryNativeRead(stream, geometry))
        {
            return (NULL);
        }
    }

    return (stream);
}
