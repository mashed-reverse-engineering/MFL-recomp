/* _rpCollTreeStreamRead (MFL.exe 0x0054de40): RenderWare Graphics 3.7 (Criterion Software), reads a collision tree (struct chunk:
 * header, splits, optional entry map) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/ctdata.c @e578672,
 * not byte-identical, changes: PC build (RwMemNative16 / RwMemNative32 / RwMemFloat32ToReal are no-ops and gone, so is the split
 * conversion loop; RwStreamReadInt16 is the exe's RwStreamReadInt32, the linker folded the identical functions), RWFUNCTION /
 * RWASSERT / RWRETURN expanded, _rpCollTreeDestroy written out (RwFree, inlined by the exe), the counts passed to _rpCollTreeCreate
 * as the stream's whole words (the exe pushes them unmasked), the structs declared here (codfh ctdata.h) with compile-time layout
 * asserts.  Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct rpStreamCollTree {
    RwUInt32 flags;
    RwV3d inf;
    RwV3d sup;
    RwUInt32 numEntries;
    RwUInt32 numSplits;
} rpStreamCollTree;
typedef struct RpCollSector {
    RwUInt8 type;
    RwUInt8 pad;
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
RW_LAYOUT_ASSERT(rw_assert_streamtree_size, sizeof(rpStreamCollTree) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_split_size, sizeof(RpCollSplit) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_tree_nsplits, RW_OFFSETOF(RpCollTree, numSplits) == 0x1e);
RW_LAYOUT_ASSERT(rw_assert_tree_map, RW_OFFSETOF(RpCollTree, map) == 0x24);

#define rwID_STRUCT 0x1
#define rpCOLLTREE_USEMAP 0x01

/*007d2128*/ extern void *RwEngineInstance;
/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*0054dbc0*/ extern RpCollTree *_rpCollTreeCreate(RwUInt32 numEntries, RwUInt32 numSplits, RwBBox *bbox, RwUInt32 flags);

#define RwStreamReadInt16(stream, shorts, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(shorts), (numBytes))

/* TIER2: validated 2026-11-10 authority=diffemu */
RpCollTree *_rpCollTreeStreamRead(RwStream *stream)
{
    RpCollTree *tree;
    RwUInt32 version;
    rpStreamCollTree streamCollTree;
    RwBBox bbox;
    RwUInt32 size;

    /* STRUCT header */
    if (!RwStreamFindChunk(stream, rwID_STRUCT, NULL, &version))
    {
        return NULL;
    }

    /* rpStreamCollTree */
    size = sizeof(rpStreamCollTree);
    if (RwStreamRead(stream, &streamCollTree, size) != size)
    {
        return NULL;
    }

    bbox.inf = streamCollTree.inf;
    bbox.sup = streamCollTree.sup;

    tree = _rpCollTreeCreate(streamCollTree.numEntries, streamCollTree.numSplits, &bbox, streamCollTree.flags);
    if (!tree)
    {
        return NULL;
    }

    /* Splits */
    if (tree->numSplits > 0)
    {
        size = tree->numSplits * sizeof(RpCollSplit);
        if (RwStreamRead(stream, tree->splits, size) != size)
        {
            RwFree(tree);      /* _rpCollTreeDestroy */
            return NULL;
        }
    }

    /* Map */
    if (rpCOLLTREE_USEMAP & tree->flags)
    {
        if (!RwStreamReadInt16(stream, tree->map, tree->numEntries * sizeof(RwUInt16)))
        {
            RwFree(tree);      /* _rpCollTreeDestroy */
            return NULL;
        }
    }

    return tree;
}
