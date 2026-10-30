/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpSectorBlob { char pad[4]; int size; } RwpSectorBlob;
/* World sector plugin stream size callback: size of the sector blob, 0 when there is none. (Rwp library) */
int rwp_trilist_sector_size_005739e0(char *object, int offset)
{
  int size = 0;
  RwpSectorBlob *blob = *(RwpSectorBlob **)(offset + object);
  if (blob != 0) size = blob->size;
  return size;
}
