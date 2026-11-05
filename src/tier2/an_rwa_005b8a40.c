/* rwa_stream_get_stream_pos_005b8a40 (0x005b8a40): streams (_rwaStreamGetStreamPos): the current play position as a marker (name / id / index) and an offset (seconds as float, or raw) */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c0820*/ extern RwUInt32 *rwa_stream_container_get_chain_param_005c0820(char *container, char *query);
/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */

typedef struct RwaStreamPosOut {
  RwUInt32 marker;      /* +0x00 */
  RwUInt32 markerType;  /* +0x04: POS_NAME / POS_ID / POS_INDEX */
  RwUInt32 offset;      /* +0x08: float seconds (OFS_SECONDS) or raw (OFS_RAW) */
  RwUInt32 offsetType;  /* +0x0c */
} RwaStreamPosOut;

/* the container's position: { marker index, w4, offset } */
#define POS_NAME  1
#define POS_ID    2
#define POS_INDEX 3
#define OFS_SECONDS 1
#define OFS_RAW     2
#define STREAM_CONTAINER(s) ((s) + 0xec)
#define STREAM_POSQUERY(s)  ((s) + 0x148)
/* the stream's sample rate: stream+0xf8 -> +0x18 -> +0x14 -> word 0 */
#define STREAM_RATE(s) (**(RwUInt32 **)(*(char **)(*(char **)((s) + 0xf8) + 0x18) + 0x14))
/* marker table: stream+0x100 -> wave (+0x5c) -> +0x118 -> +0x10: 32-byte records { id, name, ... } */
#define STREAM_MARKERS(s) (*(RwUInt32 **)(*(char **)(*(char **)((s) + 0x100) + 0x5c + 0x118) + 0x10))

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_get_stream_pos_005b8a40(char *stream, RwaStreamPosOut *pos)
{
  RwUInt32 *cur;

  WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff);
  cur = rwa_stream_container_get_chain_param_005c0820(STREAM_CONTAINER(stream), STREAM_POSQUERY(stream));
  if (!cur) {
    pos->offset = 0;
    pos->marker = 0;
    ReleaseSemaphore(rwastream_objDefRegistered, 1, 0);
    return;
  }
  ReleaseSemaphore(rwastream_objDefRegistered, 1, 0);
  if (pos->offsetType == OFS_SECONDS)
    *(float *)&pos->offset = (float)cur[2] / (float)STREAM_RATE(stream);
  else if (pos->offsetType == OFS_RAW)
    pos->offset = cur[2];
  if (pos->markerType == POS_NAME)
    pos->marker = STREAM_MARKERS(stream)[cur[0] * 8 + 1];
  else if (pos->markerType == POS_ID)
    pos->marker = STREAM_MARKERS(stream)[cur[0] * 8];
  else if (pos->markerType == POS_INDEX)
    pos->marker = cur[0];
}
