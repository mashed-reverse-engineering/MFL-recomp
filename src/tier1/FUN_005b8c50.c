#include "mfl_tier2.h"
/* FUN_005b8c50 */
/* Returns the 32-bit value stored at byte offset 0xd70 of `object`.
   Field name and owning struct unknown (inferred: none in DOSSIER). */
undefined4 FUN_005b8c50(char *object)
{
  return *(undefined4 *)(object + 0xd70);
}
