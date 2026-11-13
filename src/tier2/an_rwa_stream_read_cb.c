/* RenderWare Audio (Criterion Software) buffered stream reads: the completion callback of the asynchronous file reads, MFL.exe
 * 0x005bb8a0.  Written from the MFL.exe disassembly (objdump).  A read request is served from one shared 2 KB sector cache when it
 * can; aligned bulk reads go straight into the caller's buffer.  When a request is done its callback runs and the next pending
 * request (the cache owner's first, if it is queued) is continued in the same call.  The function passes ITSELF as the callback
 * of the reads it starts. */
#include <string.h>
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef void (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);

typedef struct RwaPendLink { struct RwaPendLink *next, *prev; } RwaPendLink;

typedef struct RwaBufRead {        /* at stream + 4 */
  RwaPendLink link;                /* +0x00 on the pending list (next 0: not queued) */
  RwUInt32 state;                  /* +0x08 1 done, 3 busy, 4 reading, 5 seeking */
  RwUInt32 position;               /* +0x0c file position of the next byte */
  RwUInt32 w10;                    /* +0x10 handed to the callback */
  RwUInt32 remaining;              /* +0x14 bytes still to deliver */
  char *dest;                      /* +0x18 */
  void *file;                      /* +0x1c async file */
  RwUInt32 w20;
  RwUInt32 w24;                    /* +0x24 bytes of the last op-2 completion */
  RwUInt32 readOffset;             /* +0x28 file offset of the read in flight */
  RwUInt32 requested;              /* +0x2c its size */
  char *buffer;                    /* +0x30 its target: the sector cache or dest */
  RwaAsyncCallback callback;       /* +0x34 */
  void *data;                      /* +0x38 */
} RwaBufRead;

#define SECTOR 0x800
#define ST_DONE 1
#define ST_BUSY 3
#define ST_READING 4
#define ST_SEEKING 5

