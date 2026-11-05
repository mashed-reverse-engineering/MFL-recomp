/* rwa_stream_set_stream_pos_005b8980 (0x005b8980): streams (_rwaStreamSetStreamPos): copy the new stream position (a marker name or a 16-byte position) into the stream and queue the seek command */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*004b34a0*/ extern char *__cdecl _strncpy(char *dst, const char *src, size_t n);
/*005c1b00*/ extern void rwa_unit246d50_005c1b00(void *commands, void *pos);      /* queue the set-position command */
/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */

typedef struct RwaStreamPosValue { RwUInt32 w[4]; } RwaStreamPosValue;
typedef struct RwaStreamPosDesc {      /* 0x10 */
  void *value;      /* POS_MARKER: the marker name; POS_VALUE: a RwaStreamPosValue */
  RwUInt32 type;
  RwUInt32 w8, wc;
} RwaStreamPosDesc;
typedef struct RwaStreamPos {
  RwaStreamPosDesc *desc;
  RwUInt32 w4;
} RwaStreamPos;

#define POS_MARKER 1
#define POS_VALUE  2
#define STREAM_POS(s)       ((RwaStreamPos *)((s) + 0x28))
#define STREAM_POS_DESC(s)  ((RwaStreamPosDesc *)((s) + 0x30))
#define STREAM_POS_NAME(s)  ((s) + 0x40)      /* char[0x80] */
#define STREAM_POS_VALUE(s) ((RwaStreamPosValue *)((s) + 0xdc))
#define STREAM_COMMANDS(s)  ((s) + 0x154)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_set_stream_pos_005b8980(char *stream, RwaStreamPos *pos)
{
  RwaStreamPosDesc *d;

  *STREAM_POS(stream) = *pos;
  *STREAM_POS_DESC(stream) = *pos->desc;
  STREAM_POS(stream)->desc = STREAM_POS_DESC(stream);
  d = pos->desc;
  if (d->type == POS_MARKER)
    _strncpy(STREAM_POS_NAME(stream), (const char *)d->value, 0x80);
  else if (d->type == POS_VALUE)
    *STREAM_POS_VALUE(stream) = *(RwaStreamPosValue *)d->value;
  WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff);
  rwa_unit246d50_005c1b00(STREAM_COMMANDS(stream), STREAM_POS(stream));
  ReleaseSemaphore(rwastream_objDefRegistered, 1, 0);
}
