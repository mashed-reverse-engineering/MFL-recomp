/* rpMeshSize (MFL.exe 00501310). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamesh.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RwObjectGetType(o) (((const RwObject *)(o))->type)
#define rpGEOMETRY 8
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
#define RpGeometryGetFlagsMacro(_geometry) ((_geometry)->flags)
#define RpGeometryGetFlags(_geometry) RpGeometryGetFlagsMacro(_geometry)
typedef struct RpVertexNormal RpVertexNormal;
struct RpWorldSector
{
    RwInt32 type;

    RpTriangle *triangles;

    RwV3d *vertices;
    RpVertexNormal *normals;

    RwTexCoords *texCoords[rwMAXTEXTURECOORDS];

    RwRGBA *preLitLum;

    RwResEntry *repEntry;

    RwLinkList collAtomicsInWorldSector;

    RwLinkList lightsInWorldSector;

    RwBBox boundingBox;

    RwBBox tightBoundingBox;

    RpMeshHeader *mesh;

    RxPipeline *pipeline;

    RwUInt16 matListWindowBase;

    RwUInt16 numVertices;
    RwUInt16 numTriangles;
    RwUInt16 pad;
};
struct RpSector
{
    RwInt32 type;
};
#define rpWORLD 7
enum RpWorldFlag
{
    rpWORLDTRISTRIP = 0x01,

    rpWORLDPOSITIONS = 0x02,
    rpWORLDTEXTURED = 0x04,
    rpWORLDPRELIT = 0x08,
    rpWORLDNORMALS = 0x10,
    rpWORLDLIGHT = 0x20,
    rpWORLDMODULATEMATERIALCOLOR = 0x40,

    rpWORLDTEXTURED2 = 0x80,

    rpWORLDNATIVE = 0x01000000,
    rpWORLDNATIVEINSTANCE = 0x02000000,

    rpWORLDFLAGSMASK = 0x000000FF,
    rpWORLDNATIVEFLAGSMASK = 0x0F000000,

    rpWORLDSECTORSOVERLAP = 0x40000000,

    rpWORLDFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldFlag RpWorldFlag;
#define RpWorldGetFlagsMacro(_world) ((_world)->flags)
#define RpWorldGetFlags(_world) RpWorldGetFlagsMacro(_world)
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
/* ---- end of the generated declarations ---- */

typedef struct binMeshHeader binMeshHeader;
struct binMeshHeader
{
    RwUInt32            flags;
    RwUInt32            numMeshes;
    RwUInt32            totalIndicesInMesh;
};
typedef struct binMesh binMesh;
struct binMesh
{
    RwUInt32            numIndices;
    RwInt32             matIndex;
};
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(binMeshHeader) == 0xc);      /* an_codfh_rpMeshRead.c */
RW_LAYOUT_ASSERT(rw37_priv_1, sizeof(binMesh) == 8);      /* an_codfh_rpMeshRead.c */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 rpMeshSize(const RpMeshHeader *meshHeader, const void *object)
{
    RwUInt32    size;

    if (((rpGEOMETRY == RwObjectGetType(object)) &&
         ((rpGEOMETRYNATIVE &
           RpGeometryGetFlags((const RpGeometry *)object)))) ||
        ((rpWORLD == RwObjectGetType(object)) &&
         ((rpWORLDNATIVE &
           RpWorldGetFlags((const RpWorld *)object)))))
    {
        
        size = (sizeof(binMeshHeader) +
               (meshHeader->numMeshes * sizeof(binMesh)));
    }
    else
    {
        
        size = (sizeof(binMeshHeader) +
               (meshHeader->numMeshes * sizeof(binMesh)) +
               (meshHeader->totalIndicesInMesh * sizeof(RwUInt32)));
    }

    return (size);
}
