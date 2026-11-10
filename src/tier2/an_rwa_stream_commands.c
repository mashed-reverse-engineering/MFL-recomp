/* RenderWare Audio (Criterion Software) stream object commands, MFL.exe 0x005b8630-0x005b9020 (rwa.dll unit 23FAD0): module unregister,
 * stream unlink and the lock-wrapped container commands (loop segment, stream gain, sub-stream info).  Written from the MFL.exe
 * disassembly (tools/disasm_va.py); names from the rwa.dll export / similarity rows of 2026-11-03.  Every command takes the stream list
 * lock (rwastream_objDefRegistered, a semaphore) around the container access, as the original does.
 * Stream object layout used here: +0x14 list link, +0xec container, +0x100 media object, +0x148 node set, +0x154 container command. */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;

typedef struct RwaGainParam {      /* stream gain command: track / channel / kind (-1 = all), raw 32-bit gain */
  RwInt32 track, channel, kind;
  RwUInt32 value;
} RwaGainParam;

/*005bfcf0*/ extern void FUN_005bfcf0(void);      /* stream container module close */
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);      /* RwaObjDefDestroy */
/*005c0950*/ extern void rwa_stream_container_release_chains_005c0950(char *container, void *nodeSet);      /* the original pushes the node set too; the callee reads only the container */
/*005c1bf0*/ extern void FUN_005c1bf0(RwUInt32 *record, RwUInt32 newValue);
/*005c1b30*/ extern void _rwaStreamContainerCommandSetStreamGain(void *command, void *set, RwInt32 track, RwInt32 channel, RwInt32 kind, RwUInt32 value);
/*005c1ee0*/ extern void *RwaObjIntGetParamData(void *obj, void *iface, int param);

/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */
/*007dac04*/ extern RwInt32 g_data_7dac04;      /* stream thread quit request */
/*007daca4*/ extern void *g_data_7daca4;        /* stream thread exit semaphore (released by the thread when it quits) */
/*007daca8*/ extern RwInt32 g_data_7daca8;      /* active stream count */
/*007dac48*/ extern RwUInt8 g_data_7dac48[];    /* stream object definition */

#define STREAM_LOCK()   WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff)
#define STREAM_UNLOCK() ReleaseSemaphore(rwastream_objDefRegistered, 1, 0)

/* RwaStreamUnregister: stop the stream thread (quit flag, wait for its exit semaphore), close the lock and that semaphore, close the container
   module and destroy the stream object definition.  Returns the definition destroy result. */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
int rwa_stream_unregister_005b8630(void)
{
  g_data_7dac04 = 1;
  WaitForSingleObject(g_data_7daca4, 0xffffffff);
  CloseHandle(rwastream_objDefRegistered);
  CloseHandle(g_data_7daca4);
  FUN_005bfcf0();
  return FUN_005ba420(g_data_7dac48);
}

/* stream object destroy hook: one active stream less, release the container's chains and take the stream out of its list (under the lock). */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_unlink_005b8740(char *stream)
{
  RwaLink *link = (RwaLink *)(stream + 0x14);

  g_data_7daca8--;
  STREAM_LOCK();
  rwa_stream_container_release_chains_005c0950(stream + 0xec, stream + 0x148);
  if (link->next) {
    *link->prev = link->next;
    link->next->prev = link->prev;
    link->prev = 0;
    link->next = 0;
  }
  return STREAM_UNLOCK();
}

/* _rwaStreamSetLoopSegment (rwa.dll name): store a new value in the container command record. */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_set_loop_segment_005b8c00(char *stream, RwUInt32 value)
{
  STREAM_LOCK();
  FUN_005c1bf0((RwUInt32 *)(stream + 0x154), value);
  return STREAM_UNLOCK();
}

/* _rwaStreamSetStreamGain */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_set_stream_gain_005b8c70(char *stream, const RwaGainParam *gain)
{
  STREAM_LOCK();
  _rwaStreamContainerCommandSetStreamGain(stream + 0x154, stream + 0x148, gain->track, gain->channel, gain->kind, gain->value);
  return STREAM_UNLOCK();
}

/* _rwaStreamGetSubStreamInfo: info[2] = sub-stream index (in), info[3] = its state (out): 1 when the entry's flag 4 is set, else its
   byte +0xd, 0 for an index out of range.  The media's parameter block 2 holds the sub-stream table (+0x14 count, +0x18 0x28-byte entries). */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_get_sub_stream_info_005b8df0(char *stream, RwUInt32 *info)
{
  char *media, *params, *entry, *sub;

  STREAM_LOCK();
  media = *(char **)(stream + 0x100);
  params = RwaObjIntGetParamData(media, *(void **)(media + 0x58), 2);
  if (info[2] < *(RwUInt32 *)(params + 0x14)) {
    entry = *(char **)(params + 0x18) + info[2] * 0x28;
    if (entry) {
      sub = *(char **)(entry + 0x14);
      info[3] = (*(RwUInt8 *)(sub + 0x18) & 4) ? 1 : *(RwUInt8 *)(sub + 0xd);
      return STREAM_UNLOCK();
    }
  }
  info[3] = 0;
  return STREAM_UNLOCK();
}

/* gain parameter handler of the stream object (parameter set callback): same command as SetStreamGain, returns the stream. */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
char *rwa_stream_gain_param_handler_005b8fd0(char *stream, void *param, const RwaGainParam *gain)
{
  STREAM_LOCK();
  _rwaStreamContainerCommandSetStreamGain(stream + 0x154, stream + 0x148, gain->track, gain->channel, gain->kind, gain->value);
  STREAM_UNLOCK();
  return stream;
}
