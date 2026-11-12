/* RwErrorSet (MFL.exe 0x004ea160): RenderWare Graphics 3.7 (Criterion Software), records an error code in the engine's error
 * module globals unless an error is already pending.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/baerr.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), RWERRORGLOBAL / RWPLUGINOFFSET
 * expanded to the byte offset into *RwEngineInstance, the structs declared here (codfh baerr.h / bamemory.h) with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;      /* offset of the module's globals in RwGlobals */
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rwErrorGlobals {
    RwError errorCode;
} rwErrorGlobals;
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define E_RW_NOERROR ((RwInt32)0x80000000)

/*007d2128*/ extern void *RwEngineInstance;
/*007d518c*/ extern RwModuleInfo errorModule;
#define RWERRORGLOBAL(var) (((rwErrorGlobals *)((RwUInt8 *)RwEngineInstance + errorModule.globalsOffset))->var)

/* TIER2: validated 2026-11-10 authority=diffemu */
RwError *RwErrorSet(RwError *code)
{
    if ((RWERRORGLOBAL(errorCode).pluginID == 0x0) &&
        (RWERRORGLOBAL(errorCode).errorCode == E_RW_NOERROR))
    {
        if (code->errorCode & 0x80000000)
        {
            /* common error */
            RWERRORGLOBAL(errorCode).pluginID = 0x0;
        }
        else
        {
            /* plugin error */
            RWERRORGLOBAL(errorCode).pluginID = code->pluginID;
        }
        /* No current error status */
        RWERRORGLOBAL(errorCode).errorCode = code->errorCode;
    }
    return code;
}
