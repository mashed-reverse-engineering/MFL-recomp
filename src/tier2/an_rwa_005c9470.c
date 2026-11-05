/* rwa_stream_file_request_queue_get_005c9470 (0x005c9470): stream file requests (_rwaStreamFileRequestQueueGet): take the oldest request off the ring */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaStreamFileRequest { RwUInt32 w[5]; } RwaStreamFileRequest;      /* 0x14 */

typedef struct RwaStreamFileRequestQueue {
  RwaStreamFileRequest *requests;      /* +0x00 ring */
  unsigned short capacity;             /* +0x04 */
  unsigned short tail;                 /* +0x06 next put */
  unsigned short head;                 /* +0x08 next get */
  unsigned short full;                 /* +0x0a tail caught up with head */
  void *lock;                          /* +0x0c semaphore */
} RwaStreamFileRequestQueue;

/* Returns 1 with the request in *out, 0 when the queue is empty. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_file_request_queue_get_005c9470(RwaStreamFileRequestQueue *queue, RwaStreamFileRequest *out)
{
  unsigned short next;

  WaitForSingleObject(queue->lock, (RwUInt32)-1);
  if (queue->tail == queue->head && !queue->full) {
    ReleaseSemaphore(queue->lock, 1, 0);
    return 0;
  }
  {
    RwaStreamFileRequest *r = &queue->requests[queue->head];

    out->w[1] = r->w[1];      /* the original's copy order (matters only if out overlaps the ring) */
    out->w[0] = r->w[0];
    out->w[4] = r->w[4];
    out->w[3] = r->w[3];
    out->w[2] = r->w[2];
  }
  next = queue->head + 1;
  if (next == queue->capacity)
    next = 0;
  queue->full = 0;
  queue->head = next;
  ReleaseSemaphore(queue->lock, 1, 0);
  return 1;
}
