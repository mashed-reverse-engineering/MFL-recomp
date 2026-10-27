#include "mfl_tier2.h"
/* FUN_005be6b0 */
/* Clears a three-dword record to zero: record[0], record[1] and record[2].
   record: pointer to the 12-byte record (inferred: meaning of the three fields unknown). */
void FUN_005be6b0(undefined4 *record)
{
  record[2] = 0;
  record[1] = 0;
  *record = 0;
  return;
}
