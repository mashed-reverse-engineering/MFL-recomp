/* colldata plugin attach + stream writer (MFL.exe 0x0054d6c0, 0x0054d870, 0x0054dce0): RenderWare Graphics 3.7 (Criterion Software),
 * collision plugin: RpCollisionPluginAttach, CollisionDataStreamWrite and _rpCollTreeStreamWrite (both rw_trap rows, ported so the
 * attach does not reference int3 code).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/colldata.c @e578672
 * and src/rw/plugin/collis/ctdata.c @e578672, not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), CollisionWorldSectorPluginAttach /
 * CollisionGeometryPluginAttach and _rpCollTreeStreamGetSize inlined (as in the exe), RwStreamWriteChunkHeader written as the
 * versioned header call it expands to, RwMemRealToFloat32 / RwMemLittleEndian* dropped (no-ops on PC), RwStreamWriteInt16 =
 * RwStreamWriteInt32 (folded by the linker).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rwID_STRUCT 0x01
#define rwID_COLLTREE 0x2c
#define rwID_COLLISPLUGIN 0x11d
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwLIBRARYBUILDNUM 0x24
#define rwCHUNKHEADERSIZE 12
#define rpCOLLTREE_USEMAP 0x01

typedef short RwInt16;

typedef struct RpCollSector {
    RwUInt8 type;
    RwUInt8 contents;
    RwUInt16 index;
    RwReal value;
} RpCollSector;
typedef struct RpCollSplit {
    RpCollSector leftSector;
    RpCollSector rightSector;
} RpCollSplit;
typedef struct RpCollTree {
    RwUInt32 flags;
    RwBBox bbox;
    RwUInt16 numEntries;
    RwUInt16 numSplits;
    RpCollSplit *splits;
    RwUInt16 *map;
} RpCollTree;
typedef struct rpStreamCollTree {
    RwUInt32 flags;
    RwV3d inf;
    RwV3d sup;
    RwUInt32 numEntries;
    RwUInt32 numSplits;
} rpStreamCollTree;
typedef struct RpCollisionData {
    RpCollTree *tree;
} RpCollisionData;
RW_LAYOUT_ASSERT(rw_assert_colltree, RW_OFFSETOF(RpCollTree, numEntries) == 0x1c && RW_OFFSETOF(RpCollTree, splits) == 0x20 && RW_OFFSETOF(RpCollTree, map) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_collsplit, sizeof(RpCollSplit) == 0x10 && sizeof(rpStreamCollTree) == 0x24);

/* the overlay / link resolve names through recomp/symbols.txt + aliases.txt: these three have only Ghidra names there */
#define _rpCollisionAtomicRefOffset g_rp_7da6ec
#define _rpCollisionWorldSectorDataOffset g_worldforalltriintsecsphere_7da6f4
#define CollisionDataConstructor LAB_0054d840      /* identical-code folded with LODAtomicCallBackConstructor */
/*007da6e8*/ extern RwInt32 _rpCollisionGlobalRefOffset;
/*007da6ec*/ extern RwInt32 _rpCollisionAtomicRefOffset;
/*007da6f0*/ extern RwInt32 _rpCollisionGeometryDataOffset;
/*007da6f4*/ extern RwInt32 _rpCollisionWorldSectorDataOffset;
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *initCB, void *termCB);
/*004f7e80*/ extern RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*00500b40*/ extern RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*00500b70*/ extern RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004f9110*/ extern RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f9140*/ extern RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004d2f10*/ extern RwInt32 RwEngineGetVersion(void);
/*004dc6c0*/ extern RwStream *_rwStreamWriteVersionedChunkHeader(RwStream *stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum);
/*004dc8b0*/ extern RwStream *RwStreamWriteInt32(RwStream *stream, const void *ints, RwUInt32 numBytes);
/*004dbfc0*/ extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);
/*0054d7d0*/ extern void *CollisionOpen(void *instance, RwInt32 offset, RwInt32 size);
/*0054d800*/ extern void *CollisionClose(void *instance, RwInt32 offset, RwInt32 size);
/*0054da00*/ extern void *CollisionAtomicInit(void *object, RwInt32 offset, RwInt32 size);
/*0054d840*/ extern void *CollisionDataConstructor(void *object, RwInt32 offset, RwInt32 size);
/*0054d810*/ extern void *CollisionDataDestructor(void *object, RwInt32 offset, RwInt32 size);
/*0054d8d0*/ extern RwStream *CollisionDataStreamRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
/*0054d850*/ extern RwInt32 CollisionDataStreamGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwStream *CollisionDataStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
const RpCollTree *_rpCollTreeStreamWrite(const RpCollTree *tree, RwStream *stream);

#define RwStreamWriteChunkHeader(stream, type, size) _rwStreamWriteVersionedChunkHeader((stream), (type), (size), rwLIBRARYCURRENTVERSION, rwLIBRARYBUILDNUM)
#define RwStreamWriteInt16 RwStreamWriteInt32      /* 0x004dc8b0: the linker folded the identical int16 / int32 writers */

