#include "mfl_tier2.h"
/* FUN_004b0580 */
#define STREAM_OBJECT_OFS (0xc)
#define STREAM_EXPECTED_OFS (0x10)
#define STREAM_CURSOR_OFS (0x4)
#define STREAM_END_OFS (0x8)
#define VTABLE_STATE_SLOT_OFS (0x14)
#define ERR_STATE_MISMATCH (0x80040203)
/*
 * Reserves `size` bytes from a cursor/end buffer (inferred: a stream or ring buffer).
 *   stream: pointer to the buffer header; cursor at +4, end at +8, expected state at +0x10,
 *           and a sub-object at +0xc whose vtable slot 5 returns the current state.
 *   size:   number of bytes to reserve.
 * Returns 0 on success, 1 if fewer than `size` bytes remain, ERR_STATE_MISMATCH if the
 * sub-object's state differs from the expected one.
 */
undefined4 __stdcall FUN_004b0580(char *stream, uint size)
{
  int current_state;

  /* Query the sub-object's state through its vtable and compare with the expected value */
  int *obj = *(int **)(stream + STREAM_OBJECT_OFS);
  current_state = (*(int (__fastcall **)(int *))(*obj + VTABLE_STATE_SLOT_OFS))(obj);
  if (current_state != *(int *)(stream + STREAM_EXPECTED_OFS)) {
    return ERR_STATE_MISMATCH;
  }
  /* Not enough room left between cursor and end: report it without moving the cursor */
  if (size > (uint)(*(int *)(stream + STREAM_END_OFS) - *(int *)(stream + STREAM_CURSOR_OFS))) {
    return 1;
  }
  /* Advance the cursor past the reserved bytes */
  *(uint *)(stream + STREAM_CURSOR_OFS) = *(int *)(stream + STREAM_CURSOR_OFS) + size;
  return 0;
}
