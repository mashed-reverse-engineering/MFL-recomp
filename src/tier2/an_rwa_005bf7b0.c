/* rwa_async_file_read_005bf7b0 (0x005bf7b0): RWA async file i/o: queue a read request for the i/o thread (waits until the request's previous operation is done) */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;

typedef void (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);
typedef struct RwaAsyncRequest {
  char *file;            /* +0x00 */
  void *fp;              /* +0x04 */
  RwUInt32 w8;
  RwUInt32 op;           /* +0x0c: 1 open, 3 seek, 4 read */
  RwUInt32 w10;
  void *buffer;          /* +0x14 */
  RwUInt32 size;         /* +0x18 */
  RwUInt32 w1c;
  RwaAsyncCallback callback;      /* +0x20 */
  void *data;            /* +0x24 */
  void *done;            /* +0x28: semaphore, signalled when the request is finished */
  RwUInt32 w2c;
  RwaLink link;          /* +0x30: in the i/o queue */
} RwaAsyncRequest;

#define ASYNC_READ 4
/*007dbc50*/ extern void *g_data_7dbc50;      /* the queue's lock */
/*007dbc48*/ extern RwaLink *g_data_7dbc48;   /* the queue */
/*007dbc54*/ extern int g_data_7dbc54;        /* requests in the queue */
/*007dbc58*/ extern void *g_data_7dbc58;      /* wakes the i/o thread */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_async_file_read_005bf7b0(RwaAsyncRequest *req, void *buffer, RwUInt32 size, RwaAsyncCallback callback, void *data)
{
  WaitForSingleObject(req->done, 0xffffffff);
  req->buffer = buffer;
  req->op = ASYNC_READ;
  req->size = size;
  req->callback = callback;
  req->data = data;
  WaitForSingleObject(g_data_7dbc50, 0xffffffff);
  req->link.prev = &g_data_7dbc48;      /* insert at the head */
  req->link.next = g_data_7dbc48;
  g_data_7dbc48->prev = (RwaLink **)&req->link;
  g_data_7dbc48 = &req->link;
  g_data_7dbc54++;
  ReleaseSemaphore(g_data_7dbc50, 1, 0);
  if (g_data_7dbc54 == 1)      /* the queue was empty: wake the thread */
    ReleaseSemaphore(g_data_7dbc58, 1, 0);
}
