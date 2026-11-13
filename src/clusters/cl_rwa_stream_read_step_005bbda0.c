// cluster-entries: rwa_stream_read_step_005bbda0 gap_5bbe60
/* Installed cluster rwa_stream_read_step_005bbda0 (tools/cluster_install.py from output/cluster_pilot_auw7/rwa_stream_read_step_005bbda0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) stream reads: MFL.exe 0x005bbda0 (register function: async result in eax, read context in
   ecx, async op in ebx) and the async callback gap_5bbe60 that drives it.  A read context is a stream object (buffered read at +4,
   see _rwaRwStreamCacheReadAsync) with the fields below.  Written from the MFL.exe disassembly (objdump). */
#include "mfl_tier2.h"

typedef void (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);

typedef struct RwaReadInfo { RwUInt32 size, offset, w8; } RwaReadInfo;
typedef struct RwaStreamReadCtx {
  char *file;                      /* +0x00 object whose +0x0c is the async file */
  char bufRead[0x3c];              /* +0x04 the buffered read */
  RwUInt32 state;                  /* +0x40 1 open, 2 header read, 3 data read */
  RwaAsyncCallback callback;       /* +0x44 */
  void *data;                      /* +0x48 */
  RwUInt32 expected;               /* +0x4c */
  RwaReadInfo *info;               /* +0x50 (also the header buffer) */
  RwUInt32 seeking;                /* +0x54 */
} RwaStreamReadCtx;
#define ASYNC_FILE(ctx) (*(void **)((ctx)->file + 0xc))

/*005bf930*/ extern RwInt32 FUN_005bf930(void *file);      /* current file position */
/*005bf850*/ extern void rwa_async_queue_read_005bf850(void *file, RwUInt32 offset, RwaAsyncCallback callback, void *data);
/*005bb8a0*/ extern void FUN_005bb8a0(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);
void gap_5bbe60(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);

#define RESULT_FAILED 4

/* One step of the header read: failed -> report 0 bytes with result 4; after a seek -> read the 12-byte header; header says the
   expected size -> report 12 bytes; else seek to the header's offset (from the current position) first. */
static void _rwaRwStreamCacheReadAsync_r(char * p0, RwUInt32 p1, char * p2, RwaAsyncCallback p3, void * p4);
extern void _rwaRwStreamCacheReadAsync(void);
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void rwa_stream_read_step_005bbda0_n(RwUInt32 result, RwaStreamReadCtx *ctx, RwUInt32 op)
{
  RwInt32 pos;

  if (result == RESULT_FAILED)
    ctx->state = 0;
  else {
    if (ctx->seeking) {
      ctx->seeking = 0;
      _rwaRwStreamCacheReadAsync_r((char *)ctx, 0xc, (char *)ctx->info, gap_5bbe60, ctx);
      return;
    }
    if (ctx->info->size == ctx->expected) {
      ctx->state = 0;
      ctx->callback(ASYNC_FILE(ctx), 0xc, result, op, ctx->data);
      return;
    }
    ctx->seeking = 1;
    pos = FUN_005bf930(ASYNC_FILE(ctx));
    if (pos >= 0) {
      rwa_async_queue_read_005bf850(ASYNC_FILE(ctx), ctx->info->offset + pos, FUN_005bb8a0, ctx);
      return;
    }      /* the position query failed: reported like a failed read (the state stays) */
  }
  ctx->info->size = 0;
  ctx->info->offset = 0;
  ctx->info->w8 = 0;
  ctx->callback(ASYNC_FILE(ctx), 0, RESULT_FAILED, op, ctx->data);
}

/* The async callback of a read context (data): state 1 = the open finished (the file is stored), 3 = data: both go on to the
   context's callback with its data; 2 = a header step. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void gap_5bbe60(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data)
{
  RwaStreamReadCtx *ctx = data;

  switch (ctx->state) {
  case 1:
    *(void **)(ctx->file + 0xc) = file;
    ctx->callback(file, done, result, op, ctx->data);
    break;
  case 2:
    rwa_stream_read_step_005bbda0_n(result, ctx, op);
    break;
  case 3:
    ctx->callback(file, done, result, op, ctx->data);
    break;
  }
}

/* _rwaRwStreamCacheReadAsync_r: C -> original _rwaRwStreamCacheReadAsync (registers edi,eax,s0,s1,s2) */
__declspec(naked) void _rwaRwStreamCacheReadAsync_r(char * p0, RwUInt32 p1, char * p2, RwaAsyncCallback p3, void * p4)
{
    __asm {
        push edi
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        mov edi, dword ptr [esp + 20]
        mov eax, dword ptr [esp + 24]
        call _rwaRwStreamCacheReadAsync
        add esp, 12
        pop edi
        ret
    }
}

/* entry thunk at the original address of rwa_stream_read_step_005bbda0: register inputs (eax,ecx,ebx) -> natural C rwa_stream_read_step_005bbda0_n; ret 0 */
__declspec(naked) void rwa_stream_read_step_005bbda0(void)
{
    __asm {
        push ebx
        push ecx
        push eax
        call rwa_stream_read_step_005bbda0_n
        add esp, 12
        ret
    }
}
