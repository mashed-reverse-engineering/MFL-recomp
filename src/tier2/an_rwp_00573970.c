/* rwp_trilist_sector_read_00573970 (0x00573970): world sector plugin stream read callback of the Rwp triangle list blob */
#include "mfl_tier2.h"

typedef struct RwStream RwStream;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* The sector blob: `data` points just past the 8 byte header when the blob is intact, `size` is its byte size. */
typedef struct RwpSectorBlob {
  char *data;
  int size;
} RwpSectorBlob;
extern RwStream *RwStreamReadInt32(RwStream *stream, void *data, int size);

/* Allocates the blob, reads `length` bytes of 32 bit words into it and keeps it in the sector at `offset`.  A blob whose size field is not
   `length` or whose data pointer is not its own header end is freed again (slot cleared, returns 0).  Returns the stream or 0. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwStream *rwp_trilist_sector_read_00573970(RwStream *stream, int length, char *object, int offset)
{
  RwpSectorBlob *blob = (RwpSectorBlob *)RwMalloc(length, 0x30900);
  RwStream *result;

  *(RwpSectorBlob **)(object + offset) = blob;
  result = RwStreamReadInt32(stream, blob, length);
  if (result != 0 && (blob->size != length || blob->data != (char *)(blob + 1))) {
    RwFree(blob);
    *(RwpSectorBlob **)(object + offset) = 0;
    result = 0;
  }
  return result;
}
