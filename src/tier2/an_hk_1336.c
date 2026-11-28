#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* gap_5da500 */

/* software mixer wave definition, destroy hook (def +0x10, rwa_soft_wave_def_create_005da590): clear the owner (+0x50) and free
   the wave's sample block (+0x4c, allocated by rwa_soft_wave_alloc_data_005da4d0).  The original rewrites its argument slot with
   the block and tail-jumps to the RWA free (0x5bf120); the old body here called it with no argument, so the slot still held the
   WAVE, a free-list element: free() of an interior pointer (PAUSE W16, heap corruption 0xc0000374 on Windows when the pause menu
   stops the music stream; VC7's heap and Wine's ignore it). */

/*005bf120*/ extern void FUN_005bf120(void *block);      /* RwaFree */
/* TIER2: unvalidated 2026-11-28 authority=diffemu */
void gap_5da500(char *wave)
{
  *(undefined4 *)(wave + 0x50) = 0;
  FUN_005bf120(*(void **)(wave + 0x4c));
}
