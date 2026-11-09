/* _rwPluginRegistryReadDataChunks (MFL.exe 0x004f1ea0): RenderWare Graphics 3.7 (Criterion Software), reads the extension chunk of an
 * object from a stream and hands each plugin chunk to its registered read callback (unknown ones are skipped), then runs the
 * plugins' always callbacks.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/batkbin.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN / RWERROR expanded as the release build has them (_rwerror +
 * RwErrorSet), the rwLIBRARYBASEVERSION / rwLIBRARYCURRENTVERSION / rwCHUNKHEADERSIZE / rwID_EXTENSION constants inlined, the
 * RWDEBUG-only validation and messages dropped, the structs declared here (field order from the exe, codfh batkreg.h is not in the
 * tree) with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
typedef RwInt32 (*RwPluginDataChunkAlwaysCallBack)(void *, RwInt32, RwInt32);
struct RwPluginRegEntry {
    RwInt32 offset;
    RwInt32 size;
    RwUInt32 pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    void *rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    void *errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);
RW_LAYOUT_ASSERT(rw_assert_entry_size, RW_OFFSETOF(RwPluginRegEntry, size) == 0x4);
RW_LAYOUT_ASSERT(rw_assert_entry_pluginid, RW_OFFSETOF(RwPluginRegEntry, pluginID) == 0x8);
RW_LAYOUT_ASSERT(rw_assert_entry_read, RW_OFFSETOF(RwPluginRegEntry, readCB) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_entry_always, RW_OFFSETOF(RwPluginRegEntry, alwaysCB) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_entry_next, RW_OFFSETOF(RwPluginRegEntry, nextRegEntry) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_reg_first, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);

#define rwID_COREPLUGIN 0x01
#define rwID_EXTENSION 0x03
#define E_RW_BADVERSION ((RwInt32)0x80000004)
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwCHUNKHEADERSIZE 12

/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dc540*/ extern RwBool _rwStreamReadChunkHeader(RwStream *stream, RwUInt32 *type, RwUInt32 *length, RwUInt32 *version, RwUInt32 *buildNum);
/*004dc190*/ extern RwStream *RwStreamSkip(RwStream *stream, RwUInt32 offset);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
const RwPluginRegistry *_rwPluginRegistryReadDataChunks(const RwPluginRegistry *reg, RwStream *stream, void *object)
{
    RwUInt32 length, version;

    /* Find the extension chunk and get its length */
    if (!RwStreamFindChunk(stream, rwID_EXTENSION, &length, &version))
    {
        return 0;
    }

    if ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION))
    {
        RwPluginRegEntry *entry;

        /* Read a chunk from the stream.  If we can identify it as one for a registered plugin, then allow the plugin to read it,
         * else skip it */
        while (length > 0)
        {
            /* Read a chunk header */
            RwUInt32 readType, readLength;

            if (!_rwStreamReadChunkHeader(stream, &readType, &readLength, (RwUInt32 *)0, (RwUInt32 *)0))
            {
                return 0;
            }

            /* Search for chunkID in the registry list */
            entry = reg->firstRegEntry;
            while (entry)
            {
                if (entry->pluginID == readType)
                {
                    break;
                }
                entry = entry->nextRegEntry;
            }

            if (entry && entry->readCB)
            {
                if (!entry->readCB(stream, readLength, object, entry->offset, entry->size))
                {
                    /* Failed to read, return fail */
                    return 0;
                }
            }
            else
            {
                if (!RwStreamSkip(stream, readLength))
                {
                    return 0;
                }
            }

            length -= (readLength + rwCHUNKHEADERSIZE);
        }

        /* Call alwaysCBs for all the plugins (including those that didn't get any data to read) */
        entry = reg->firstRegEntry;
        while (entry)
        {
            if (entry->alwaysCB)
            {
                if (!entry->alwaysCB(object, entry->offset, entry->size))
                {
                    /* Failure, return fail */
                    return 0;
                }
            }
            entry = entry->nextRegEntry;
        }

        return reg;
    }
    else
    {
        RwError error;

        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_BADVERSION);
        RwErrorSet(&error);
        return 0;
    }
}