/*007db770*/ extern void *g_data_7db770;          /* lock of the cache and the requests */
/*007daf60*/ extern char g_data_7daf60[SECTOR];   /* the sector cache */
/*007db760*/ extern RwaBufRead *g_data_7db760;    /* request that filled the cache */
/*007db764*/ extern void *g_data_7db764;          /* file of the cached sector */
/*007db768*/ extern RwUInt32 g_data_7db768;       /* file offset of the cached sector (-1: none) */
/*007db76c*/ extern RwUInt32 g_data_7db76c;       /* valid bytes in the cache */
/*007daf4c*/ extern void *g_data_7daf4c;          /* lock of the pending list */
/*007daf50*/ extern RwaPendLink g_data_7daf50;    /* pending requests */
/*007daf48*/ extern void *g_data_7daf48;          /* signalled when the pending list runs empty */
/*007daf58*/ extern RwaBufRead *g_data_7daf58;    /* request being continued */
/*005bf930*/ extern RwUInt32 FUN_005bf930(void *file);      /* current file position */
/*005bf7b0*/ extern void rwa_async_file_read_005bf7b0(void *file, void *buffer, RwUInt32 size, RwaAsyncCallback callback, void *data);
/*005bf850*/ extern void rwa_async_queue_read_005bf850(void *file, RwUInt32 offset, RwaAsyncCallback callback, void *data);
/*005bb8a0*/ extern void FUN_005bb8a0(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);      /* it passes itself as the read callback; declared for the gate's shadow build (shadowgen2 renames the definition) */

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void FUN_005bb8a0(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data)
{
  RwaBufRead *r = (RwaBufRead *)((char *)data + 4);
  RwUInt32 cached, position, sector, n, len;
  RwaPendLink *l;

  if (op == 2)
    r->w24 = done;
  if (op != 1 && op != 5) {
    if (r->state != ST_SEEKING) {
      r->callback(file, done, result, op, r->data);
      return;
    }
    r->state = ST_BUSY;
  }
  WaitForSingleObject(g_data_7db770, 0xffffffff);
  for (;;) {
    cached = g_data_7db76c;
    if (r->state == ST_READING) {
      r->state = ST_BUSY;
      if (result == 4 || done != r->requested)
        goto finished;
      if (r->buffer == g_data_7daf60) {      /* a sector arrived in the cache */
        cached = done;
        g_data_7db76c = done;
        g_data_7db760 = r;
        g_data_7db764 = file;
        g_data_7db768 = r->readOffset;
      } else {      /* a bulk read straight into dest */
        r->dest += done;
        r->position += done;
        r->remaining -= done;
        cached = g_data_7db76c;
      }
    }
    position = r->position;
    sector = position & ~(SECTOR - 1);
    if (g_data_7db760 == r && g_data_7db768 == sector) {      /* serve what the cache holds */
      n = cached - (position - g_data_7db768);
      if (n >= r->remaining)
        n = r->remaining;
      memcpy(r->dest, g_data_7daf60 + (position - g_data_7db768), n);
      r->position += n;
      position += n;
      r->remaining -= n;
      r->dest += n;
      sector = position & ~(SECTOR - 1);
      result = 2;
    }
    if (r->remaining) {      /* start the next read */
      if (!((mfl_uptr)r->dest & 3) && position == sector && r->remaining >= SECTOR) {
        len = r->remaining & ~(SECTOR - 1);
        r->buffer = r->dest;
        if (r == g_data_7db760) {
          g_data_7db760 = 0;
          g_data_7db764 = 0;
        }
      } else {
        g_data_7db760 = 0;
        g_data_7db764 = 0;
        g_data_7db768 = 0xffffffff;
        len = SECTOR;
        r->buffer = g_data_7daf60;
      }
      if (FUN_005bf930(r->file) == sector) {
        r->state = ST_READING;
        r->readOffset = sector;
        r->requested = len;
        rwa_async_file_read_005bf7b0(r->file, r->buffer, len, FUN_005bb8a0, data);
      } else {
        r->state = ST_SEEKING;
        rwa_async_queue_read_005bf850(r->file, sector, FUN_005bb8a0, data);
      }
      break;
    }
  finished:
    r->state = ST_DONE;
    r->callback(r->file, r->w10, result, op, r->data);
    WaitForSingleObject(g_data_7daf4c, 0xffffffff);
    r = 0;
    if (g_data_7daf50.next != &g_data_7daf50) {      /* continue a pending request: the cache owner's if it waits */
      r = g_data_7db760;
      l = (r && r->link.next) ? &r->link : g_data_7daf50.next;
      r = (RwaBufRead *)l;
      data = (char *)l - 4;
      l->prev->next = l->next;
      l->next->prev = l->prev;
      l->next = 0;
      r->state = ST_BUSY;
    } else
      ReleaseSemaphore(g_data_7daf48, 1, 0);
    ReleaseSemaphore(g_data_7daf4c, 1, 0);
    g_data_7daf58 = r;
    if (!r)
      break;
  }
  ReleaseSemaphore(g_data_7db770, 1, 0);
}

/*005c4370*/ extern RwBool _rwaSemaphorePoll(void **semaphore);      /* takes it when it is free (no wait): 1, else 0 */
/*005bf8f0*/ extern void FUN_005bf8f0(void *file);      /* cancel the file's pending i/o */

/* MFL.exe 0x005bbc70: cancels the stream's buffered read.  The read being continued right now (g_data_7daf58, only while the
   pending list is in use) is cancelled at its file and reported as failed (result 2) after the locks are released; a queued one
   just leaves the pending list. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void _rwaRwStreamCancel(void *data)
{
  RwaBufRead *r = (RwaBufRead *)((char *)data + 4);
  RwBool cancelled = 0;

  WaitForSingleObject(g_data_7db770, 0xffffffff);
  WaitForSingleObject(g_data_7daf4c, 0xffffffff);
  if (!_rwaSemaphorePoll(&g_data_7daf48)) {
    if (g_data_7daf58 == r) {
      FUN_005bf8f0(r->file);
      cancelled = 1;
      r->state = ST_BUSY;
      r->remaining = 0;
    }
    if (g_data_7db760 == r) {
      g_data_7db760 = 0;
      g_data_7db764 = 0;
    }
  } else
    ReleaseSemaphore(g_data_7daf48, 1, 0);
  if (r->link.next) {
    r->link.prev->next = r->link.next;
    r->link.next->prev = r->link.prev;
    r->link.next = 0;
    r->state = ST_DONE;
  }
  ReleaseSemaphore(g_data_7daf4c, 1, 0);
  ReleaseSemaphore(g_data_7db770, 1, 0);
  if (cancelled)
    FUN_005bb8a0(r->file, 0, 2, 1, data);
}
