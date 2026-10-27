#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall InterlockedIncrement(int);
#define REF_COUNT_OFFSET (8)
/* FUN_004b1000
 * Adds one to the 32-bit reference count stored at REF_COUNT_OFFSET inside obj
 * (atomic, via InterlockedIncrement) and returns the resulting count.
 * Counts below 2 are reported as 1 (the object is treated as having at least
 * one reference). obj is an object whose reference count lives at offset 8
 * (inferred: no struct in the dossier describes it).
 */
uint __stdcall FUN_004b1000(char *obj)
{
  uint ref_count;

  InterlockedIncrement((LONG *)(obj + REF_COUNT_OFFSET));
  ref_count = *(uint *)(obj + REF_COUNT_OFFSET);
  /* clamp: a count of 0 or 1 is returned as 1 */
  if (ref_count <= 1) {
    ref_count = 1;
  }
  return ref_count;
}
