// cluster-entries: rwa_stream_source_free_005c20a0 rwa_stream_source_destroy_005c2170 rwa_stream_source_init_005c40d0 rwa_stream_source_read_callback_005c34f0
/* Installed cluster rwa_stream_source_free_005c20a0 (tools/cluster_install.py from output/cluster_pilot_l2/rwa_stream_source_free_005c20a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA stream source (objects/rwastreamsource.c): releasing a source's buffers (register function, source in esi), its stack
   wrapper, the source init (opens the .rws stream) and the asynchronous read callback that walks the stream header (chunk type,
   wave header, uuid / name match, header buffer) and then fills the frame ring.  Cluster: rwa_stream_source_free_005c20a0 is
   entered through the generated esi thunk; the request executor 5c2240 (eax) is called through a generated adapter. */
#include "mfl_tier2.h"

typedef struct RwaRequestQueue {      /* 0x10 bytes */
  RwUInt32 w0;
  unsigned short s4, head, tail, count;      /* +0x04 .. +0x0b */
  RwUInt32 wc;
} RwaRequestQueue;

typedef void *(*RwaAllocFunc)(RwUInt32 size, void *data);
typedef void (*RwaFreeFunc)(void *block, void *data);
typedef void (*RwaNotifyFunc)(void *source, RwUInt32 status, void *data);

typedef struct RwaStreamSource {
  RwUInt32 status;                  /* +0x000 */
  char rwStream[0x24];              /* +0x004 */
  char *streamHandle;               /* +0x028 the RwaRwStream at +0x28 (its first word: the stream) */
  char stream2c[0x54];
  RwUInt32 chunkType;               /* +0x080 */
  RwUInt32 chunkLength;             /* +0x084 */
  RwUInt32 w088;
  RwaRequestQueue queue;            /* +0x08c */
  char queueStorage[0x3c];          /* +0x09c */
  RwUInt32 request[4];              /* +0x0d8 */
  RwUInt32 pending;                 /* +0x0e8 */
  RwUInt32 readCallback;            /* +0x0ec */
  RwUInt32 wf0[4];                  /* +0x0f0 */
  RwUInt32 w100;
  RwaAllocFunc alloc;               /* +0x104 */
  RwaFreeFunc free;                 /* +0x108 */
  void *allocData;                  /* +0x10c */
  RwaNotifyFunc notify;             /* +0x110 */
  void *notifyData;                 /* +0x114 */
  char *header;                     /* +0x118 the wave header buffer */
  RwUInt32 hdr[5];                  /* +0x11c chunk header: [0] header bytes, [2] uuid bytes, [4] name bytes */
  void *uuid;                       /* +0x130 unique id {uuid, name, ..} */
  char *name;                       /* +0x134 */
  RwUInt32 w138;
  RwUInt32 w13c, w140;
  char *frameBuffer;                /* +0x144 */
  char *frames;                     /* +0x148 frame descriptors (0x14 bytes, +0x0c, +0x10 state) */
  RwUInt32 w14c;
  RwUInt32 writeFrame;              /* +0x150 */
  RwUInt32 w154, w158, w15c;
  RwUInt32 frameBufferSize;         /* +0x160 */
  RwUInt32 frameCount;              /* +0x164 */
  char *markerBuffer;               /* +0x168 */
  char *markers;                    /* +0x16c */
  RwUInt32 markerBufferSize;        /* +0x170 */
  RwUInt32 w174, w178;
  RwUInt32 markerCount;             /* +0x17c */
  RwUInt32 w180, w184;
  RwUInt32 flags;                   /* +0x188 */
  void *semaphore;                  /* +0x18c */
  RwUInt32 time;                    /* +0x190 */
  char *p194, *p198;
} RwaStreamSource;

