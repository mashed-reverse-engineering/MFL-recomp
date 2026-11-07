/* rwa_rwstream_seek_005bbb70 (0x005bbb70): RwStream sound source (_rwaRwStreamSeek): remember the seek target and completion callback, then queue an async read of the 2 KB sector that holds POS */
#include "mfl_tier2.h"

/*005bf850*/ extern int rwa_async_queue_read_005bf850(void *file, RwUInt32 pos, void *callback, void *data);
/*005bb8a0*/ extern void FUN_005bb8a0(void);      /* the stream's async read completion callback */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
int rwa_rwstream_seek_005bbb70(char *stream, RwUInt32 pos, void *callback, void *data)
{
  *(void **)(stream + 0x44) = callback;
  *(RwUInt32 *)(stream + 0x10) = pos;
  *(void **)(stream + 0x48) = data;
  *(RwUInt32 *)(stream + 0x40) = 3;      /* state: seeking */
  return rwa_async_queue_read_005bf850(*(void **)(*(char **)stream + 0xc), pos & ~0x7ffu, FUN_005bb8a0, stream);
}
