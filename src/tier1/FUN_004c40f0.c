#include "mfl_tier2.h"
/*004c40c0*/ extern char *FUN_004c40c0(undefined4 p0);
/* Looks up the entry for `index` via FUN_004c40c0, which returns a pointer to it, and returns the 32-bit value stored there. (inferred: the callee's role is unknown) Parameter: index - ... */
/* FUN_004c40f0
 * Looks up the entry for `index` via FUN_004c40c0, which returns a pointer to it,
 * and returns the 32-bit value stored there. (inferred: the callee's role is unknown)
 * Parameter: index - passed unchanged to FUN_004c40c0 (inferred meaning)
 * Returns: the first dword of the entry (no NULL check; the callee must not return NULL)
 */
undefined4 FUN_004c40f0(undefined4 index)
{
  undefined4 *entry_ptr;

  entry_ptr = (undefined4 *)FUN_004c40c0(index);
  return *entry_ptr;
}