typedef struct RwaStreamSourceParams {
  RwaAllocFunc alloc;               /* +0x00 */
  RwaFreeFunc free;                 /* +0x04 */
  void *allocData;                  /* +0x08 */
  RwUInt32 w0c;
  char *fileName;                   /* +0x10 */
  RwUInt32 kind;                    /* +0x14 1: a file */
  void *uuid;                       /* +0x18 */
  char *name;                       /* +0x1c */
  RwUInt32 w20, w24;
  RwUInt32 frameBufferSize;         /* +0x28 */
  char *frameBuffer;                /* +0x2c */
  RwUInt32 markerCount;             /* +0x30 */
  RwUInt32 markerBufferSize;        /* +0x34 */
  char *markerBuffer;               /* +0x38 */
  RwaNotifyFunc notify;             /* +0x3c */
  void *notifyData;                 /* +0x40 */
} RwaStreamSourceParams;

/*005bbf20*/ extern void FUN_005bbf20(void *stream);      /* close the RwaRwStream */
/*005bf230*/ extern void FUN_005bf230(void *block);      /* RWA aligned free */
/*005bf0e0*/ extern void *FUN_005bf0e0(RwUInt32 size, RwUInt32 hint);      /* RWA aligned malloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005c93c0*/ extern void FUN_005c93c0(RwaRequestQueue *queue);      /* _rwaStreamFileRequestQueueDestroy */
/*005c9380*/ extern int FUN_005c9380(RwaRequestQueue *queue, void *storage, RwUInt32 n);      /* _rwaStreamFileRequestQueueCreate */
/*005c9470*/ extern int FUN_005c9470(RwaRequestQueue *queue, RwUInt32 *request);      /* _rwaStreamFileRequestQueueGet */
/*005be6b0*/ extern void FUN_005be6b0(void *id);      /* _rwaUniqueIDInitialize */
/*005be7d0*/ extern void FUN_005be7d0(void *id, char *name);      /* set the name */
/*005be810*/ extern void FUN_005be810(void *id, void *uuid);      /* set the uuid */
/*005be600*/ extern int FUN_005be600(const void *a, const void *b);      /* RwaUUIDCompare (0: equal) */
/*005c4340*/ extern int FUN_005c4340(void **semaphore, RwUInt32 initial, RwUInt32 max);      /* RwaSemaphoreCreate */
/*005bf940*/ extern char *FUN_005bf940(char *fileName);      /* _rwaGetFilenameExt */
/*005bf930*/ extern RwUInt32 FUN_005bf930(RwUInt32 a);
/*005bbeb0*/ extern void FUN_005bbeb0(void *rwStream, void *stream, int a3, int a4, char *fileName, void *callback, void *data);      /* _rwaRwStreamOpen */
/*005bbf50*/ extern void rwa_unit2449c0_005bbf50(char *handle, void *stream, RwUInt32 type, RwUInt32 *chunkType, void *callback, void *data);      /* find a chunk */
/*005bbd70*/ extern void FUN_005bbd70(void *stream, void *buffer, RwUInt32 size, void *callback, void *data);      /* read */
/*005bb6b0*/ extern RwUInt32 rwa_unit2449c0_005bb6b0(void *stream, void *callback, void *data);      /* _rwaRwStreamGetPos */
/*005bbb70*/ extern void FUN_005bbb70(void *stream, RwUInt32 pos, void *callback, void *data);      /* _rwaRwStreamSeek */
/*005bbfa0*/ extern void rwa_unit2449c0_005bbfa0(void *stream, RwUInt32 *chunkType, void *callback, void *data);      /* read a chunk header */
/*005c9530*/ extern int gap_5c9530(char *header);      /* is the wave header supported */
/*005c2bd0*/ extern RwUInt32 FUN_005c2bd0(RwaStreamSource *s);      /* _RwaStreamSourceSeekReadCallback */
/*005c2c90*/ extern void FUN_005c2c90(RwaStreamSource *s, RwUInt32 a, RwUInt32 b);      /* __rwaStreamSourceSeek */
/*005c30f0*/ extern void FUN_005c30f0(RwaStreamSource *s);      /* __rwaStreamSourceProcess */
/*005c2040*/ extern void gap_5c2040(void);      /* default alloc */
/*005c1c20*/ extern void gap_5c1c20(void);      /* default free */
/*005c34f0*/ extern void gap_5c34f0(void);      /* the read callback (its original address: what the stream calls back) */
/*005dd088*/ __declspec(dllimport) int __stdcall CloseHandle(void *handle);
/*005dd090*/ __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 ms);
/*005dd094*/ __declspec(dllimport) int __stdcall ReleaseSemaphore(void *handle, long count, long *previous);
/*007d2128*/ extern char *RwEngineInstance;
/*005f7c34*/ extern char g_rwa_ext_rws[];      /* "rws" */
/*005f7c2c*/ extern char g_rwa_ext_rws_1[];    /* "rws;1" */

