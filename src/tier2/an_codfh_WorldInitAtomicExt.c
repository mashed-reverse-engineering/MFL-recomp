/* WorldInitAtomicExt (MFL.exe 004f3cf0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

typedef struct RpWorldAtomicExt RpWorldAtomicExt;
struct RpWorldAtomicExt
{
    
    RpWorld            *world;

    RwObjectHasFrameSyncFunction oldSync;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldAtomicExt, oldSync) == 4);      /* MFL.exe 0x4f3d15: mov [ecx+0x4],edx (the saved sync function) */
#define ATOMICEXTFROMATOMIC(atomic) ((RpWorldAtomicExt *)(((RwUInt8 *)(atomic)) + atomicExtOffset))
extern RwInt32      atomicExtOffset;
extern RwObjectHasFrame * WorldAtomicSync(RwObjectHasFrame * type);
/* TIER2: unvalidated authority=diffemu */
void * WorldInitAtomicExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpWorldAtomicExt   *atomicExt = ATOMICEXTFROMATOMIC(object);
    RpAtomic           *atomic = (RpAtomic *) object;

    atomicExt->world = (RpWorld *)NULL;

    atomic->renderFrame = RWSRCGLOBAL(renderFrame) - 1;

    atomicExt->oldSync = atomic->object.sync;
    atomic->object.sync = WorldAtomicSync;

    return (object);
}
