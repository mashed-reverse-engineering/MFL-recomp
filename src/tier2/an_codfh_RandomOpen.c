/* RandomOpen (MFL.exe 0x00544fa0): RenderWare Graphics 3.7 (Criterion Software), random plugin open: allocates the 31-word
 * additive-feedback state table and seeds it.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/random/rprandom.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWAPIFUNCTION / RWRETURN expanded (release build), RwMalloc expanded to the RwGlobals
 * memory function (0x108), RWRANDOMGLOBAL / RWPLUGINOFFSET expanded to the byte offset into *RwEngineInstance, RpRandomSeed
 * (0x9a319039) inlined as the exe has it (RpRandom stays a call), the struct declared here with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RandomGlobals {
    RwUInt32 *randomTable;
    RwUInt32 *fptr;
    RwUInt32 *rptr;
    RwUInt32 *end_ptr;
} RandomGlobals;
RW_LAYOUT_ASSERT(rw_assert_random_end, RW_OFFSETOF(RandomGlobals, end_ptr) == 0xc);

#define DEG_3 31
#define SEP_3 3
#define rwID_RANDOMPLUGIN 0x10c
#define rwMEMHINTDUR_GLOBAL 0x40000
#define RW_GLOBALS_RWMALLOC 0x108      /* RwGlobals.memoryFuncs.rwmalloc (rw37.h RwGlobals37) */

typedef void *(*RwMallocFn)(size_t size, RwUInt32 hint);
/*007d2128*/ extern void *RwEngineInstance;
/*007da6b8*/ extern RwInt32 GlobalOffset;
/*00544e80*/ extern RwUInt32 RpRandom(void);
#define RWRANDOMGLOBAL(var) (((RandomGlobals *)((RwUInt8 *)RwEngineInstance + GlobalOffset))->var)
#define RwMalloc(size, hint) ((*(RwMallocFn *)((RwUInt8 *)RwEngineInstance + RW_GLOBALS_RWMALLOC))((size), (hint)))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *RandomOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    RwInt32 i;

    /* And continue with the initialization */
    RWRANDOMGLOBAL(randomTable) = (RwUInt32 *)RwMalloc(DEG_3 * sizeof(RwUInt32), rwID_RANDOMPLUGIN | rwMEMHINTDUR_GLOBAL);
    if (!RWRANDOMGLOBAL(randomTable))
    {
        /* Failure */
        return NULL;
    }
    RWRANDOMGLOBAL(fptr) = &RWRANDOMGLOBAL(randomTable)[SEP_3];
    RWRANDOMGLOBAL(rptr) = RWRANDOMGLOBAL(randomTable);
    RWRANDOMGLOBAL(end_ptr) = &RWRANDOMGLOBAL(randomTable)[DEG_3];

    /* Initialise the table: RpRandomSeed(0x9a319039) */
    RWRANDOMGLOBAL(randomTable)[0] = 0x9a319039UL;
    for (i = 1; i < DEG_3; i++)
    {
        RWRANDOMGLOBAL(randomTable)[i] = 1103515245 * RWRANDOMGLOBAL(randomTable)[i - 1] + 12345;
    }
    RWRANDOMGLOBAL(fptr) = &RWRANDOMGLOBAL(randomTable)[SEP_3];
    RWRANDOMGLOBAL(rptr) = &RWRANDOMGLOBAL(randomTable)[0];
    for (i = 0; i < 10 * DEG_3; i++)
    {
        (void)RpRandom();
    }

    return instance;
}
