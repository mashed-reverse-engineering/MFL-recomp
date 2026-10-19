#include "mfl_tier2.h"
/*008dae40*/ extern undefined4 g_piz_8dae40;
#define PIZ_DWORD_COUNT (0x102b)
/* FUN_004c6740 */
/* Copies PIZ_DWORD_COUNT 32-bit words from the global g_piz_8dae40 into dst (inferred: a snapshot of that global's block).
 * dst: destination buffer, must hold PIZ_DWORD_COUNT words; NULL is ignored. Returns nothing. */

typedef struct { undefined4 d[0x102b]; } Dw102b;
extern void FUN_004c6740(undefined4 *dst);


