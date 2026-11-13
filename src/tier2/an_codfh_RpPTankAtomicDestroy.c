/* RpPTankAtomicDestroy (MFL.exe 00545d00). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: generated, then hand-checked: function cut out of the file, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

/*007d2128*/ extern void *RwEngineInstance;

/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void RpPTankAtomicDestroy(RpAtomic *ptank)
{

    RpAtomicDestroy(ptank);

    return;
}
