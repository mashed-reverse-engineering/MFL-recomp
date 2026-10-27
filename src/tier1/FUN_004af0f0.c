#include "mfl_tier2.h"
/* FUN_004af0f0 */
/* Refreshes two cached values in a context object (inferred).
 * ctx: context struct; ctx+0xc points to an object whose vtable provides the two values.
 * Stores the vtable value at slot 0x14 in ctx+0x10 and slot 0x18 in ctx+8, clears ctx+4 (inferred).
 * Always returns 0. */
undefined4 __stdcall FUN_004af0f0(char *ctx)
{
  undefined4 value;

  /* first cached value: vtable slot 0x14 of ctx->object (inferred) */
  value = (*(int (__fastcall **)(int))(**(int **)(ctx + 0xc) + 0x14))(*(int *)(ctx + 0xc));
  *(undefined4 *)(ctx + 0x10) = value;
  /* second cached value: vtable slot 0x18 of ctx->object (inferred) */
  value = (*(int (__fastcall **)(int))(**(int **)(ctx + 0xc) + 0x18))(*(int *)(ctx + 0xc));
  *(undefined4 *)(ctx + 8) = value;
  *(undefined4 *)(ctx + 4) = 0;
  return 0;
}
