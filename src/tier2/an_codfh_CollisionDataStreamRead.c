/* CollisionDataStreamRead (MFL.exe 0054d8d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/colldata.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCorePluginID
{
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),

    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),

    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),

    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),

    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),

    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),

    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),

    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),

    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),

    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),

    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),

    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),

    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),

    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),

    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),

    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),

    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),

    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),

    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),

    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),

    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),

    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),

    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),

    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
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
#define RwStreamReadInt(_stream,_ints,_numBytes) RwStreamReadInt32(_stream, _ints, _numBytes)
extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type,
                                RwUInt32 *lengthOut, RwUInt32 *versionOut);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
#define rpGEOMETRY 8
#define RpMorphTargetGetBoundingSphereMacro(_mt) (&((_mt)->boundingSphere))
#define RpGeometryGetMorphTargetMacro(_geometry,_index) (&((_geometry)->morphTarget[(_index)]))
#define RpMorphTargetGetBoundingSphere(_geometry) RpMorphTargetGetBoundingSphereMacro(_geometry)
#define RpGeometryGetMorphTarget(_geometry,_index) RpGeometryGetMorphTargetMacro(_geometry, _index)
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
#define RpWorldSectorGetTightBBoxMacro(_sector) (&((_sector)->tightBoundingBox))
#define RpWorldSectorGetTightBBox(_sector) RpWorldSectorGetTightBBoxMacro(_sector)
typedef struct RpCollTree RpCollTree;
extern RpCollTree *
_rpCollTreeStreamRead(RwStream *stream);
typedef struct RpCollisionData RpCollisionData;
struct RpCollisionData
{
    RpCollTree *tree;
};
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
RW_LAYOUT_ASSERT(rw37_row_11, RW_OFFSETOF(RpCollisionData, tree) == 0);      /* MFL.exe 0x54da51 / 0x54da56 (RpCollisionGeometryBuildData): mov eax,[_rpCollisionGeometryDataOffset + geometry] = the old tree (the struct's only member) */
/* ---- end of the generated declarations ---- */

extern RpCollTree *
_rpCollTreeStreamReadPre36001(RwStream *stream, 
                              RwUInt32  numEntries, 
                              RwUInt32  numSplits,
                              RwBBox   *bbox,
                              RwBool   *failConv);
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream * CollisionDataStreamRead(RwStream *stream, RwInt32 __RWUNUSED__ binaryLength, void *object, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RwUInt32            version;
    RpCollisionData    *collData;

    collData = RWPLUGINOFFSET(RpCollisionData, object, offset);

    if (!RwStreamReadInt(stream, (RwInt32 *)&version, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    if (version < 0x36001)
    {
        RwUInt32  numTriangles, numSplits;
        RwBool    failConv;
        RwBBox    bbox;

        if (!RwStreamReadInt(stream,
                (RwInt32 *)&numTriangles, sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        if (RwObjectGetType(object) == rpGEOMETRY)
        {
            RpMorphTarget *mt;
            RwSphere      *sph;

            mt = RpGeometryGetMorphTarget((RpGeometry *)object, 0);
            sph = RpMorphTargetGetBoundingSphere(mt);
            bbox.inf.x = sph->center.x - sph->radius;
            bbox.inf.y = sph->center.y - sph->radius;
            bbox.inf.z = sph->center.z - sph->radius;
            bbox.sup.x = sph->center.x + sph->radius;
            bbox.sup.y = sph->center.y + sph->radius;
            bbox.sup.z = sph->center.z + sph->radius;
        }
        else
        {
            bbox = *RpWorldSectorGetTightBBox((RpWorldSector *)object);
        }

        numSplits = version - 1;
        failConv = FALSE;
        collData->tree = _rpCollTreeStreamReadPre36001(
            stream, numTriangles, numSplits, &bbox, &failConv);

        if (failConv)
        {

            return (stream);
        }
    }
    else
    {
        if (!RwStreamFindChunk(stream, rwID_COLLTREE, NULL, NULL))
        {
            return ((RwStream *)NULL);
        }

        collData->tree = _rpCollTreeStreamRead(stream);
    }

    return (collData->tree ? stream : (RwStream *)NULL);
}
