#include "mfl_tier2.h"
#define E_POINTER (0x80004003)
/* Writes 1 (TRUE) through result_flag and returns S_OK (0). */
/* FUN_004b16a0
 * Writes 1 (TRUE) through result_flag and returns S_OK (0).
 * unused_arg is never read (inferred).
 * Returns E_POINTER without writing if result_flag is NULL.
 */
undefined4 __stdcall FUN_004b16a0(char *unused_arg, undefined4 *result_flag)
{
  if (result_flag == (undefined4 *)0x0) {
    return E_POINTER;
  }
  *result_flag = 1;
  return 0;
}
