#include "mfl_tier2.h"
/* FUN_00571830 */
/* Stores `value` as a 32-bit word at byte offset 8 of `object` (field meaning unknown; inferred: a plain setter).
   Parameters: object = base pointer of the structure, value = 4-byte value to store. Returns nothing. */

char * __cdecl FUN_00571830(char *object,undefined4 value)
{
  *(undefined4 *)(object + 8) = value;
  return object;
}
