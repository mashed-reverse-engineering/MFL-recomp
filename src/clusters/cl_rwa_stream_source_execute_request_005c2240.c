// cluster-entries: rwa_stream_source_execute_request_005c2240
/* Installed cluster rwa_stream_source_execute_request_005c2240 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_stream_source_execute_request_005c2240.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream source (_RwaStreamSourceExecuteRequest, register function: source in eax, entered through the generated thunk): when no
   request is in flight (+0x188 & 0x2ff8 clear), start the pending one of +0xe8: 4 = read (+0xdc buffer, +0xe0 size, in-flight
   flag 8), else 1 = seek (+0xe4 position, in-flight flag 0x20); both complete through the read callback 5c34f0. */
#include "mfl_tier2.h"

/*005bbd70*/ extern void FUN_005bbd70(void *stream, void *buffer, RwUInt32 size, void *callback, void *data);      /* read */
/*005bbb70*/ extern int FUN_005bbb70(char *stream, RwUInt32 pos, void *callback, void *data);      /* _rwaRwStreamSeek */
/*005c34f0*/ extern void gap_5c34f0(void);      /* the read callback (its original address: what the stream calls back) */

#define SRC_PENDING(s) (*(RwUInt32 *)((s) + 0xe8))      /* 4: read pending, 1: seek pending */
#define SRC_STATUS(s) (*(RwUInt32 *)((s) + 0x188))      /* 8: read in flight, 0x20: seek in flight */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void rwa_stream_source_execute_request_005c2240_n(char *source)
{
  RwUInt32 pending;

  if (SRC_STATUS(source) & 0x2ff8)
    return;
  pending = SRC_PENDING(source);
  if (pending & 4) {
    SRC_PENDING(source) = pending & ~4u;
    SRC_STATUS(source) |= 8;
    FUN_005bbd70(source + 0x28, *(void **)(source + 0xdc), *(RwUInt32 *)(source + 0xe0), gap_5c34f0, source);
  } else if (pending & 1) {
    SRC_PENDING(source) = pending & ~1u;
    SRC_STATUS(source) |= 0x20;
    FUN_005bbb70(source + 0x28, *(RwUInt32 *)(source + 0xe4), gap_5c34f0, source);
  }
}

/* entry thunk at the original address of rwa_stream_source_execute_request_005c2240: register inputs (eax) -> natural C rwa_stream_source_execute_request_005c2240_n; ret 0 */
__declspec(naked) void rwa_stream_source_execute_request_005c2240(void)
{
    __asm {
        push eax
        call rwa_stream_source_execute_request_005c2240_n
        add esp, 4
        ret
    }
}
