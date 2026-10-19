#include "mfl_tier2.h"
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*006a07c4*/ extern undefined4 g_course_6a07c4;
/*006a07c8*/ extern undefined4 g_course_6a07c8;
/*006a07cc*/ extern undefined4 g_course_6a07cc;
/*006a07d0*/ extern undefined4 g_course_6a07d0;
/*008c5728*/ extern undefined4 g_course_dump_8c5728;
/* course_dump_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Course dump callback 0 (inferred: gathers course/track data for the course dump).
 * game_state: base of the game state block; every field is read at a byte offset from it.
 * Copies the word at +0xc110 into g_course_dump_8c5728. Then, for four clumps stored at
 * +0xc154, +0xc158, +0xc15c and +0xc128, takes each clump's first atomic and passes it to
 * func_0x004a5d50 together with a per-slot word and a block address inside game_state;
 * the result goes to g_course_6a07c4, g_course_6a07c8, g_course_6a07cc and g_course_6a07d0.
 * Returns nothing. */
void course_dump_cb0(char *game_state)

{
  undefined4 atomic;

  g_course_dump_8c5728 = *(undefined4 *)(game_state + 0xc110);
  /* One block per clump slot: first atomic of the clump -> func_0x004a5d50 -> global. */
  atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc154));
  g_course_6a07c4 =
       func_0x004a5d50(atomic,*(undefined4 *)(game_state + 0xc010),game_state + 0x3000,
                       *(undefined4 *)(game_state + 0xc9e0),*(undefined4 *)(game_state + 0xc9e4));
  atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc158));
  g_course_6a07c8 =
       func_0x004a5d50(atomic,*(undefined4 *)(game_state + 0xc014),game_state + 0x3c00,
                       *(undefined4 *)(game_state + 0xc9e0),*(undefined4 *)(game_state + 0xc9e4));
  atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc15c));
  g_course_6a07cc =
       func_0x004a5d50(atomic,*(undefined4 *)(game_state + 0xc018),game_state + 0x4800,
                       *(undefined4 *)(game_state + 0xc9e0),*(undefined4 *)(game_state + 0xc9e4));
  atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc128));
  g_course_6a07d0 =
       func_0x004a5d50(atomic,*(undefined4 *)(game_state + 0xc000),game_state,
                       *(undefined4 *)(game_state + 0xc9e0),*(undefined4 *)(game_state + 0xc9e4));
  return;
}


