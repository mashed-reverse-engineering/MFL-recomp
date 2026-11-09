/* _rwStringOpen (MFL.exe 0x004ea210): RenderWare Graphics 3.7 (Criterion Software), installs the ANSI string functions in the engine globals.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/rwstring.c @e578672,
 * not byte-identical, changes: PC build (no RWUNICODE branch, RWFUNCTION / RWRETURN expanded); RwGlobals is the 3.7 release
 * layout up to memoryFuncs (below, with compile-time offset asserts against librw-mfl capi/rw37.h RwGlobals37); the C runtime
 * functions are declared here (mfl_tier2.h and the CRT headers do not mix).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef char *rw_va_list;

/* the string function table of RwGlobals (codfh rwstring.h), 17 pointers; the typedef name comes from mfl_tier2.h */
struct RwStringFunctions {
    int (*vecSprintf)(RwChar *buffer, const RwChar *format, ...);
    int (*vecVsprintf)(RwChar *buffer, const RwChar *format, rw_va_list argptr);
    RwChar *(*vecStrcpy)(RwChar *dest, const RwChar *srce);
    RwChar *(*vecStrncpy)(RwChar *dest, const RwChar *srce, size_t size);
    RwChar *(*vecStrcat)(RwChar *dest, const RwChar *srce);
    RwChar *(*vecStrncat)(RwChar *dest, const RwChar *srce, size_t size);
    RwChar *(*vecStrrchr)(const RwChar *string, int findThis);
    RwChar *(*vecStrchr)(const RwChar *string, int findThis);
    RwChar *(*vecStrstr)(const RwChar *string, const RwChar *findThis);
    int (*vecStrcmp)(const RwChar *string1, const RwChar *string2);
    int (*vecStrncmp)(const RwChar *string1, const RwChar *string2, size_t max_size);
    int (*vecStricmp)(const RwChar *string1, const RwChar *string2);
    size_t (*vecStrlen)(const RwChar *string);
    RwChar *(*vecStrupr)(RwChar *string);
    RwChar *(*vecStrlwr)(RwChar *string);
    RwChar *(*vecStrtok)(RwChar *string, const RwChar *delimit);
    int (*vecSscanf)(const RwChar *buffer, const RwChar *format, ...);
};

/* RwGlobals (*RwEngineInstance) of the RenderWare 3.7 PC release build, the part up to the memory functions */
typedef struct RwGlobalsHead {
    void *curCamera;                    /* 0x00 */
    void *curWorld;                     /* 0x04 */
    RwUInt8 pad08[8];                   /* 0x08 renderFrame, lockedDevice */
    RwUInt8 dOpenDevice[56];            /* 0x10 RwDevice */
    void *stdFunc[29];                  /* 0x48 standard device functions */
    RwUInt8 dirtyFrameList[8];          /* 0xbc RwLinkList */
    RwStringFunctions stringFuncs;      /* 0xc4 */
    void *memoryFuncs[4];               /* 0x108 rwmalloc, rwfree, rwrealloc, rwcalloc */
} RwGlobalsHead;

/* layout barrier (window 2 plan step 0): values of rw37.h RwGlobals37 / RW37_STRLEN / RW37_STRNCPY */
RW_LAYOUT_ASSERT(rw_assert_dOpenDevice, RW_OFFSETOF(RwGlobalsHead, dOpenDevice) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_stdFunc, RW_OFFSETOF(RwGlobalsHead, stdFunc) == 0x48);
RW_LAYOUT_ASSERT(rw_assert_stringFuncs, RW_OFFSETOF(RwGlobalsHead, stringFuncs) == 0xc4);
RW_LAYOUT_ASSERT(rw_assert_vecStrncpy, RW_OFFSETOF(RwGlobalsHead, stringFuncs.vecStrncpy) == 0xd0);
RW_LAYOUT_ASSERT(rw_assert_vecStrlen, RW_OFFSETOF(RwGlobalsHead, stringFuncs.vecStrlen) == 0xf4);
RW_LAYOUT_ASSERT(rw_assert_vecSscanf, RW_OFFSETOF(RwGlobalsHead, stringFuncs.vecSscanf) == 0x104);
RW_LAYOUT_ASSERT(rw_assert_memoryFuncs, RW_OFFSETOF(RwGlobalsHead, memoryFuncs) == 0x108);

/*007d2128*/ extern RwGlobalsHead *RwEngineInstance;
#define RWSRCGLOBAL(variable) (RwEngineInstance->variable)

/* C runtime (libcmt, in MFL.exe) */
extern int sprintf(char *buffer, const char *format, ...);
extern int vsprintf(char *buffer, const char *format, rw_va_list argptr);
extern char *strcpy(char *dest, const char *srce);
extern char *strncpy(char *dest, const char *srce, size_t count);
extern char *strcat(char *dest, const char *srce);
extern char *strncat(char *dest, const char *srce, size_t count);
extern char *strstr(const char *string, const char *findThis);
extern int strcmp(const char *string1, const char *string2);
extern int strncmp(const char *string1, const char *string2, size_t count);
extern size_t strlen(const char *string);
extern char *strtok(char *string, const char *delimit);
extern int sscanf(const char *buffer, const char *format, ...);
/* RenderWare's own versions (rwstring.c, static there, byte-identical tier 1 files here) */
/*004ea3f0*/ extern RwChar *StrRChr(const RwChar *s, int c);
/*004ea3d0*/ extern RwChar *StrChr(const RwChar *s, int c);
/*004ea320*/ extern int StrICmp(const RwChar *s1, const RwChar *s2);
/*004ea370*/ extern RwChar *StrUpr(RwChar *s);
/*004ea3a0*/ extern RwChar *StrLwr(RwChar *s);

/* TIER2: unvalidated 2026-11-08 authority=diffemu */
RwBool _rwStringOpen(void)
{
    /* Install ANSI char string system */
    RWSRCGLOBAL(stringFuncs).vecSprintf = sprintf;
    RWSRCGLOBAL(stringFuncs).vecVsprintf = vsprintf;
    RWSRCGLOBAL(stringFuncs).vecStrcpy = strcpy;
    RWSRCGLOBAL(stringFuncs).vecStrncpy = strncpy;
    RWSRCGLOBAL(stringFuncs).vecStrcat = strcat;
    RWSRCGLOBAL(stringFuncs).vecStrncat = strncat;
    RWSRCGLOBAL(stringFuncs).vecStrrchr = StrRChr;
    RWSRCGLOBAL(stringFuncs).vecStrchr = StrChr;
    RWSRCGLOBAL(stringFuncs).vecStrstr = strstr;
    RWSRCGLOBAL(stringFuncs).vecStrcmp = strcmp;
    RWSRCGLOBAL(stringFuncs).vecStrncmp = strncmp;
    RWSRCGLOBAL(stringFuncs).vecStricmp = StrICmp;
    RWSRCGLOBAL(stringFuncs).vecStrlen = strlen;
    RWSRCGLOBAL(stringFuncs).vecStrupr = StrUpr;
    RWSRCGLOBAL(stringFuncs).vecStrlwr = StrLwr;
    RWSRCGLOBAL(stringFuncs).vecStrtok = strtok;
    RWSRCGLOBAL(stringFuncs).vecSscanf = sscanf;

    return 1;      /* TRUE */
}
