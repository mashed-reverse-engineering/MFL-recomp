/* RenderWare Audio (Criterion Software) one-line rows, MFL.exe: the jump thunks 0x005bbd60 / 0x005ccc00 / 0x005da4a0, the empty
 * module hook 0x005bf220 and the library's default memory functions 0x005bf140 (malloc) / 0x005bf150 (realloc), installed by
 * rwa_memory_set_functions_005bf1b0.  Written from the MFL.exe disassembly (capstone). */
#include "mfl_tier2.h"

/*004b45eb*/ extern void *malloc(size_t size);
/*004b4729*/ extern void *realloc(void *mem, size_t new_size);
/*005bbc70*/ extern void _rwaRwStreamCancel(void *data);
/*005bc0a0*/ extern int RwaWaveDestroy(void *wave);

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void thunk__rwaRwStreamCancel_5bbd60(void *stream)
{
  _rwaRwStreamCancel(stream);
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
int thunk_RwaWaveDestroy_5ccc00(void *wave)
{
  return RwaWaveDestroy(wave);
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
int thunk_RwaWaveDestroy_5da4a0(void *wave)
{
  return RwaWaveDestroy(wave);
}

/* the empty per-module close hook (a bare ret) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void nullsub_6(void)
{
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *rwa_memory_malloc_005bf140(RwUInt32 size, RwUInt32 hint)
{
  return malloc(size);
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *rwa_memory_realloc_005bf150(void *mem, RwUInt32 size, RwUInt32 hint)
{
  return realloc(mem, size);
}
