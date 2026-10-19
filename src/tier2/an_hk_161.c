#include "mfl_tier2.h"
/* FUN_005c1f60 */

/* Initialises an 8-byte record (inferred): clears the three header bytes at
 * offsets 0-2, stores `kind` at offset 3 and `value` as a 32-bit word at offset 4.
 * record: destination buffer (at least 8 bytes).
 * value:  32-bit payload stored at record + 4 (inferred).
 * kind:   byte stored at record + 3 (inferred).
 * Returns nothing. */
void FUN_005c1f60(undefined1 *record,undefined4 value,undefined1 kind)

{
  record[1] = 0;
  record[2] = 0;
  *record = 0;
  record[3] = kind;
  *(undefined4 *)(record + 4) = value;
  return;
}


