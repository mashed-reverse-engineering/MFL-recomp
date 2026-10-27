#include "mfl_tier2.h"
/*008dae40*/ extern undefined4 g_piz_8dae40;
/* FUN_004c6740 */
/* Copies PIZ_DWORD_COUNT 32-bit words from the global g_piz_8dae40 into dst (inferred: a snapshot of that global's block).
 * dst: destination buffer, must hold PIZ_DWORD_COUNT words; NULL is ignored. Returns nothing. */

typedef struct { undefined4 d[0x102b]; } Dw102b;
void FUN_004c6740(undefined4 *dst)
{
  if (dst != (undefined4 *)0x0) {
    *(Dw102b *)dst = *(Dw102b *)&g_piz_8dae40;
  }
  return;
}
