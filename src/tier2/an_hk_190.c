#include "mfl_tier2.h"
/* FUN_004af1f0 */
/*
 * Runs one virtual method on a sub-object and stores its result in the record.
 * record: pointer to a record (struct layout unknown, inferred from use).
 *   +0x4: cleared to 0 on every call (inferred: a flag or status word).
 *   +0x8: pointer to a sub-object whose vtable slot at +0x10 is called (inferred).
 *   +0xc: receives the value returned by that call.
 * Returns 0 (always).
 */

undefined4 __stdcall FUN_004af1f0(char *record)

{
  undefined4 result;

  *(undefined4 *)(record + 4) = 0;
  /* call the virtual method in vtable slot +0x10 of the sub-object at record+8, no arguments (inferred) */
  result = (**(int (**)())(**(int **)(record + 8) + 0x10))();
  *(undefined4 *)(record + 0xc) = result;
  return 0;
}


