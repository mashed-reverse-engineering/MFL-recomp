/* RwStreamWriteReal (MFL.exe 004dc820): writes an array of reals to a stream in 256 byte chunks through a frame buffer (the x86
 * build's real -> float32 conversion is a plain copy).  Written from the MFL.exe disassembly (lane P W8 / W9); RenderWare SDK types from
 * rw37_slim.h.  Compared with MFL.exe by differential emulation (tier 2).  cl 13 aligns this frame (and esp, -8), MFL.exe does not: the
 * stack bytes behind the copied `bytes` of a partial last word differ, RwStreamWrite reads only `bytes` of them. */
#include "rw37_slim.h"

extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);

#define rwSTREAMWRITEREALCHUNK 256

/* TIER2: unvalidated 2026-11-14 authority=static */
RwStream * RwStreamWriteReal(RwStream *stream, const RwReal *reals, RwUInt32 numBytes)
{
    RwUInt8 buffer[rwSTREAMWRITEREALCHUNK];
    const RwUInt8 *src = (const RwUInt8 *)reals;

    while (numBytes)
    {
        RwUInt32 bytes = (numBytes < rwSTREAMWRITEREALCHUNK) ? numBytes : rwSTREAMWRITEREALCHUNK;

        memcpy(buffer, src, bytes);
        if (!RwStreamWrite(stream, buffer, bytes))
        {
            return ((RwStream *)NULL);
        }
        numBytes -= bytes;
        src += bytes;
    }

    return (stream);
}
