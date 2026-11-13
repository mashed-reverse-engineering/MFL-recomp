/* StalacMiteAlloc (MFL.exe 004e6420). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2define.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

#define rwPLUGIN_ID rwID_COREPLUGIN
extern void *
StalacMiteAlloc(RwUInt32 size);
typedef struct P2MemoryLimits P2MemoryLimits;
struct P2MemoryLimits
{
    RwUInt8 *StalacTiteBase;
    RwUInt8 *StalacMiteBase;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(P2MemoryLimits, StalacMiteBase) == 4);      /* MFL.exe 0x4e6424 / 0x4e6430: StalacMiteBase at 0x7d4d20, StalacTiteBase at 0x7d4d1c = gMemoryLimits */
extern P2MemoryLimits gMemoryLimits;
/* TIER2: unvalidated authority=diffemu */
void * StalacMiteAlloc(RwUInt32 size)
{

    size = (size + 3) & ~3;

    gMemoryLimits.StalacMiteBase += size;

    if (gMemoryLimits.StalacMiteBase > gMemoryLimits.StalacTiteBase)
    {
        gMemoryLimits.StalacMiteBase -= size;
        RWERROR((E_RW_NOMEM, size));
        return (NULL);
    }

    return (gMemoryLimits.StalacMiteBase - size);
}
