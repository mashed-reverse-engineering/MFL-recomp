/* _rwStreamReadChunkHeader (MFL.exe 0x004dc540): RenderWare Graphics 3.7 (Criterion Software), reads one 12 byte chunk
 * header (type, length, library ID) from a stream and unpacks the library ID into version and build number.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/babinary.c @e578672,
 * not byte-identical, changes: PC build (little endian: RwMemNative32 is a no-op and gone), RWERROR expanded as the release
 * build has it (_rwerror + RwErrorSet), RWFUNCTION / RWASSERT / RWRETURN expanded, the RWLIBRARYIDUNPACK macros inlined,
 * the structs declared here (codfh babinary.c / babinary.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

/* the chunk header as it is in the stream */
typedef struct RwMark {
    RwUInt32 type;
    RwUInt32 length;
    RwUInt32 libraryID;
} RwMark;
typedef struct RwChunkHeaderInfo {
    RwUInt32 type;      /* chunk ID */
    RwUInt32 length;    /* length of the chunk data in bytes */
    RwUInt32 version;   /* version of the chunk data */
    RwUInt32 buildNum;  /* build number of the libraries that wrote it */
    RwBool isComplex;   /* TRUE if the chunk contains sub-chunks */
} RwChunkHeaderInfo;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_mark_size, sizeof(RwMark) == 12);
RW_LAYOUT_ASSERT(rw_assert_chunk_size, sizeof(RwChunkHeaderInfo) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);

#define rwID_COREPLUGIN 0x01
#define E_RW_READ ((RwInt32)0x8000001a)      /* "Read error on stream" (RwErrorCodeCommon) */

/* RWLIBRARYIDUNPACKVERSION / RWLIBRARYIDUNPACKBUILDNUM (rwcore.h) */
#define RWLIBRARYIDUNPACKVERSION(libraryID) ((((libraryID) >> 14) & 0x3ff00) + 0x30000 | (((libraryID) >> 16) & 0x3f))
#define RWLIBRARYIDUNPACKBUILDNUM(libraryID) ((libraryID) & 0xffff)

/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc630*/ extern RwBool ChunkIsComplex(RwChunkHeaderInfo *chunkHeaderInfo);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: unvalidated 2026-11-08 authority=diffemu */
RwBool _rwStreamReadChunkHeader(RwStream *stream, RwUInt32 *type, RwUInt32 *length, RwUInt32 *version, RwUInt32 *buildNum)
{
    RwMark mark;
    RwChunkHeaderInfo chunkHdrInfo;

    /* Read it */
    if (RwStreamRead(stream, &mark, sizeof(mark)) != sizeof(mark))
    {
        /* Ooops, failed somewhere */
        RwError error;

        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_READ);
        RwErrorSet(&error);
        return 0;      /* FALSE */
    }

    /* Use it */
    chunkHdrInfo.type = mark.type;
    chunkHdrInfo.length = mark.length;

    /* Check for old library ID */
    if (!(mark.libraryID & 0xffff0000))
    {
        /* Just contains old-style version number */
        chunkHdrInfo.version = mark.libraryID << 8;
        chunkHdrInfo.buildNum = 0;
    }
    else
    {
        /* Unpack new library ID */
        chunkHdrInfo.version = RWLIBRARYIDUNPACKVERSION(mark.libraryID);
        chunkHdrInfo.buildNum = RWLIBRARYIDUNPACKBUILDNUM(mark.libraryID);
    }

    chunkHdrInfo.isComplex = ChunkIsComplex(&chunkHdrInfo);

    if (type)
    {
        *type = chunkHdrInfo.type;
    }
    if (length)
    {
        *length = chunkHdrInfo.length;
    }
    if (buildNum)
    {
        *buildNum = chunkHdrInfo.buildNum;
    }
    if (version)
    {
        *version = chunkHdrInfo.version;
    }

    return 1;      /* TRUE */
}
