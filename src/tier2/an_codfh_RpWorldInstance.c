/* RpWorldInstance (MFL.exe 004f5d10). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpWorldForAllWorldSectors and the WorldSectorInstance callback inlined as the BSP stack walk MFL.exe has, release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwBool RwResourcesFreeResEntry(RwResEntry * entry);
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
typedef struct RpPlaneSector RpPlaneSector;
struct RpPlaneSector
{
    RwInt32 type;

    RwReal value;
    RpSector *leftSubTree;
    RpSector *rightSubTree;
    RwReal leftValue;
    RwReal rightValue;
};
extern RpWorldSector * RpWorldSectorRender(RpWorldSector *worldSector);
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
#define RpWorldSetFlagsMacro(_world,_flags) (((_world)->flags = (_flags)), (_world))
#define RpWorldGetFlagsMacro(_world) ((_world)->flags)
#define RpWorldSetFlags(_world,_flags) RpWorldSetFlagsMacro(_world, _flags)
#define RpWorldGetFlags(_world) RpWorldGetFlagsMacro(_world)
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RpWorldSector, matListWindowBase) == 0x80);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpWorldSector, numTriangles) == 0x84);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RpWorldSector, triangles) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(RpWorldSector, vertices) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_row_4, sizeof(RpPlaneSector) == 0x18);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_5, RW_OFFSETOF(RpPlaneSector, leftSubTree) == 8);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_6, RW_OFFSETOF(RpPlaneSector, leftValue) == 0x10);      /* an_codfh_PlaneSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_7, RW_OFFSETOF(RpWorldSector, texCoords) == 0x10);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_8, RW_OFFSETOF(RpWorldSector, preLitLum) == 0x30);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_9, RW_OFFSETOF(RpWorldSector, collAtomicsInWorldSector) == 0x38);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_10, RW_OFFSETOF(RpWorldSector, tightBoundingBox) == 0x60);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpWorldSector, mesh) == 0x78);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_12, RW_OFFSETOF(RpPlaneSector, rightSubTree) == 0xc);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_13, RW_OFFSETOF(RpWorldSector, boundingBox) == 0x48);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_row_14, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
/* ---- end of the generated declarations ---- */

#define _rpWorldSectorNativeFreeResEntry(_sctr) ((_sctr->repEntry)?(RwResourcesFreeResEntry(_sctr->repEntry)):(TRUE))
/* MFL.exe inlines RpWorldForAllWorldSectors and the WorldSectorInstance callback (BSP walk with a 64 entry stack in the frame):
 * the walk is written out here in the same form */
#define rpWORLDINSTANCESTACK 64
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool RpWorldInstance(RpWorld *world)
{
    RpSector *stack[rpWORLDINSTANCESTACK];
    RpSector *sector;
    RwInt32 top;

    if( rpWORLDNATIVE & RpWorldGetFlags(world) )
    {

        return (TRUE);
    }

    RpWorldSetFlags(world, RpWorldGetFlags(world) | rpWORLDNATIVEINSTANCE);

    sector = world->rootSector;
    top = 0;
    while (top >= 0)
    {
        if (sector->type < 0)
        {
            RpWorldSector *worldSector = (RpWorldSector *)sector;

            _rpWorldSectorNativeFreeResEntry(worldSector);
            RpWorldSectorRender(worldSector);
            sector = stack[top];
            top--;
        }
        else
        {
            RpPlaneSector *planeSector = (RpPlaneSector *)sector;

            sector = planeSector->leftSubTree;
            top++;
            stack[top] = planeSector->rightSubTree;
        }
    }

    RpWorldSetFlags(world, (~rpWORLDNATIVEINSTANCE & RpWorldGetFlags(world)) |
                           rpWORLDNATIVE);

    return (TRUE);
}
