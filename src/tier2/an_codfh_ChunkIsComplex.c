/* ChunkIsComplex (MFL.exe 0x004dc630): RenderWare Graphics 3.7 (Criterion Software), TRUE for the core chunk types that hold
 * sub-chunks (callers: _rwStreamReadChunkHeader, RwStreamReadChunkHeaderInfo).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/babinary.c @e578672,
 * not byte-identical, changes: static -> external (the overlay patches it at its own address), the FALSE cases folded into
 * `default`, RWFUNCTION / RWASSERT / RWRETURN expanded, RwChunkHeaderInfo declared here (codfh babinary.h) with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwChunkHeaderInfo {
    RwUInt32 type;      /* chunk ID */
    RwUInt32 length;    /* length of the chunk data in bytes */
    RwUInt32 version;   /* version of the chunk data */
    RwUInt32 buildNum;  /* build number of the libraries that wrote it */
    RwBool isComplex;   /* TRUE if the chunk contains sub-chunks */
} RwChunkHeaderInfo;
RW_LAYOUT_ASSERT(rw_assert_chunk_type, RW_OFFSETOF(RwChunkHeaderInfo, type) == 0);
RW_LAYOUT_ASSERT(rw_assert_chunk_size, sizeof(RwChunkHeaderInfo) == 0x14);

/* core chunk IDs (RwCorePluginID) with sub-chunks */
enum {
    rwID_CAMERA = 0x05, rwID_TEXTURE = 0x06, rwID_MATERIAL = 0x07, rwID_MATLIST = 0x08, rwID_ATOMICSECT = 0x09,
    rwID_PLANESECT = 0x0a, rwID_WORLD = 0x0b, rwID_FRAMELIST = 0x0e, rwID_GEOMETRY = 0x0f, rwID_CLUMP = 0x10,
    rwID_LIGHT = 0x12, rwID_ATOMIC = 0x14, rwID_GEOMETRYLIST = 0x1a
};

/* TIER2: unvalidated 2026-11-08 authority=diffemu */
RwBool ChunkIsComplex(RwChunkHeaderInfo *chunkHeaderInfo)
{
    switch (chunkHeaderInfo->type)
    {
        case rwID_CAMERA:
        case rwID_TEXTURE:
        case rwID_MATERIAL:
        case rwID_MATLIST:
        case rwID_ATOMICSECT:
        case rwID_PLANESECT:
        case rwID_WORLD:
        case rwID_FRAMELIST:
        case rwID_GEOMETRY:
        case rwID_CLUMP:
        case rwID_LIGHT:
        case rwID_ATOMIC:
        case rwID_GEOMETRYLIST:
            return 1;      /* TRUE */

        default:      /* rwID_STRUCT, rwID_STRING, rwID_EXTENSION, rwID_MATRIX, rwID_UNICODESTRING and all others */
            return 0;
    }
}
