/* rwResourcesClose (MFL.exe 0x004ea810): RenderWare Graphics 3.7 (Criterion Software), closes the resources module: empties the
 * resources arena, releases the arena heap block and drops one module instance.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/baresour.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), RWRESOURCESGLOBAL expanded to the byte
 * offset into *RwEngineInstance (resourcesModule.globalsOffset), the exe tests the heap pointer before closing and freeing it
 * (codfh does not), _rwResHeapClose is the exe's always-TRUE stub, the module info struct declared here with compile-time layout
 * asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;      /* offset of the module's globals in RwGlobals */
    RwInt32 numInstances;
} RwModuleInfo;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define RES_MEMHEAP_OFFSET 0xc      /* rwResources.memHeap (the resources globals start with res), see an_codfh_rwResourcesOpen.c */

/*007d2128*/ extern void *RwEngineInstance;
/*00947138*/ extern RwModuleInfo resourcesModule;
/*004eab50*/ extern void *RwResourcesEmptyArena(void);
/*004a34a0*/ extern RwBool __cdecl stub_return_one(void *heap);      /* _rwResHeapClose */
#define RWRESOURCESGLOBAL_memHeap (*(void **)((RwUInt8 *)RwEngineInstance + resourcesModule.globalsOffset + RES_MEMHEAP_OFFSET))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwResourcesClose(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Release all the resources */
    RwResourcesEmptyArena();

    /* Free the block of memory used for the resources heap */
    if (RWRESOURCESGLOBAL_memHeap)
    {
        stub_return_one(RWRESOURCESGLOBAL_memHeap);
        RwFree(RWRESOURCESGLOBAL_memHeap);
        RWRESOURCESGLOBAL_memHeap = 0;
    }

    /* One less module instance */
    --resourcesModule.numInstances;

    return instance;
}