#define RWA_HINT 0x3080e
#define CALLBACK ((void *)gap_5c34f0)
#define STREAM(s) ((void *)&(s)->streamHandle)
#define FRAME_STATE(s, i) (*(RwUInt32 *)((s)->frames + (i) * 0x14 + 0x10))
#define STRCMP(a, b) (*(int (**)(const char *, const char *))(RwEngineInstance + 0xe8))(a, b)
#define STRICMP(a, b) (*(int (**)(const char *, const char *))(RwEngineInstance + 0xf0))(a, b)

static void _RwaStreamSourceExecuteRequest_r(RwaStreamSource * p0);
extern void _RwaStreamSourceExecuteRequest(void);
/* _RwaStreamSourceFree: once; buffers the source did not get from its creator, the header, the semaphore, the request queue */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_free_005c20a0_n(RwaStreamSource *s)
{
  if (s->flags & 0x1000)
    return;
  if (s->flags & 4)
    FUN_005bbf20(STREAM(s));
  if (s->frameBuffer && !(s->flags & 1)) {
    FUN_005bf230(s->frameBuffer);
    s->frameBuffer = 0;
  }
  if (s->markerBuffer && !(s->flags & 2)) {
    FUN_005bf230(s->markerBuffer);
    s->markerBuffer = 0;
  }
  if (s->header) {
    s->free(s->header, s->allocData);
    s->header = 0;
  }
  if (s->semaphore)
    CloseHandle(s->semaphore);
  FUN_005c93c0(&s->queue);
  s->flags |= 0x1000;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_destroy_005c2170(RwaStreamSource *s)
{
  rwa_stream_source_free_005c20a0_n(s);
}

/* Source init: callbacks and buffers from the parameters (own buffers allocated when only a size is given), request queue,
   semaphore; a .rws file is opened asynchronously (the read callback then walks its header).  1: opened. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_source_init_005c40d0(RwaStreamSource *s, RwaStreamSourceParams *p)
{
  char *ext, *c;

  s->flags = 0;
  s->status = 0;
  s->wf0[0] = 0;
  s->wf0[1] = 0;
  s->wf0[2] = 0;
  s->wf0[3] = 0;
  s->header = 0;
  s->alloc = p->alloc;
  s->free = p->free;
  s->allocData = p->allocData;
  s->notify = p->notify;
  s->notifyData = p->notifyData;
  s->w184 = p->w20;
  s->w180 = p->w24;
  FUN_005be6b0(&s->uuid);
  FUN_005be7d0(&s->uuid, p->name);
  FUN_005be810(&s->uuid, p->uuid);
  s->markerBuffer = p->markerBuffer;
  s->markerBufferSize = p->markerBufferSize;
  s->frameBuffer = p->frameBuffer;
  s->w154 = 0;
  s->w158 = 0;
  s->writeFrame = 0;
  s->w15c = 0;
  s->w178 = 0;
  s->frameBufferSize = p->frameBufferSize;
  s->w14c = 0;
  s->frames = 0;
  s->w174 = 0;
  s->readCallback = 0;
  s->markerCount = p->markerCount;
  s->markers = 0;
  if (!p->alloc)
    p->alloc = (RwaAllocFunc)gap_5c2040;
  if (!p->free)
    p->free = (RwaFreeFunc)gap_5c1c20;
  if (!FUN_005c9380(&s->queue, s->queueStorage, 3))
    return 0;
  if (s->frameBuffer)
    s->flags |= 1;
  else if (s->frameBufferSize && (s->frameBuffer = FUN_005bf0e0(s->frameBufferSize, RWA_HINT)) == 0)
    goto fail;
  if (s->markerBuffer)
    s->flags |= 2;
  else if (s->markerBufferSize && (s->markerBuffer = FUN_005bf0e0(s->markerBufferSize, RWA_HINT)) == 0)
    goto fail;
  if (!FUN_005c4340(&s->semaphore, 1, 1))
    goto fail;
  if (p->kind != 1)
    return 0;
  ext = FUN_005bf940(p->fileName);
  s->flags |= 0x10;
  s->chunkType = 0x80d;
  for (c = p->fileName; *c; c++)
    if (*c == '/')
      *c = '\\';
  if (STRICMP(ext, g_rwa_ext_rws) && STRICMP(ext, g_rwa_ext_rws_1))
    return 0;
  FUN_005bbeb0(s->rwStream, STREAM(s), 2, 1, p->fileName, CALLBACK, s);
  return 1;
fail:
  rwa_stream_source_free_005c20a0_n(s);
  return 0;
}

/* The read callback of the source's stream (status 4: failed): one step of the header walk per completed request, then frame
   ring updates; the next queued request is executed when the stream is idle. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_read_callback_005c34f0(char *request, RwUInt32 bytes, RwUInt32 status, RwUInt32 a4, RwaStreamSource *s)
{
  RwUInt32 fs, size, n, i;
  char *uuid, *name;
  int match;

  if (status == 4)
    s->status = *(RwUInt32 *)(request + 0x24);
  if (s->flags & 8) {      /* frames read */
    fs = *(RwUInt32 *)(s->header + 0x20);
    s->flags &= ~8u;
    WaitForSingleObject(s->semaphore, 0xffffffff);
    if (s->flags & 0x2000) {
      ReleaseSemaphore(s->semaphore, 1, 0);
      return;
    }
    while (bytes) {
      if (FRAME_STATE(s, s->writeFrame) & 0x10) {
        FRAME_STATE(s, s->writeFrame) = 0x12;
        if (s->markerBuffer && s->readCallback != 0xffffffff)
          FRAME_STATE(s, s->writeFrame) |= 4;
      } else
        FRAME_STATE(s, s->writeFrame) = 2;
      i = s->writeFrame + 1;
      if (i == s->frameCount)
        i = 0;
      s->writeFrame = i;
      bytes -= fs;
    }
    ReleaseSemaphore(s->semaphore, 1, 0);
    goto next;
  }
  if (s->flags & 0x20) {      /* seek done */
    s->flags &= ~0x20u;
    goto next;
  }
  if (s->flags & 0x10) {      /* stream open: find the first chunk */
    s->flags &= ~0x10u;
    if (status == 4) {
      if (!s->notify)
        goto release;
      s->notify(s, 6, s->notifyData);
      goto release;
    }
    s->flags |= 4;
    s->flags |= 0x40;
    rwa_unit2449c0_005bbf50(s->streamHandle, STREAM(s), s->chunkType, &s->chunkType, CALLBACK, s);
    return;
  }
  if (s->flags & 0x40) {      /* chunk found: the stream chunk, then the header chunk */
    s->flags &= ~0x40u;
    if (status == 4 || (s->chunkType != 0x80e && s->chunkType != 0x80d))
      goto fail;
    if (s->chunkType == 0x80d) {
      s->flags |= 0x40;
      rwa_unit2449c0_005bbf50(s->streamHandle, STREAM(s), 0x80e, &s->chunkType, CALLBACK, s);
      return;
    }
    s->flags |= 0x80;
    FUN_005bbd70(STREAM(s), s->hdr, 0x14, CALLBACK, s);
    return;
  }
  if (s->flags & 0x80) {      /* chunk header read: read the wave header, or the uuid / name to match */
    s->flags &= ~0x80u;
    size = 0;
    if (status == 4)
      goto failed5;
    if (!s->name && !s->uuid) {
      size = s->hdr[0];
      s->flags |= 0x100;
    } else {
      if (s->hdr[2])
        size = 0x10;
      if (s->hdr[4])
        size += s->hdr[4];
      s->flags |= 0x800;
    }
    s->header = s->alloc(size, s->allocData);
    if (!s->header) {
      if (!s->notify)
        goto release;
      s->notify(s, status | 1, s->notifyData);
      goto release;
    }
    FUN_005bbd70(STREAM(s), s->header, size, CALLBACK, s);
    return;
  }
  if (s->flags & 0x800) {      /* uuid / name read */
    s->flags &= ~0x800u;
    uuid = 0;
    name = 0;
    match = 0;
    if (status == 4) {
      s->free(s->header, s->allocData);
      s->header = 0;
      goto failed5;
    }
    if (s->hdr[2])
      uuid = s->header;
    if (s->hdr[4])
      name = s->header + s->hdr[2];
    if (uuid && s->uuid && !FUN_005be600(uuid, s->uuid))
      match = 1;
    if (name && s->name && !STRCMP(name, s->name))
      match = 1;
    s->free(s->header, s->allocData);
    s->header = 0;
    if (match) {      /* this one: read its header */
      s->flags |= 0x80;
      FUN_005be6b0(&s->uuid);
      FUN_005bbb70(STREAM(s), rwa_unit2449c0_005bb6b0(STREAM(s), CALLBACK, s) - s->hdr[4] - s->hdr[2], CALLBACK, s);
      return;
    }
    s->flags |= 0x10;      /* not this one: skip to the next */
    s->chunkType = 0x80d;
    n = rwa_unit2449c0_005bb6b0(STREAM(s), CALLBACK, s);
    FUN_005bbb70(STREAM(s), n + (s->chunkLength - s->hdr[4] - s->hdr[2]) - 0x14, CALLBACK, s);
    return;
  }
  if (s->flags & 0x100) {      /* wave header read */
    s->flags &= ~0x100u;
    if (status == 4)
      goto failed5;
    if (!gap_5c9530(s->header)) {
      s->free(s->header, s->allocData);
      s->header = 0;
      goto fail;
    }
    s->flags |= 0x200;
    n = rwa_unit2449c0_005bb6b0(STREAM(s), CALLBACK, s);
    FUN_005bbb70(STREAM(s), n + (s->chunkLength - s->hdr[0]) - 0x14, CALLBACK, s);
    return;
  }
  if (s->flags & 0x200) {      /* at the data chunk: read its header */
    s->flags &= ~0x200u;
    if (status == 4) {
      s->free(s->header, s->allocData);
      s->header = 0;
      goto failed5;
    }
    s->flags |= 0x400;
    rwa_unit2449c0_005bbfa0(STREAM(s), &s->chunkType, CALLBACK, s);
    return;
  }
  if (!(s->flags & 0x400))
    goto next;
  /* data chunk header read: frame and marker buffers, the frame ring, start */
  fs = *(RwUInt32 *)(s->header + 0x20);
  s->flags &= ~0x400u;
  if (status == 4 || s->chunkType != 0x80f) {
    s->free(s->header, s->allocData);
    s->header = 0;
    goto fail;
  }
  if (!s->frameBufferSize) {
    size = (fs + 0x38) * (s->w180 ? s->w180 : 2);
    s->frameBuffer = FUN_005bf0e0(size, RWA_HINT);
    if (!s->frameBuffer)
      goto fail;
    s->frameBufferSize = size;
  }
  if (!s->markerBufferSize && s->markerCount) {
    size = (fs + 0x14) * s->markerCount;
    s->markerBuffer = FUN_005bf0e0(size, RWA_HINT);
    if (!s->markerBuffer)
      goto fail;
    s->markerBufferSize = size;
  }
  n = s->markerBufferSize / (fs + 0x14);
  s->frameCount = s->frameBufferSize / (fs + 0x38);
  s->frameBufferSize = s->frameCount * fs;
  s->frames = s->frameBuffer + s->frameBufferSize;
  s->p194 = s->frames + s->frameCount * 0x14;
  s->p198 = s->p194 + s->frameCount * 0x10;
  s->markerCount = n;
  s->markerBufferSize = n * fs;
  if (!s->markerBufferSize) {
    if (!(s->flags & 2) && s->markerBuffer)
      FUN_005bf120(s->markerBuffer);
    s->markerBuffer = 0;
  }
  s->markers = s->markerBuffer + s->markerBufferSize;
  if (!s->w184) {
    s->w184 = s->frameCount >> 1;
    if (!s->w184)
      s->w184 = 1;
  }
  if (s->w184 >= s->frameCount)
    s->w184 = s->frameCount;
  if (!s->w180) {
    s->w180 = s->frameCount >> 1;
    if (!s->w180)
      s->w180 = 1;
  }
  if (s->w180 >= s->frameCount)
    s->w180 = s->frameCount;
  for (i = 0; i < s->markerCount; i++) {
    *(RwUInt32 *)(s->markers + i * 0x14 + 0xc) = 0;
    *(RwUInt32 *)(s->markers + i * 0x14 + 0x10) = 1;
  }
  for (i = 0; i < s->frameCount; i++) {
    *(RwUInt32 *)(s->p198 + i * 0x14) = 0;
    *(RwUInt32 *)(s->frames + i * 0x14 + 0xc) = 0;
    FRAME_STATE(s, i) = 1;
  }
  s->w13c = s->w140 = *(RwUInt32 *)(s->header + 0x10);
  s->readCallback = FUN_005c2bd0(s);
  s->flags |= 0x8000;
  FUN_005c2c90(s, 0, 0);
  FUN_005c30f0(s);
  if (s->notify)
    s->notify(s, 0, s->notifyData);
  return;

