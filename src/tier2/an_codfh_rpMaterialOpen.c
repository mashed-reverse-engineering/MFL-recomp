/* rpMaterialOpen = codfh _rpMaterialOpen (MFL.exe 0x004f8090; exe name as rpMaterialClose's tier-1 file): RenderWare Graphics 3.7
 * (Criterion Software), material module open: creates the material free list in the module's globals.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamateri.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RWMATERIALGLOBAL / RWPLUGINOFFSET expanded to the byte
 * offset into *RwEngineInstance, free list alignment 4 and hint 0x40007 (the exe's values), the structs declared here with
 * compile-time layout asserts.  Module-level only: no RpMaterial field is touched.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpMaterialGlobals {
    void *matFreeList;                  /* RwFreeList * */
} rpMaterialGlobals;
typedef struct RwPluginRegistryHead {
    RwInt32 sizeOfStruct;
} RwPluginRegistryHead;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define RP_MATERIAL_FREELIST_ALIGN 4
#define RP_MATERIAL_FREELIST_HINT 0x40007      /* rwMEMHINTDUR_GLOBAL | rwID_MATERIAL */

/*007d2128*/ extern void *RwEngineInstance;
/*007d5390*/ extern RwModuleInfo materialModule;
/*0062ec44*/ extern RwPluginRegistryHead materialTKList;
/*0062ec68*/ extern RwInt32 _rpMaterialFreeListBlockSize;
/*0062ec6c*/ extern RwInt32 _rpMaterialFreeListPreallocBlocks;
/*007d5368*/ extern RwUInt8 _rpMaterialFreeList[];
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
#define RWMATERIALGLOBAL(var) (((rpMaterialGlobals *)((RwUInt8 *)RwEngineInstance + materialModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rpMaterialOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Store the data block offset (the same for all instances) */
    materialModule.globalsOffset = offset;

    RWMATERIALGLOBAL(matFreeList) = RwFreeListCreateAndPreallocateSpace(materialTKList.sizeOfStruct, _rpMaterialFreeListBlockSize,
                                                                        RP_MATERIAL_FREELIST_ALIGN, _rpMaterialFreeListPreallocBlocks,
                                                                        _rpMaterialFreeList, RP_MATERIAL_FREELIST_HINT);
    if (!RWMATERIALGLOBAL(matFreeList))
    {
        /* Failure */
        return NULL;
    }

    /* One more module instance */
    ++materialModule.numInstances;
    return instance;
}
