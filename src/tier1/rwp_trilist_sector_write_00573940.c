/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwStream RwStream;
typedef struct RwpSectorBlob { char pad[4]; int size; } RwpSectorBlob;
extern RwStream *RwStreamWriteInt32(RwStream *stream, void *data, int size);
/* World sector plugin stream write callback: writes the sector's blob (its first word is data, the second the size); returns the stream. (Rwp library) */
RwStream *rwp_trilist_sector_write_00573940(RwStream *stream, int length, char *object, int offset)
{
  RwpSectorBlob *blob = *(RwpSectorBlob **)(offset + object);
  if (blob != 0) {
    int size = blob->size;
    return RwStreamWriteInt32(stream, blob, size);
  }
  return stream;
}