next:      /* the next request when the stream is idle */
  if (s->flags & 0x20000) {
    s->time = FUN_005bf930(*(RwUInt32 *)(s->streamHandle + 0xc));
    return;
  }
  if (s->queue.head == s->queue.tail && s->queue.count == 0 && !s->pending)
    return;
  if (!s->pending && !FUN_005c9470(&s->queue, s->request))
    return;
  _RwaStreamSourceExecuteRequest_r(s);
  return;

failed5:
  if (!s->notify)
    goto release;
  s->notify(s, 5, s->notifyData);
  goto release;
fail:
  if (s->notify)
    s->notify(s, status | 1, s->notifyData);
release:
  rwa_stream_source_free_005c20a0_n(s);
}

/* _RwaStreamSourceExecuteRequest_r: C -> original _RwaStreamSourceExecuteRequest (registers eax) */
__declspec(naked) void _RwaStreamSourceExecuteRequest_r(RwaStreamSource * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call _RwaStreamSourceExecuteRequest
        ret
    }
}

/* entry thunk at the original address of rwa_stream_source_free_005c20a0: register inputs (esi) -> natural C rwa_stream_source_free_005c20a0_n; ret 0 */
__declspec(naked) void rwa_stream_source_free_005c20a0(void)
{
    __asm {
        push esi
        call rwa_stream_source_free_005c20a0_n
        add esp, 4
        ret
    }
}
