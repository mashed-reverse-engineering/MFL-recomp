/* bamatrix module (MFL.exe 0x004d3780 / 0x004d3b40): RenderWare Graphics 3.7 (Criterion Software), matrix module open and the
 * multiply-function setter (rwMatrixSetMultFn = codfh _rwMatrixSetMultFn, rwMatrixOpen = codfh _rwMatrixOpen; exe names).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamatrix.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RWMATRIXGLOBAL / RWPLUGINOFFSET expanded to the
 * byte offset into *RwEngineInstance; in rwMatrixOpen _rwMatrixSetOptimizations, _rwMatrixSetMultFn and RwEngineSetMatrixTolerances
 * written out as the exe inlined them; the free list alignment is 4 and the hint 0x4000d (the exe's values; codfh has
 * rwMATRIXALIGNMENT); the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef RwMatrix *(*rwMatrixMultFn)(RwMatrix *dstMat, const RwMatrix *matA, const RwMatrix *matB);
typedef struct RwMatrixTolerance {
    RwReal Normal;
    RwReal Orthogonal;
    RwReal Identity;
} RwMatrixTolerance;
typedef struct rwMatrixGlobals {
    void *matrixFreeList;               /* RwFreeList * */
    RwInt32 matrixOptimizations;
    rwMatrixMultFn multMatrix;
    RwMatrixTolerance tolerance;
} rwMatrixGlobals;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
RW_LAYOUT_ASSERT(rw_assert_mult, RW_OFFSETOF(rwMatrixGlobals, multMatrix) == 8);
RW_LAYOUT_ASSERT(rw_assert_tolerance, RW_OFFSETOF(rwMatrixGlobals, tolerance) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define rwMATRIXOPTIMIZE_IDENTITY 0x20000
#define RWMATRIXOPTIMIZETOLERANCE ((RwReal)0.01)
#define RW_MATRIX_FREELIST_ALIGN 4
#define RW_MATRIX_FREELIST_HINT 0x4000d      /* rwMEMHINTDUR_GLOBAL | rwID_MATRIX */

/*007d2128*/ extern void *RwEngineInstance;
/*007d2150*/ extern RwModuleInfo matrixModule;
/*0062e6ac*/ extern RwInt32 _rwMatrixFreeListBlockSize;
/*0062e6b0*/ extern RwInt32 _rwMatrixFreeListPreallocBlocks;
/*007d212c*/ extern RwUInt8 _rwMatrixFreeList[];
/*004d37b0*/ extern RwMatrix *MatrixMultiply(RwMatrix *dstMat, const RwMatrix *matA, const RwMatrix *matB);
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
#define RWMATRIXGLOBAL(var) (((rwMatrixGlobals *)((RwUInt8 *)RwEngineInstance + matrixModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
/* multMatFn is an rwMatrixMultFn (a void * here: diffemu's signature parser takes no function typedefs) */
RwBool rwMatrixSetMultFn(void *multMatFn)
{
    rwMatrixMultFn multMat = (rwMatrixMultFn)multMatFn;

    if (NULL == multMat)
    {
        multMat = MatrixMultiply;
    }
    RWMATRIXGLOBAL(multMatrix) = multMat;
    return 1;      /* TRUE */
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwMatrixOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Save data block offset (same for all instances) */
    matrixModule.globalsOffset = offset;

    RWMATRIXGLOBAL(matrixFreeList) = RwFreeListCreateAndPreallocateSpace(sizeof(RwMatrix), _rwMatrixFreeListBlockSize, RW_MATRIX_FREELIST_ALIGN,
                                                                         _rwMatrixFreeListPreallocBlocks, _rwMatrixFreeList, RW_MATRIX_FREELIST_HINT);
    if (!RWMATRIXGLOBAL(matrixFreeList))
    {
        /* Failure */
        instance = NULL;
    }
    else
    {
        const RwMatrixTolerance tolerance = { RWMATRIXOPTIMIZETOLERANCE, RWMATRIXOPTIMIZETOLERANCE, RWMATRIXOPTIMIZETOLERANCE };

        /* Set default optimizations */
        RWMATRIXGLOBAL(matrixOptimizations) = rwMATRIXOPTIMIZE_IDENTITY;
        /* Set default multiply function */
        RWMATRIXGLOBAL(multMatrix) = MatrixMultiply;
        /* Set default tolerances */
        RWMATRIXGLOBAL(tolerance) = tolerance;

        /* One more module instance */
        ++matrixModule.numInstances;
    }
    return instance;
}
