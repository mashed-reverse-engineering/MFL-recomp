#include "mfl_tier2.h"
/*008dae40*/ extern undefined4 g_piz_8dae40;
#define PIZ_DWORD_COUNT (0x102b)
/* FUN_004c6740 */
/* Copies PIZ_DWORD_COUNT 32-bit words from the global g_piz_8dae40 into dst (inferred: a snapshot of that global's block).
 * dst: destination buffer, must hold PIZ_DWORD_COUNT words; NULL is ignored. Returns nothing. */

void FUN_004c6740(undefined4 *dst)

{
  int remaining;
  undefined4 *src;

  if (dst != (undefined4 *)0x0) {
    src = &g_piz_8dae40;
    /* copy loop: one word per iteration, counting remaining down to 0 */
    for (remaining = PIZ_DWORD_COUNT; remaining != 0; remaining = remaining + -1) {
      *dst = *src;
      src = src + 1;
      dst = dst + 1;
    }
  }
  return;
}


