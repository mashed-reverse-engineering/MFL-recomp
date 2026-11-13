/* RenderWare Audio (Criterion Software) DirectShow codec voice: one step of the decoder output, MFL.exe 0x005cee90
 * (the symbol table calls it _rwaDXAudioCodecSetOutputConnection, which does not fit what it does).  Written from the MFL.exe disassembly (objdump).
 * Codec object (owner +0x20) layout used here: +0x6f8 sample renderer, +0x6fc owner word handed to the sample slots,
 * +0x72c output sample rate, +0x748 output packet list (bytes +0x749 / +0x74b = its read / write index),
 * +0x788 decoded sample slot, +0x79c output packet slot. */
#include "mfl_tier2.h"

typedef struct RwaCodecSample {      /* 20-byte slot: a media sample or a stream packet being copied */
  RwUInt32 w0;
  RwUInt32 position, length;
  void *object;                      /* +0x788: IMediaSample; +0x79c: stream packet (+0xc -> record, +8 = start position in samples) */
  RwUInt32 w10;
} RwaCodecSample;

typedef RwInt32 (__stdcall *MediaSampleGetTime)(void *sample, __int64 *start, __int64 *end);      /* IMediaSample vtable slot 5 */

/*005c1f80*/ extern void *RwaStreamPacketListAlloc(void *list);
/*005cf210*/ extern void _RwaDXAudioCodecSampleInit(RwaCodecSample *slot);

/* These four are int3 trap rows of the AU2 DirectShow block (tools/delink/rw_trap.txt): called by their original address, not by
   name (trapcheck refuses a C reference to a filled row).  The original calls them the same way, so if this function ever ran
   both would hit the same trap. */
#define _rwaDSRendererGetSample ((void *(*)(void *renderer))0x005d06c0)
#define _RwaDXAudioCodecSampleAssign ((void (*)(RwaCodecSample *slot, void *object, RwInt32 kind, RwUInt32 owner))0x005cf230)
#define _RwaDXAudioCodecSampleRelease ((void (*)(RwaCodecSample *slot))0x005cf2b0)
#define _RwaDXAudioCodecSampleIsComplete ((void (*)(RwaCodecSample *packet, RwaCodecSample *sample))0x005cf300)

/* Fetches a decoded media sample and an output packet when the slots are empty, stamps a fresh packet with the sample's start time
   (100 ns units -> samples at the output rate), lets the callee copy sample data into the packet, and returns the packet once it
   is full (its slot is then reset); a used-up sample is released.  Returns 0 while no packet is complete, and at once when the
   packet list is empty and no packet is in work. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void *rwa_dxaudio_codec_fill_packet_005cee90(char *voice)
{
  char *codec = *(char **)(voice + 0x20);
  RwaCodecSample *sample = (RwaCodecSample *)(codec + 0x788), *packet = (RwaCodecSample *)(codec + 0x79c);
  void *complete = 0;
  __int64 start, end;

  if (*(RwUInt8 *)(codec + 0x749) == *(RwUInt8 *)(codec + 0x74b) && !packet->object) return 0;
  if (!sample->object) {
    _RwaDXAudioCodecSampleAssign(sample, _rwaDSRendererGetSample(*(void **)(codec + 0x6f8)), 1, *(RwUInt32 *)(codec + 0x6fc));
    if (!sample->object) return 0;
  }
  if (!packet->object)
    _RwaDXAudioCodecSampleAssign(packet, RwaStreamPacketListAlloc(codec + 0x748), 2, *(RwUInt32 *)(codec + 0x6fc));
  if (packet->position == 0 && (*(MediaSampleGetTime **)sample->object)[5](sample->object, &start, &end) >= 0)
    *(RwUInt32 *)(*(char **)((char *)packet->object + 0xc) + 8) = (RwUInt32)(start * *(RwUInt32 *)(codec + 0x72c) / 10000000);
  _RwaDXAudioCodecSampleIsComplete(packet, sample);
  if (packet->object && packet->position == packet->length) {
    complete = packet->object;
    _RwaDXAudioCodecSampleInit(packet);
  }
  if (sample->object && sample->position == sample->length) _RwaDXAudioCodecSampleRelease(sample);
  return complete;
}
