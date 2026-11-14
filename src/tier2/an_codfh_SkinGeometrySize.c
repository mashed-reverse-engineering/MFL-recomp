/* SkinGeometrySize (MFL.exe 0053ec30). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672
 *
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RwMatrixWeights fields as one array, the D3D9 native size is the shared return-0 body at 0x405e40, release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
#define RPSKINGEOMETRYGETCONSTDATA(geometryInstance) ((const RpSkin * const *) (((const RwUInt8 *)geometryInstance) + _rpSkinGlobals.geometryOffset))
/* MFL.exe: the D3D9 _rpSkinGeometryNativeSize is the shared `return 0` body at 0x405e40 (stub_return_zero); _rpSkinSplitDataStreamGetSize
 * is named rpSkinSplitDataStreamGetSize (0x540830) in recomp/symbols.txt */
extern RwInt32 stub_return_zero(const RpGeometry *geometry);
extern RwInt32 rpSkinSplitDataStreamGetSize(const RpSkin *skin);
#define _rpSkinGeometryNativeSize(geometry) stub_return_zero(geometry)
#define _rpSkinSplitDataStreamGetSize(skin) rpSkinSplitDataStreamGetSize(skin)
RW_LAYOUT_ASSERT(rw37_priv_0, RW_OFFSETOF(SkinGlobals, freeList) == 0x18);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(SkinGlobals, module) == 0x1c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_2, RW_OFFSETOF(RpSkin, skinSplitData) == 0x24);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_3, RW_OFFSETOF(RpSkin, unaligned) == 0x3c);      /* an_codfh_rpskin.c */
RW_LAYOUT_ASSERT(rw37_priv_5, sizeof(RpSkin) == 0x40);      /* installed an_codfh_rpskin.c (rpSKINSIZE 0x40) */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 SkinGeometrySize(const void *object, RwInt32 offset __RWUNUSED__, RwInt32 bytes __RWUNUSED__)
{
    const RpGeometry    *geometry;
    const RpSkin        *skin;
    RwInt32             size = 0;

    geometry = (const RpGeometry *)object;

    skin = *RPSKINGEOMETRYGETCONSTDATA(geometry);

    if (NULL != skin)
    {
        if (!(rpGEOMETRYNATIVE & RpGeometryGetFlags(geometry)))
        {
            RwUInt32    numVertices;

            numVertices = RpGeometryGetNumVertices(geometry);

            size = sizeof(RwUInt32);

            size += skin->boneData.numUsedBones * sizeof(RwUInt8);

            size += numVertices * sizeof(RwUInt32);

            size += numVertices * sizeof(RwMatrixWeights);

            size += skin->boneData.numBones * sizeof(RwMatrix);

            size += _rpSkinSplitDataStreamGetSize(skin);
        }
        else
        {
            size = _rpSkinGeometryNativeSize(geometry);
        }
    }

    return (size);
}
