/* _rpMeshRead (MFL.exe 0x00501140): RenderWare Graphics 3.7 (Criterion Software), reads a bin mesh (header, per mesh header and
 * indices) of a geometry or world sector from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamesh.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded, _rpMeshHeaderCreate (RwMalloc) and the object type / flag
 * macros written out (the exe inlines them), RPMESHGLOBAL expanded to the byte offset into *RwEngineInstance, the structs declared
 * here (codfh bamesh.h / bamesh.c) with compile-time layout asserts.  Verified by tools/diffemu (tier 2, chunk-stream spec
 * s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct binMeshHeader {
    RwUInt32 flags;
    RwUInt32 numMeshes;
    RwUInt32 totalIndicesInMesh;
} binMeshHeader;
typedef struct binMesh {
    RwUInt32 numIndices;
    RwInt32 matIndex;
} binMesh;
typedef struct RpMesh {
    RxVertexIndex *indices;
    RwUInt32 numIndices;
    void *material;                     /* RpMaterial * */
} RpMesh;
typedef struct RpMeshHeader {
    RwUInt32 flags;
    RwUInt16 numMeshes;
    RwUInt16 serialNum;
    RwUInt32 totalIndicesInMesh;
    RwUInt32 firstMeshOffset;
} RpMeshHeader;
typedef struct rpMeshObject {           /* the head of an RpGeometry / RpWorld: RwObject type, flags at +8 */
    RwUInt8 type;
    RwUInt8 subType;
    RwUInt8 objFlags;
    RwUInt8 privateFlags;
    void *parent;
    RwUInt32 flags;
} rpMeshObject;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpMeshGlobals {
    short nextSerialNum;
} rpMeshGlobals;
typedef struct RpMaterialList RpMaterialList;
RW_LAYOUT_ASSERT(rw_assert_binmeshheader, sizeof(binMeshHeader) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_binmesh, sizeof(binMesh) == 8);
RW_LAYOUT_ASSERT(rw_assert_mesh, sizeof(RpMesh) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_meshheader, sizeof(RpMeshHeader) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_meshheader_serial, RW_OFFSETOF(RpMeshHeader, serialNum) == 6);
RW_LAYOUT_ASSERT(rw_assert_meshobj_flags, RW_OFFSETOF(rpMeshObject, flags) == 8);

#define rpWORLD 7
#define rpGEOMETRY 8
#define rpGEOMETRYNATIVE 0x01000000
#define rpWORLDNATIVE 0x01000000
#define RWINDEXBUFFERSIZE 256
#define rwMEMHINTDUR_EVENT 0x30000
#define rwID_MESHMODULE 0x502

/*007d2128*/ extern void *RwEngineInstance;
/*00947778*/ extern RwModuleInfo meshModule;
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*00503e00*/ extern void *_rpMaterialListGetMaterial(const RpMaterialList *matList, RwInt32 matIndex);
#define RPMESHGLOBAL(var) (((rpMeshGlobals *)((RwUInt8 *)RwEngineInstance + meshModule.globalsOffset))->var)
#define rpMeshObjectHasIndices(object)                                                                          \
    (((rpGEOMETRY == ((const rpMeshObject *)(object))->type) && !(rpGEOMETRYNATIVE & ((const rpMeshObject *)(object))->flags)) || \
     ((rpWORLD == ((const rpMeshObject *)(object))->type) && !(rpWORLDNATIVE & ((const rpMeshObject *)(object))->flags)))

/* TIER2: validated 2026-11-10 authority=diffemu */
RpMeshHeader *_rpMeshRead(RwStream *stream, const void *object, const RpMaterialList *matList)
{
    binMeshHeader bmh;
    RpMeshHeader *meshHeader;
    RwUInt32 size;

    /* Read in a header */
    if (!RwStreamReadInt32(stream, (RwInt32 *)&bmh, sizeof(bmh)))
    {
        /* Failure */
        return NULL;
    }

    /* Figure out the size (little bit of slack for alignment) */
    size = (sizeof(RpMeshHeader) + ((sizeof(RpMesh) + 4) * bmh.numMeshes));

    if (rpMeshObjectHasIndices(object))
    {
        size += (sizeof(RxVertexIndex) * bmh.totalIndicesInMesh);
    }

    /* Allocate memory for the mesh (_rpMeshHeaderCreate) */
    meshHeader = (RpMeshHeader *)RwMalloc(size, rwMEMHINTDUR_EVENT | rwID_MESHMODULE);
    if (meshHeader)
    {
        RpMesh *mesh = (RpMesh *)(meshHeader + 1);
        RxVertexIndex *meshIndices = (RxVertexIndex *)(mesh + bmh.numMeshes);
        RwUInt32 numMeshes;

        meshHeader->flags = bmh.flags;
        meshHeader->numMeshes = (RwUInt16)bmh.numMeshes;
        meshHeader->serialNum = RPMESHGLOBAL(nextSerialNum);
        meshHeader->totalIndicesInMesh = (RwUInt32)bmh.totalIndicesInMesh;
        meshHeader->firstMeshOffset = 0;

        RPMESHGLOBAL(nextSerialNum)++;

        /* Then for each mesh... */
        numMeshes = meshHeader->numMeshes;
        while (numMeshes-- > 0)
        {
            binMesh bm;

            /* ... do another header... */
            if (NULL == RwStreamReadInt32(stream, (RwInt32 *)&bm, sizeof(bm)))
            {
                /* Failure */
                return NULL;
            }

            mesh->numIndices = bm.numIndices;
            mesh->material = _rpMaterialListGetMaterial(matList, bm.matIndex);
            mesh->indices = meshIndices;

            if (rpMeshObjectHasIndices(object))
            {
                RwUInt32 remainingIndices;

                /* ...and all the indices */
                remainingIndices = mesh->numIndices;
                while (remainingIndices > 0)
                {
                    RwInt32 IndexBuffer[RWINDEXBUFFERSIZE];      /* RpIndexBuffer */
                    RwInt32 *source = &IndexBuffer[0];
                    RwUInt32 readIndices;

                    readIndices = (remainingIndices < RWINDEXBUFFERSIZE) ? (remainingIndices) : (RWINDEXBUFFERSIZE);

                    if (!RwStreamReadInt32(stream, source, sizeof(RwInt32) * readIndices))
                    {
                        /* Failure */
                        return NULL;
                    }

                    remainingIndices -= readIndices;
                    while (readIndices-- > 0)
                    {
                        *meshIndices++ = (RxVertexIndex)(*source++);
                    }
                }
            }

            ++mesh;
        }
    }

    /* All done */
    return meshHeader;
}
