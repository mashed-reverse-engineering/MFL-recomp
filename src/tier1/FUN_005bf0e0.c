#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define SLOT_SIZE (4)
#define ALIGN4_MASK (0xfffffffc)
/*
 * FUN_005bf0e0: allocate `size` bytes through the RenderWare memory hook
 * (rwmalloc, passing `hints` through) and return a 4-byte aligned pointer
 * into the block (inferred). SLOT_SIZE extra bytes are reserved so the raw
 * pointer can be stored in the 4 bytes just before the aligned pointer.
 * Returns 0 if the allocation failed.
 */
uint FUN_005bf0e0(int size,RwUInt32 hints)
{
  void *raw;
  uint aligned;

  raw = RwMalloc(size + SLOT_SIZE,hints);
  /* on success: round up to a 4-byte boundary and stash the raw pointer below it */
  if (0 < (char *)raw) {
    aligned = ((uint)raw + 4U) / 4U;
    aligned = aligned * 4U;
    *(void **)(aligned - SLOT_SIZE) = raw;
    return aligned;
  }
  return 0;
}
