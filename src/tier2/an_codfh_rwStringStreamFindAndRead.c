/* _rwStringStreamFindAndRead (MFL.exe 0x004ea4b0): RenderWare Graphics 3.7 (Criterion Software), skips chunks up to the next
 * string chunk (rwID_STRING, or rwID_UNICODESTRING narrowed to chars) and reads it into `string` (NULL: RwMalloc'ed here).
 * Texture, material and frame names come through it.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/rwstring.c @e578672,
 * not byte-identical, changes: PC build (no RWUNICODE); the static readers StringStreamRead / UnicodeStringStreamRead are
 * written into this function (the original inlined them; one function per tier 2 file), their unused `mallocced` flag of the
 * ANSI reader dropped; RWALIGN empty (P7); RWERROR expanded as the release build has it (_rwerror + RwErrorSet);
 * RWFUNCTION / RWASSERT / RWRETURN expanded.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;

#define rwID_COREPLUGIN 0x01
#define rwID_STRING 0x02
#define rwID_UNICODESTRING 0x13
#define rwMEMHINTDUR_EVENT 0x00030000
#define rwLIBRARYBASEVERSION 0x35000
#define rwLIBRARYCURRENTVERSION 0x37002
#define E_RW_BADVERSION ((RwInt32)0x80000004)      /* "Invalid format version" (RwErrorCodeCommon) */
#define E_RW_NOMEM ((RwInt32)0x80000013)           /* "Unable to allocate %d bytes of memory" */

/*007d2128*/ extern void *RwEngineInstance;      /* RwMalloc / RwFree (mfl_tier2.h): memoryFuncs +0x108 / +0x10c */
/*004dbe70*/ extern RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length);
/*004dc190*/ extern RwStream *RwStreamSkip(RwStream *stream, RwUInt32 offset);
/*004dc540*/ extern RwBool _rwStreamReadChunkHeader(RwStream *stream, RwUInt32 *type, RwUInt32 *length, RwUInt32 *version, RwUInt32 *buildNum);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

/* TIER2: validated 2026-11-10 authority=diffemu */
RwChar *_rwStringStreamFindAndRead(RwChar *string, RwStream *stream)
{
    RwUInt32 type, length, version;

    while (_rwStreamReadChunkHeader(stream, &type, &length, &version, (RwUInt32 *)0))
    {
        const RwBool valid = ((version >= rwLIBRARYBASEVERSION) && (version <= rwLIBRARYCURRENTVERSION));

        /* Reject invalid versions */
        if (!valid)
        {
            RwError error;

            error.pluginID = rwID_COREPLUGIN;
            error.errorCode = _rwerror(E_RW_BADVERSION);
            RwErrorSet(&error);
            return 0;
        }

        if (type == rwID_STRING)
        {
            /* StringStreamRead: chars in the stream; the length in bytes is also the number of chars */
            char multiByteString[64];
            RwChar *nativeString = string;
            RwChar *baseString;

            if (nativeString == 0)
            {
                nativeString = (RwChar *)RwMalloc(length * sizeof(RwChar), rwID_STRING | rwMEMHINTDUR_EVENT);
                if (!nativeString)
                {
                    RwError error;

                    error.pluginID = rwID_COREPLUGIN;
                    error.errorCode = _rwerror(E_RW_NOMEM, length * sizeof(RwChar));
                    RwErrorSet(&error);
                    return 0;
                }
            }

            /* few strings are longer than 64 bytes: read in 64 byte pieces */
            baseString = nativeString;
            while (length > 0)
            {
                RwUInt32 bytesToRead = (length > 64) ? 64 : length;
                RwUInt32 i;

                if (RwStreamRead(stream, multiByteString, bytesToRead) != bytesToRead)
                {
                    return 0;      /* (a buffer allocated above is not freed: as the original) */
                }

                /* Reduce by the amount we read */
                length -= bytesToRead;

                /* convert into the native string format */
                for (i = 0; i < bytesToRead; ++i)
                {
                    baseString[i] = (RwChar)multiByteString[i];
                }
                baseString += bytesToRead;
            }

            return nativeString;
        }
        else if (type == rwID_UNICODESTRING)
        {
            /* UnicodeStringStreamRead: 16 bit chars in the stream, narrowed to RwChar (the low byte) */
            unsigned short uniCodeString[64];
            RwChar *nativeString = string;
            RwChar *baseString;
            RwBool mallocced = 0;

            if (nativeString == 0)
            {
                nativeString = (RwChar *)RwMalloc(length, rwID_STRING | rwMEMHINTDUR_EVENT);
                if (!nativeString)
                {
                    RwError error;

                    error.pluginID = rwID_COREPLUGIN;
                    error.errorCode = _rwerror(E_RW_NOMEM, length);
                    RwErrorSet(&error);
                    return 0;
                }
                mallocced = 1;
            }

            baseString = nativeString;
            while (length > 0)
            {
                RwUInt32 bytesToRead = (length > 64 * 2) ? 64 * 2 : length;
                RwUInt32 i, CharCount;

                if (RwStreamRead(stream, uniCodeString, bytesToRead) != bytesToRead)
                {
                    if (mallocced)
                    {
                        RwFree(nativeString);
                    }
                    return 0;
                }

                /* Reduce by the amount we read */
                length -= bytesToRead;

                /* convert into the native string format */
                CharCount = bytesToRead >> 1;
                for (i = 0; i < CharCount; ++i)
                {
                    baseString[i] = (RwChar)uniCodeString[i];
                }
                baseString += CharCount;
            }

            return nativeString;
        }

        if (!RwStreamSkip(stream, length))
        {
            return 0;
        }
    }

    return 0;
}
