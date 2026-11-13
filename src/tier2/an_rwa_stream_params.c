/* RenderWare Audio (Criterion Software) stream object: segment info query and four parameter handlers, MFL.exe
 * 0x005b8cc0-0x005b9110 (rwa.dll unit 23FAD0; same unit and conventions as an_rwa_stream_commands.c).  Written from the MFL.exe
 * disassembly (objdump) with the decompiler text of the gaps as a cross-check.  Every function takes the stream list lock.
 * Stream object layout used here: +0x100 media object (+0x58 interface, +0x5c segment directory), +0x148 node set,
 * +0x154 container command record.  Media parameter block 2: +0xc a word reported by the getter, +0x10 segment table (0x20-byte
 * entries, +0xc -> first word = segment start in bytes), +0x14 segment count, +0x18 sub-stream table (see below). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaSegmentInfo {
  RwUInt32 segment;       /* in: name / uuid pointer or index (by `kind`); out: index */
  RwUInt32 kind;          /* in: 1 = by name, 2 = by uuid, 3 = by index; out: 3, or 0 when not found */
  RwUInt32 start;         /* out: start in sample frames, or (units == 1) a float in seconds */
  RwUInt32 units;         /* in: 1 = seconds; cleared when not found */
  RwUInt32 count;         /* out: segment count of the media */
} RwaSegmentInfo;

/*005c21e0*/ extern RwUInt32 __rwaStreamSourceGetSegmentIndexByName(void *directory, RwUInt32 name);
/*005c2180*/ extern RwUInt32 __rwaStreamSourceGetSegmentIndexByUUID(void *directory, RwUInt32 uuid);
/*005c1ee0*/ extern void *RwaObjIntGetParamData(void *obj, void *iface, int param);
/*005c1be0*/ extern void rwa_unit246d50_005c1be0(RwUInt32 *record, RwUInt32 value);      /* record word +0x14, change bit 0x40 */
/*005c1bf0*/ extern void FUN_005c1bf0(RwUInt32 *record, RwUInt32 newValue);               /* loop segment: word +4, change bit 0x80 */
/*005c1b10*/ extern void _rwaStreamContainerCommandSetPitch(void *command, RwUInt32 pitch, void *set);

/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */

#define STREAM_LOCK()   WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff)
#define STREAM_UNLOCK() ReleaseSemaphore(rwastream_objDefRegistered, 1, 0)

/* Looks a segment up and reports its index, the segment count and its start: byte offset * the first sub-stream's word +0xc /
   its interleave unit / its channel count (1 when the format's flag 4 is set), divided by the sample rate (first word of the
   format) when seconds are asked for.  An unknown `kind` leaves the index unset in the original: its register then holds the
   stream pointer, which is kept here. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
RwBool _rwaStreamGetSegmentInfo(char *stream, RwaSegmentInfo *info)
{
  char *media, *params, *segment, *sub, *format;
  RwUInt32 index, channels;

  STREAM_LOCK();
  media = *(char **)(stream + 0x100);
  switch (info->kind) {
  case 1: index = __rwaStreamSourceGetSegmentIndexByName(media + 0x5c, info->segment); break;
  case 2: index = __rwaStreamSourceGetSegmentIndexByUUID(media + 0x5c, info->segment); break;
  case 3: index = info->segment; break;
  default: index = (RwUInt32)(mfl_uptr)stream; break;
  }
  if (index != 0xffffffff && (params = RwaObjIntGetParamData(media, *(void **)(media + 0x58), 2)) != 0 &&
      (segment = *(char **)(params + 0x10) + index * 0x20) != 0 && *(RwUInt32 *)(params + 0x14) != 0) {
    info->start = **(RwUInt32 **)(segment + 0xc);
    info->segment = index;
    info->count = *(RwUInt32 *)(params + 0x14);
    info->kind = 3;
    sub = *(char **)(params + 0x18);
    format = *(char **)(sub + 0x14);
    channels = (*(RwUInt8 *)(format + 0x18) & 4) ? 1 : *(RwUInt8 *)(format + 0xd);
    info->start = *(RwUInt32 *)(sub + 0xc) * info->start / *(RwUInt16 *)(sub + 0x1a) / channels;
    if (info->units == 1) {
      format = *(char **)(*(char **)(params + 0x18) + 0x14);
      *(RwReal *)&info->start = (RwReal)info->start / (RwReal)*(RwUInt32 *)format;
    }
  } else {
    info->kind = 0;
    info->units = 0;
    info->count = 0;
  }
  return STREAM_UNLOCK();
}

/* parameter handler: container command word +0x14 */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_stream_command_word_param_handler_005b8ed0(char *stream, void *param, const RwUInt32 *value)
{
  RwUInt32 v = *value;

  STREAM_LOCK();
  rwa_unit246d50_005c1be0((RwUInt32 *)(stream + 0x154), v);
  STREAM_UNLOCK();
  return stream;
}

/* parameter handler: loop segment (same command as rwa_stream_set_loop_segment_005b8c00) */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_stream_loop_segment_param_handler_005b8f30(char *stream, void *param, const RwUInt32 *value)
{
  RwUInt32 v = *value;

  STREAM_LOCK();
  FUN_005c1bf0((RwUInt32 *)(stream + 0x154), v);
  STREAM_UNLOCK();
  return stream;
}

/* parameter handler: pitch */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_stream_pitch_param_handler_005b9050(char *stream, void *param, const RwUInt32 *value)
{
  RwUInt32 v = *value;

  STREAM_LOCK();
  _rwaStreamContainerCommandSetPitch(stream + 0x154, v, stream + 0x148);
  STREAM_UNLOCK();
  return stream;
}

/* parameter getter: word +0xc of the media's parameter block 2 */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
char *rwa_stream_media_word_param_handler_005b90c0(char *stream, void *param, RwUInt32 *value)
{
  char *media, *params;
  RwUInt32 v;

  STREAM_LOCK();
  media = *(char **)(stream + 0x100);
  params = RwaObjIntGetParamData(media, *(void **)(media + 0x58), 2);
  v = *(RwUInt32 *)(params + 0xc);
  STREAM_UNLOCK();
  *value = v;
  return stream;
}
