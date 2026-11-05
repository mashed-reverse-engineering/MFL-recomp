/* rwa_stream_file_request_queue_add_005c93d0 (0x005c93d0): stream file source (_rwaStreamFileRequestQueueAdd): append a read request to the ring buffer; 0 when it is full */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);

typedef struct RwaFileRequest {      /* 0x14 */
  RwUInt32 a5;          /* +0x00 */
  void *buffer;         /* +0x04 */
  RwUInt32 bytes;       /* +0x08 */
  RwUInt32 a4;          /* +0x0c */
  RwUInt32 a6;          /* +0x10 */
} RwaFileRequest;

typedef struct RwaFileRequestQueue {
  RwaFileRequest *items;
  unsigned short capacity;      /* +0x04 */
  unsigned short tail;          /* +0x06: next free slot */
  unsigned short head;          /* +0x08 */
  unsigned short full;          /* +0x0a */
  void *lock;                   /* +0x0c: semaphore */
} RwaFileRequestQueue;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_file_request_queue_add_005c93d0(RwaFileRequestQueue *q, void *buffer, RwUInt32 bytes, RwUInt32 a4, RwUInt32 a5, RwUInt32 a6)
{
  RwaFileRequest *req;
  RwUInt32 next;

  WaitForSingleObject(q->lock, 0xffffffff);
  if (q->tail == q->head && q->full) {
    ReleaseSemaphore(q->lock, 1, 0);
    return 0;
  }
  req = &q->items[q->tail];
  req->buffer = buffer;
  req->bytes = bytes;
  req->a6 = a6;
  req->a4 = a4;
  req->a5 = a5;
  next = q->tail + 1;
  if (next == q->capacity)
    next = 0;
  if (next == q->head)
    q->full = 1;
  q->tail = (unsigned short)next;
  ReleaseSemaphore(q->lock, 1, 0);
  return 1;
}
