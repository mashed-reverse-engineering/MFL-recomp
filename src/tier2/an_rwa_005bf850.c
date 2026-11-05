/* rwa_async_queue_read_005bf850 (0x005bf850): async file I/O: queue a read request (state 3) for the I/O thread */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*007dbc48*/ extern RwaLink *g_data_7dbc48;      /* request queue head */
/*007dbc50*/ extern void *g_data_7dbc50;         /* queue lock (semaphore) */
/*007dbc54*/ extern int g_data_7dbc54;           /* queued requests */
/*007dbc58*/ extern void *g_data_7dbc58;         /* wakes the I/O thread when the queue becomes non-empty */

#define REQUEST_READ 3

/* request+0x28: the request's own semaphore (taken until the read completes), +0x0c kind, +0x1c..+0x24 the read
   (buffer, size, callback data), +0x30 queue link. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_async_queue_read_005bf850(char *request, void *buffer, RwUInt32 size, void *data)
{
  RwaLink *link = (RwaLink *)(request + 0x30);

  WaitForSingleObject(*(void **)(request + 0x28), (RwUInt32)-1);
  *(RwUInt32 *)(request + 0x0c) = REQUEST_READ;
  *(void **)(request + 0x1c) = buffer;
  *(RwUInt32 *)(request + 0x20) = size;
  *(void **)(request + 0x24) = data;
  WaitForSingleObject(g_data_7dbc50, (RwUInt32)-1);
  link->prev = (RwaLink *)&g_data_7dbc48;
  link->next = g_data_7dbc48;
  g_data_7dbc48->prev = link;
  g_data_7dbc48 = link;
  g_data_7dbc54++;
  ReleaseSemaphore(g_data_7dbc50, 1, 0);
  if (g_data_7dbc54 == 1)
    ReleaseSemaphore(g_data_7dbc58, 1, 0);
}
