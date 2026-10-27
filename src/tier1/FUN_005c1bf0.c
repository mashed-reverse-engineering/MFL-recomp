#include "mfl_tier2.h"
/* FUN_005c1bf0 */

#define RECORD_FLAG_DIRTY (0x80) /* inferred: flag bit in record word 0 */
/* Stores new_value in word 1 of the record and sets the flag bit 0x80 in word 0.
   record: pointer to a 2-word record (word 0 = flags, word 1 = value).
   new_value: value to store in word 1. */
void FUN_005c1bf0(uint *record,uint new_value)
{
  record[1] = new_value;
  *record = *record | RECORD_FLAG_DIRTY;
  return;
}
