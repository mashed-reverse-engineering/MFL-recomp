/* bavector module (MFL.exe 0x004d5700 / 0x004d5770): RenderWare Graphics 3.7 (Criterion Software), vector module open / close:
 * default transform functions and the sqrt / inverse-sqrt lookup tables (rwVectorOpen / rwVectorClose = codfh _rwVectorOpen /
 * _rwVectorClose; exe names).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bavector.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RWERROR expanded as the release build has it
 * (_rwerror + RwErrorSet), RwMalloc / RwFree expanded to the RwGlobals memory functions (0x108 / 0x10c), RWVECTORGLOBAL /
 * RWPLUGINOFFSET expanded to the byte offset into *RwEngineInstance; _rwVectorSetMultFn(NULL x4), SqrtTableCreate /
 * InvSqrtTableCreate and the two table destroys written out where the exe inlined them (sqrt = the double sqrt intrinsic, fsqrt);
 * the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef union RwSplitBits {
    RwReal nReal;
    volatile RwInt32 nInt;
    volatile RwUInt32 nUInt;
} RwSplitBits;
typedef struct rwVectorGlobals {
    RwSplitBits *SqrtTab;
    RwSplitBits *InvSqrtTab;
    void *multPoint;
    void *multPoints;
    void *multVector;
    void *multVectors;
} rwVectorGlobals;
typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_vector_multvectors, RW_OFFSETOF(rwVectorGlobals, multVectors) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);

#define rwSQRTTABLESIZE 0x800
#define rwSQRTTABLESHIFT 12
#define rwSQRTTABLEBYTES (2 * rwSQRTTABLESIZE * sizeof(RwSplitBits))
#define rwVECTOR_MEMHINT 0x40401      /* rwID_VECTORMODULE | rwMEMHINTDUR_GLOBAL */
#define rwID_COREPLUGIN 0x01
#define E_RW_NOMEM ((RwInt32)0x80000013)
#define RW_GLOBALS_RWMALLOC 0x108      /* RwGlobals.memoryFuncs (rw37.h RwGlobals37) */
#define RW_GLOBALS_RWFREE 0x10c

typedef void *(*RwMallocFn)(size_t size, RwUInt32 hint);
typedef void (*RwFreeFn)(void *mem);
extern double sqrt(double x);      /* /O2 implies /Oi: inline fsqrt like the exe */

/*007d2128*/ extern void *RwEngineInstance;
/*007d2158*/ extern RwModuleInfo vectorModule;
/*004d5010*/ extern RwV3d *VectorMultPoint(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix);
/*004d4f50*/ extern RwV3d *VectorMultPoints(RwV3d *pointsOut, const RwV3d *pointsIn, RwInt32 numPoints, const RwMatrix *matrix);
/*004d5160*/ extern RwV3d *VectorMultVector(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix);
/*004d50b0*/ extern RwV3d *VectorMultVectors(RwV3d *pointsOut, const RwV3d *pointsIn, RwInt32 numPoints, const RwMatrix *matrix);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
#define RWVECTORGLOBAL(var) (((rwVectorGlobals *)((RwUInt8 *)RwEngineInstance + vectorModule.globalsOffset))->var)
#define RwMalloc(size, hint) ((*(RwMallocFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWMALLOC))((size), (hint)))
#define RwFree(mem) ((*(RwFreeFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWFREE))(mem))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwVectorClose(void *instance, RwInt32 offset, RwInt32 size)
{
    /* InvSqrtTableDestroy */
    if (NULL != RWVECTORGLOBAL(InvSqrtTab))
    {
        RwFree(RWVECTORGLOBAL(InvSqrtTab));
        RWVECTORGLOBAL(InvSqrtTab) = NULL;
    }
    /* SqrtTableDestroy */
    if (NULL != RWVECTORGLOBAL(SqrtTab))
    {
        RwFree(RWVECTORGLOBAL(SqrtTab));
        RWVECTORGLOBAL(SqrtTab) = NULL;
    }

    /* One less module instance */
    --vectorModule.numInstances;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwVectorOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    RwUInt32 i;
    RwSplitBits *tab2to4, *tab1to2;
    RwSplitBits spIn, spOut;
    RwError error;

    /* Save the global data offset (same for all instances) */
    vectorModule.globalsOffset = offset;

    /* Set default vector operation functions */
    RWVECTORGLOBAL(multPoint) = (void *)VectorMultPoint;
    RWVECTORGLOBAL(multPoints) = (void *)VectorMultPoints;
    RWVECTORGLOBAL(multVector) = (void *)VectorMultVector;
    RWVECTORGLOBAL(multVectors) = (void *)VectorMultVectors;

    /* SqrtTableCreate: X^0.5 */
    tab2to4 = (RwSplitBits *)RwMalloc(rwSQRTTABLEBYTES, rwVECTOR_MEMHINT);
    if (NULL == tab2to4)
    {
        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_NOMEM, rwSQRTTABLEBYTES);
        RwErrorSet(&error);
        return NULL;
    }
    tab1to2 = &tab2to4[rwSQRTTABLESIZE];
    spIn.nReal = (RwReal)1;
    for (i = 0; i < rwSQRTTABLESIZE; ++i)
    {
        spOut.nReal = (RwReal)sqrt(spIn.nReal);
        spOut.nUInt = spOut.nUInt + 0x00400000UL - 0x20000000UL;
        tab1to2[i].nUInt = spOut.nUInt;
        spIn.nUInt += (1L << rwSQRTTABLESHIFT);
    }
    for (i = 0; i < rwSQRTTABLESIZE; ++i)
    {
        spOut.nReal = (RwReal)sqrt(spIn.nReal);
        spOut.nUInt = spOut.nUInt - 0x20000000UL;
        tab2to4[i].nUInt = spOut.nUInt;
        spIn.nUInt += (1L << rwSQRTTABLESHIFT);
    }
    RWVECTORGLOBAL(SqrtTab) = tab2to4;

    /* InvSqrtTableCreate: X^-0.5 */
    tab2to4 = (RwSplitBits *)RwMalloc(rwSQRTTABLEBYTES, rwVECTOR_MEMHINT);
    if (NULL == tab2to4)
    {
        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_NOMEM, rwSQRTTABLEBYTES);
        RwErrorSet(&error);
        return NULL;
    }
    tab1to2 = &tab2to4[rwSQRTTABLESIZE];
    spIn.nReal = (RwReal)1;
    for (i = 0; i < rwSQRTTABLESIZE; ++i)
    {
        spOut.nReal = (RwReal)(1.0f / sqrt(spIn.nReal));
        spOut.nUInt = spOut.nUInt - 0x20000000UL;
        tab1to2[i].nUInt = spOut.nUInt;
        spIn.nUInt += (1L << rwSQRTTABLESHIFT);
    }
    for (i = 0; i < rwSQRTTABLESIZE; ++i)
    {
        spOut.nReal = (RwReal)(1.0f / sqrt(spIn.nReal));
        spOut.nUInt = spOut.nUInt + 0x00400000UL - 0x20000000UL;
        tab2to4[i].nUInt = spOut.nUInt;
        spIn.nUInt += (1L << rwSQRTTABLESHIFT);
    }
    RWVECTORGLOBAL(InvSqrtTab) = tab2to4;

    /* This is another module instance */
    ++vectorModule.numInstances;
    return instance;
}
