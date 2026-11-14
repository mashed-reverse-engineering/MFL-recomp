/* rpMeshWrite (MFL.exe 00500ff0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamesh.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the mesh record and header record
 * in one local (the original's frame order), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional
 * compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared
 * extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RwObjectGetType(o) (((const RwObject *)(o))->type)
enum RwStreamType
{
    rwNASTREAM = 0,
    rwSTREAMFILE,
    rwSTREAMFILENAME,
    rwSTREAMMEMORY,
    rwSTREAMCUSTOM,
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,
    rwSTREAMREAD,
    rwSTREAMWRITE,
    rwSTREAMAPPEND,
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;
typedef struct RwStreamMemory RwStreamMemory;
struct RwStreamMemory
{
    RwUInt32 position;
    RwUInt32 nSize;
    RwUInt8 *memBlock;
};
typedef union RwStreamFile RwStreamFile;
union RwStreamFile
{
    void *fpFile;
    const void *constfpFile;
};
typedef RwBool(*rwCustomStreamFnClose) (void *data);
typedef RwUInt32(*rwCustomStreamFnRead) (void *data, void *buffer,
                                                     RwUInt32 length);
typedef RwBool(*rwCustomStreamFnWrite) (void *data,
                                                    const void *buffer,

                                                    RwUInt32 length);
typedef RwBool(*rwCustomStreamFnSkip) (void *data,

                                                   RwUInt32 offset);
typedef struct RwStreamCustom RwStreamCustom;
struct RwStreamCustom
{
    rwCustomStreamFnClose sfnclose;
    rwCustomStreamFnRead sfnread;
    rwCustomStreamFnWrite sfnwrite;
    rwCustomStreamFnSkip sfnskip;
    void *data;
};
typedef union RwStreamUnion RwStreamUnion;
union RwStreamUnion
{
    RwStreamMemory memory;
    RwStreamFile file;
    RwStreamCustom custom;
};
struct RwStream
{
    RwStreamType type;
    RwStreamAccessType accessType;
    RwInt32 position;
    RwStreamUnion Type;
    RwBool rwOwned;
};
extern RwStream *RwStreamWriteInt32(RwStream *stream, const RwInt32 *ints,
                                    RwUInt32 numBytes);
extern RwInt32 _rpMaterialListFindMaterialIndex(const RpMaterialList *matList,
                                               const RpMaterial *material);
typedef struct RpMesh RpMesh;
struct RpMesh
{
    RxVertexIndex *indices;
    RwUInt32 numIndices;
    RpMaterial *material;

};
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
RW_LAYOUT_ASSERT(rw37_row_10, sizeof(RpMesh) == 0xc);      /* an_codfh_rpMeshRead.c */
RW_LAYOUT_ASSERT(rw37_row_11, sizeof(RpWorldSector) == 0x88);      /* exe: exe static sectorTKList.sizeOfStruct at 0x0062ef88 */
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
#define RWINDEXBUFFERSIZE 256
typedef RwUInt32    RpIndexBuffer[RWINDEXBUFFERSIZE];
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(binMeshHeader) == 0xc);      /* an_codfh_rpMeshRead.c */
RW_LAYOUT_ASSERT(rw37_priv_1, sizeof(binMesh) == 8);      /* an_codfh_rpMeshRead.c */
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream * rpMeshWrite(const RpMeshHeader *meshHeader, const void *object, RwStream *stream, const RpMaterialList *matList)
{
    /* MFL.exe keeps the mesh record directly below the header record in its frame (diffemu compares the words behind the
     * buffer handed to RwStreamWriteInt32): one local holds both, in that order */
    struct { binMesh bm; binMeshHeader bmh; } rec;
    RwUInt32        numMeshes;
    const RpMesh    *mesh;
#define bmh rec.bmh
#define bm rec.bm

    bmh.flags = meshHeader->flags;
    bmh.numMeshes = (RwUInt32)meshHeader->numMeshes;
    bmh.totalIndicesInMesh = meshHeader->totalIndicesInMesh;

    if (!RwStreamWriteInt32(stream, (const RwInt32 *)&bmh, sizeof(bmh)))
    {
        
        return ((RwStream *)NULL);
    }

    mesh = (const RpMesh *)(meshHeader + 1);
    numMeshes = meshHeader->numMeshes;
    while (numMeshes--)
    {
        RwUInt32        numIndices;
        RxVertexIndex   *meshIndices;

        bm.numIndices = mesh->numIndices;
        bm.matIndex = _rpMaterialListFindMaterialIndex(matList, mesh->material);

        if (bm.matIndex < 0)
        {
            bm.matIndex = 0;
        }

        if (!RwStreamWriteInt32(stream, (const RwInt32 *)&bm, sizeof(bm)))
        {
            
            return ((RwStream *)NULL);
        }

        if (((rpGEOMETRY == RwObjectGetType(object)) &&
             (!(rpGEOMETRYNATIVE &
                RpGeometryGetFlags((const RpGeometry *)object)))) ||
            ((rpWORLD == RwObjectGetType(object)) &&
             (!(rpWORLDNATIVE &
                RpWorldGetFlags((const RpWorld *)object)))))
        {
            
            numIndices = mesh->numIndices;
            meshIndices = mesh->indices;

            while (numIndices > 0)
            {
                RpIndexBuffer   IndexBuffer;
                RwUInt32        writeIndices;
                RwUInt32        i;

                writeIndices = (numIndices < RWINDEXBUFFERSIZE) ?
                               (numIndices) : (RWINDEXBUFFERSIZE);

                for (i = 0; i < writeIndices; ++i)
                {
                    IndexBuffer[i] = (RwUInt32)*meshIndices++;
                }

                if (!RwStreamWriteInt32(stream,
                                        (const RwInt32 *)&IndexBuffer[0],
                                        sizeof(IndexBuffer[0]) * writeIndices))
                {
                    
                    return ((RwStream *)NULL);
                }

                numIndices -= writeIndices;
            }
        }

        ++mesh;
    }

    return (stream);
#undef bm
#undef bmh
}
