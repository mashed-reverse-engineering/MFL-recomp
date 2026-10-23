#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*008d8ca0*/ extern undefined g_player_8d8ca0;
/*008d8ca4*/ extern undefined g_player_8d8ca4;
/*008d8d48*/ extern undefined g_player_8d8d48;
/*008d8d50*/ extern undefined g_player_8d8d50;
/*008d8d54*/ extern undefined g_player_8d8d54;

#define MAX_PLAYER_INDEX (3)
#define PLAYER_RECORD_SIZE (0x30c)

/* player_set_precise_position */

/*
 * Stores a precise (float) position for one player and mirrors its integer part.
 * Logs "put precisepos" with the index and position first.
 *
 * player_index: player record index, valid 0..MAX_PLAYER_INDEX (four slots, inferred).
 *               Out-of-range values return 0 without writing. Negative values are not
 *               rejected (signed compare, inferred from decompile).
 * precise_position: float position. Its integer part (truncated, inferred) is written
 *               to four int slots of the record; the float itself goes to the float slot.
 *
 * Record layout, byte offsets from g_player_8d8ca0 for player 0 (inferred):
 *   +0x00, +0x04, +0xb0, +0xb4: int position copies
 *   +0xa8: float precise position
 * Records are PLAYER_RECORD_SIZE bytes apart.
 *
 * Returns 1 on success, 0 if player_index is out of range.
 */
undefined4 player_set_precise_position(int player_index,float precise_position)

{
  undefined4 position_int;

  /* Reject player indexes past the last slot. */
  if (MAX_PLAYER_INDEX < player_index) {
    return 0;
  }
  FID_conflict__wprintf("put precisepos %d,%f\n",player_index,(double)precise_position);
  position_int = ((int)(precise_position));
  /* From here player_index holds the byte offset of that player's record. */
  player_index = player_index * PLAYER_RECORD_SIZE;
  /* Write the integer part to the four int slots of the record. */
  *(undefined4 *)((char *)&g_player_8d8ca0 + player_index) = position_int;
  *(undefined4 *)((char *)&g_player_8d8ca4 + player_index) = position_int;
  *(undefined4 *)((char *)&g_player_8d8d50 + player_index) = position_int;
  *(undefined4 *)((char *)&g_player_8d8d54 + player_index) = position_int;
  /* Write the full float position. */
  *(float *)((char *)&g_player_8d8d48 + player_index) = precise_position;
  return 1;
}


