/* RenderWare Audio (Criterion Software) asynchronous file i/o thread, MFL.exe 0x005bf970 (started by 0x005bfc50 through
 * _RwaThreadCreate / _RwaThreadStart, stopped by 0x005bfcf0).  Written from the MFL.exe disassembly (capstone).
 * The thread takes the oldest request of the queue at 0x7dbc48 (ring through the request's link at +0x30), performs it on the
 * request's file (open / seek / read) and reports to the request's callback.  It sleeps on the "idle" semaphore 0x7dbc58 while the
 * queue is empty and ends when the running flag 0x7dbc98 is cleared (it then signals 0x7dbc58 once more for the closer).
 * Thread side effects: tools/shadow_exclude.txt line; HS spec s_rwa_au_w8.py ends the loop through the flag. */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef void (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);

typedef struct RwaAsyncLink { struct RwaAsyncLink *next, *prev; } RwaAsyncLink;
typedef struct RwaAsyncFile {
  void *self;                      /* +0x00 */
  void *fp;                        /* +0x04 RwFopen handle */
  RwUInt32 position;               /* +0x08 */
  char pad0c[0x1c];
  void *done;                      /* +0x28 semaphore released after every request */
} RwaAsyncFile;
typedef struct RwaAsyncRequest {
  RwaAsyncFile *file;              /* +0x00 */
  void *fp;                        /* +0x04 the file's fp when the request was queued (seek / read use this copy) */
  RwUInt32 position;               /* +0x08 (of the request block itself; cleared by an open) */
  RwUInt32 op;                     /* +0x0c 1 open, 3 seek, 4 read */
  const char *name;                /* +0x10 open: file name */
  void *buffer;                    /* +0x14 read: destination */
  RwUInt32 size;                   /* +0x18 read: bytes */
  RwUInt32 offset;                 /* +0x1c seek: absolute position */
  RwaAsyncCallback callback;       /* +0x20 */
  void *data;                      /* +0x24 */
  char pad28[4];
  RwUInt32 taken;                  /* +0x2c set when the thread takes the request */
  RwaAsyncLink link;               /* +0x30 */
} RwaAsyncRequest;

#define OP_OPEN 1
#define OP_SEEK 3
#define OP_READ 4
#define RESULT_FAILED 4

/*00560d10*/ extern void *RwFopen_00560d10(const char *name, const char *mode);
/* (symbols.txt also gives the names RwFwrite_00560eb0 / RwFseek_00560f10 to the jump thunks 0x4c54a0 / 0x4c54b0: own aliases) */
/*00560eb0*/ extern RwUInt32 rwa_file_read_00560eb0(void *buffer, RwUInt32 size, RwUInt32 count, void *fp);      /* fread-shaped */
/*00560f10*/ extern RwInt32 rwa_file_seek_00560f10(void *fp, RwInt32 offset, RwInt32 origin);
/*005bed80*/ extern void RwaFreeListFree(void *list, void *block);

/*005e0450*/ extern const char g_piz_5e0450[];      /* the open mode */
/*007dbc44*/ extern void *g_data_7dbc44;            /* callback lock */
/*007dbc4c*/ extern RwaAsyncLink *g_data_7dbc4c;    /* queue head (0x7dbc48) .prev = the oldest request */
/*007dbc50*/ extern void *g_data_7dbc50;            /* queue lock */
/*007dbc54*/ extern RwUInt32 g_data_7dbc54;         /* queued requests */
/*007dbc58*/ extern void *g_data_7dbc58;            /* idle: released when a request is queued (and at the thread's end) */
/*007dbc5c*/ extern void *g_data_7dbc5c;            /* released after every request */
/*007dbc74*/ extern undefined g_data_7dbc74;        /* the file free list */
/*007dbc98*/ extern RwUInt32 g_data_7dbc98;         /* running */

/* the thread procedure (run by gap_5c43d0); always 0 */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *rwa_async_io_thread_005bf970(void *arg)
{
  RwaAsyncLink *link;
  RwaAsyncRequest *req;
  RwaAsyncFile *file;
  RwUInt32 done, result, code;

  do {
    WaitForSingleObject(g_data_7dbc50, 0xffffffff);
    if (g_data_7dbc54 == 0) {
      ReleaseSemaphore(g_data_7dbc50, 1, 0);
      WaitForSingleObject(g_data_7dbc58, 0xffffffff);
    } else
      ReleaseSemaphore(g_data_7dbc50, 1, 0);
    if (!g_data_7dbc98)
      continue;
    result = 0;
    WaitForSingleObject(g_data_7dbc50, 0xffffffff);
    if (g_data_7dbc54 == 0) {
      ReleaseSemaphore(g_data_7dbc50, 1, 0);
      continue;
    }
    link = g_data_7dbc4c;
    req = (RwaAsyncRequest *)((char *)link - 0x30);
    req->taken = 1;
    link->prev->next = link->next;
    link->next->prev = link->prev;
    link->prev = 0;
    link->next = 0;
    g_data_7dbc54--;
    ReleaseSemaphore(g_data_7dbc50, 1, 0);
    file = req->file;
    done = 0;
    switch (req->op) {
    case OP_OPEN:
      file->fp = RwFopen_00560d10(req->name, g_piz_5e0450);
      req->position = 0;
      if (!file->fp) {      /* the file object is given back; no lock, no semaphores */
        req->callback(file, 0, RESULT_FAILED, 2, req->data);
        CloseHandle(file->done);
        RwaFreeListFree(&g_data_7dbc74, file);
        continue;
      }
      break;
    case OP_SEEK:
      if (rwa_file_seek_00560f10(req->fp, req->offset, 0) == 0) {
        done = req->offset;
        file->position = done;
      } else
        result = RESULT_FAILED;
      break;
    case OP_READ:
      done = rwa_file_read_00560eb0(req->buffer, 1, req->size, req->fp);
      if (done != req->size)
        result = RESULT_FAILED;
      file->position += done;
      break;
    }
    WaitForSingleObject(g_data_7dbc44, 0xffffffff);
    ReleaseSemaphore(g_data_7dbc5c, 1, 0);
    ReleaseSemaphore(file->done, 1, 0);
    if (req->callback) {
      switch (req->op) {
      case OP_OPEN: code = 2; break;
      case OP_SEEK: code = 4; break;
      case OP_READ: code = 5; break;
      default: code = 1; break;
      }
      req->callback(file, done, result, code, req->data);
    }
    ReleaseSemaphore(g_data_7dbc44, 1, 0);
  } while (g_data_7dbc98);
  ReleaseSemaphore(g_data_7dbc58, 1, 0);
  return 0;
}
