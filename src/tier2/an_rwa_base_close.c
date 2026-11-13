/* RenderWare Audio (Criterion Software) base module, MFL.exe 0x005b6860: library shutdown (close the 16 sub-modules in reverse
 * order) and 0x005bf1b0: install the memory functions.  Written from the MFL.exe disassembly (objdump). */
#include "mfl_tier2.h"

typedef struct RwaModuleEntry {      /* 8-byte row of the base module table */
  void (*open)(void);
  void (*close)(void);
} RwaModuleEntry;

typedef struct RwaMemoryFunctions {      /* the four words of RwGlobals.memoryFuncs (+0x108) */
  void *(*rwmalloc)(size_t size, RwUInt32 hint);
  void (*rwfree)(void *mem);
  void *(*rwrealloc)(void *mem, size_t size, RwUInt32 hint);
  void *(*rwcalloc)(size_t count, size_t size, RwUInt32 hint);
} RwaMemoryFunctions;

/*005bae80*/ extern char *RwaObjGetTopLevelObject(void);
/*005bace0*/ extern void *_rwaObjFinishExecute(void *obj, int flag);
/*005b6630*/ extern void rwa_base_005b6630(RwaModuleEntry *entry, RwUInt32 index);      /* per-module close bookkeeping */
/*005bf140*/ extern void *rwa_memory_malloc_005bf140(size_t size, RwUInt32 hint);
/*004b45fd*/ extern void __cdecl free(void *mem);
/*005bf150*/ extern void *rwa_memory_realloc_005bf150(void *mem, size_t size, RwUInt32 hint);
/*005bf170*/ extern void *rwa_memory_005bf170(size_t count, size_t size, RwUInt32 hint);      /* calloc */

/*007d2128*/ extern void *RwEngineInstance;      /* RwGlobals */
/*00649140*/ extern RwaModuleEntry g_rwa_base_modules[16];
/*007daa8c*/ extern RwInt32 g_data_7daa8c;         /* library open */
/*007daa90*/ extern RwInt32 g_data_7daa90;         /* shutdown in progress */
/*007daa98*/ extern RwInt32 g_data_7daa98;         /* update counter */
/*007daa9c*/ extern RwInt32 rwabase_updating;      /* an object update is open */

/* Ends an open update of the top level object, then closes the modules from the last to the first. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void rwa_base_close_005b6860(void)
{
  RwUInt32 i;

  g_data_7daa90 = 1;
  if (rwabase_updating == 1) {
    _rwaObjFinishExecute(RwaObjGetTopLevelObject(), 0);
    rwabase_updating = 0;
    g_data_7daa98++;
  }
  for (i = 16; i > 0; i--) {
    g_rwa_base_modules[i - 1].close();
    rwa_base_005b6630(&g_rwa_base_modules[i - 1], i - 1);
  }
  g_data_7daa8c = 0;
  g_data_7daa90 = 0;
}

/* Copies the caller's four memory functions into the engine globals, or installs the library's own (malloc, the CRT free,
   realloc, calloc) when none are given.  Always 1. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool rwa_memory_set_functions_005bf1b0(const RwaMemoryFunctions *functions)
{
  if (functions) {
    RW_GLOBAL(0x108, RwaMemoryFunctions) = *functions;
    return 1;
  }
  RW_GLOBAL(0x108, RwaMemoryFunctions).rwmalloc = rwa_memory_malloc_005bf140;
  RW_GLOBAL(0x108, RwaMemoryFunctions).rwfree = free;
  RW_GLOBAL(0x108, RwaMemoryFunctions).rwrealloc = rwa_memory_realloc_005bf150;
  RW_GLOBAL(0x108, RwaMemoryFunctions).rwcalloc = rwa_memory_005bf170;
  return 1;
}