/* _rpCollTreeStreamGetSize (inlined in the exe): STRUCT header + rpStreamCollTree, the splits and the map */
#define COLLTREE_STREAM_SIZE(tree)                                                                              \
    (rwCHUNKHEADERSIZE + sizeof(rpStreamCollTree) + (tree)->numSplits * sizeof(RpCollSplit)                     \
     + ((rpCOLLTREE_USEMAP & (tree)->flags) ? (tree)->numEntries * sizeof(RwInt16) : 0))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
const RpCollTree *_rpCollTreeStreamWrite(const RpCollTree *tree, RwStream *stream)
{
    RwUInt32 size;
    rpStreamCollTree streamCollTree;

    /* Main chunk header */
    size = COLLTREE_STREAM_SIZE(tree);
    if (!RwStreamWriteChunkHeader(stream, rwID_COLLTREE, size))
    {
        return NULL;
    }

    /* Wrap structure data */
    size -= rwCHUNKHEADERSIZE;
    if (!RwStreamWriteChunkHeader(stream, rwID_STRUCT, size))
    {
        return NULL;
    }

    /* rpStreamCollTree */
    streamCollTree.inf = tree->bbox.inf;
    streamCollTree.sup = tree->bbox.sup;
    streamCollTree.flags = tree->flags;
    streamCollTree.numEntries = tree->numEntries;
    streamCollTree.numSplits = tree->numSplits;

    if (!RwStreamWrite(stream, &streamCollTree, sizeof(rpStreamCollTree)))
    {
        return NULL;
    }

    /* Splits */
    if (tree->numSplits > 0)
    {
        RwUInt32 i;

        for (i = 0; i < tree->numSplits; ++i)
        {
            RpCollSplit temp;

            temp = tree->splits[i];
            if (!RwStreamWrite(stream, &temp, sizeof(RpCollSplit)))
            {
                return NULL;
            }
        }
    }

    /* Map */
    if (rpCOLLTREE_USEMAP & tree->flags)
    {
        if (!RwStreamWriteInt16(stream, tree->map, tree->numEntries * sizeof(RwInt16)))
        {
            return NULL;
        }
    }
    return tree;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *CollisionDataStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpCollisionData *collData = (const RpCollisionData *)((const RwUInt8 *)object + offsetInObject);

    if (collData->tree)
    {
        RwUInt32 version = RwEngineGetVersion();

        if (!RwStreamWriteInt32(stream, &version, sizeof(RwInt32)))
        {
            return NULL;
        }

        if (!_rpCollTreeStreamWrite(collData->tree, stream))
        {
            return NULL;
        }
    }
    return stream;
}

/* TIER2: validated 2026-11-12 authority=diffemu */
RwBool RpCollisionPluginAttach(void)
{
    RwBool result;
    RwInt32 offset;

    /* Global instance data */
    _rpCollisionGlobalRefOffset = RwEngineRegisterPlugin(sizeof(RwUInt32), rwID_COLLISPLUGIN, (void *)CollisionOpen, (void *)CollisionClose);

    result = (_rpCollisionGlobalRefOffset > 0);

    /* Atomic plugin - for storing collision reference ID */
    if (result)
    {
        _rpCollisionAtomicRefOffset = RpAtomicRegisterPlugin(sizeof(RwInt32), rwID_COLLISPLUGIN, (void *)CollisionAtomicInit, NULL, NULL);

        result = (_rpCollisionAtomicRefOffset > 0);
        if (result)
        {
            /* CollisionWorldSectorPluginAttach() */
            _rpCollisionWorldSectorDataOffset = RpWorldSectorRegisterPlugin(sizeof(void *), rwID_COLLISPLUGIN, (void *)CollisionDataConstructor,
                                                                            (void *)CollisionDataDestructor, NULL);
            result = (_rpCollisionWorldSectorDataOffset >= 0);
            if (result)
            {
                offset = RpWorldSectorRegisterPluginStream(rwID_COLLISPLUGIN, (void *)CollisionDataStreamRead, (void *)CollisionDataStreamWrite,
                                                           (void *)CollisionDataStreamGetSize);
                result = (offset >= 0);
            }
        }
        if (result)
        {
            /* CollisionGeometryPluginAttach() */
            _rpCollisionGeometryDataOffset = RpGeometryRegisterPlugin(sizeof(void *), rwID_COLLISPLUGIN, (void *)CollisionDataConstructor,
                                                                      (void *)CollisionDataDestructor, NULL);
            result = (_rpCollisionGeometryDataOffset >= 0);
            if (result)
            {
                offset = RpGeometryRegisterPluginStream(rwID_COLLISPLUGIN, (void *)CollisionDataStreamRead, (void *)CollisionDataStreamWrite,
                                                        (void *)CollisionDataStreamGetSize);
                result = (offset >= 0);
            }
        }
    }
    return result;
}
