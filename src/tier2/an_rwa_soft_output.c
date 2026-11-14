/* RenderWare Audio (Criterion Software) software output, MFL.exe 0x005d8d00 (the mix callback the DirectX output calls once per
 * period) and 0x005d8f10 (create: buffers, lock semaphore, output object, mixer callback).  Written from the MFL.exe disassembly
 * (tools/disasm_va.py).  These were the rwatap sites AUD_mix / AUD_softOutputInit (tools/rwtap/aud_funcs.txt): our builds can no
 * longer be tapped there by VA (the entry is a jmp into this C). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

void *__cdecl memset(void *dst, int value, unsigned int size);      /* intrinsic (/O2): inline rep stosd / stosb like the original */

typedef struct RwaSoftLink { struct RwaSoftLink *next, *prev; } RwaSoftLink;

typedef struct RwaSoftOutput {      /* at output object + 0x28 */
  RwUInt32 owner_word;              /* +0x00 params[1] */
  void *lock;                       /* +0x04 semaphore */
  RwReal gain;                      /* +0x08 1.0 */
  RwUInt32 w0c;                     /* +0x0c */
  RwUInt32 state;                   /* +0x10 1 */
  void *output;                     /* +0x14 the DirectX output object */
  RwUInt32 rate;                    /* +0x18 samples per second */
  RwaSoftLink voices;               /* +0x1c ring of the voices (link at voice + 0xd0) */
  RwUInt32 flags;                   /* +0x24 bit 0: the output object was created here */
  RwUInt32 period_words;            /* +0x28 period bytes / 4 */
  char *bus_b;                      /* +0x2c */
  char *buffer;                     /* +0x30 one block for the three buses */
  RwUInt32 bus_bytes;               /* +0x34 */
  char *dry;                        /* +0x38 */
  char *reverb;                     /* +0x3c */
  RwUInt32 bus_count;               /* +0x40 2 */
  char **buses;                     /* +0x44 -> bus_b */
} RwaSoftOutput;

typedef struct RwaSoftParams { void *output_given; RwUInt32 owner_word, rate, period_ms; } RwaSoftParams;
typedef struct RwaMixerCallback { void *(*mix)(void *unused, char *mixer); RwUInt32 bytes; RwaSoftOutput *mixer; } RwaMixerCallback;

/*005bb5d0*/ extern RwUInt32 rwa_timer_get_value_005bb5d0(void);
/*005daf60*/ extern void rwa_soft_voice_resample_005daf60(char *voice);
/*005dab50*/ extern void rwa_soft_voice_mix_005dab50(char *voice);
/*005dab20*/ extern void rwa_soft_mixer_bus_to_pcm16_005dab20(char *mixer);
/*005ba5c0*/ extern void *RwaObjDefFindByUUID(void *base, const void *uuid, void *flags);
/*005bac30*/ extern void RwaObjEnum(void *callback, void *data);
/*005bac10*/ extern int rwa_obj_005bac10();      /* enum callback: the first object of the definition in data[1] -> data[0] */
/*005cb1d0*/ extern void *rwa_dxaudio_output_create_005cb1d0(void *a_null, void *b_null, void *c_null, void *arg_d, void *arg_e);
/*005bf130*/ extern char *rwa_calloc_005bf130(RwUInt32 size, RwUInt32 count, RwUInt32 hint);
/*005bf120*/ extern void rwa_free_005bf120(void *mem);
/*005baf80*/ extern void RwaObjDestroy(void *obj, int a_flag, int b_flag);
/*005c4340*/ extern RwBool RwaSemaphoreCreate(void **semaphore, RwInt32 initial, RwInt32 maximum);
/*005cc3a0*/ extern void _rwaDXAudioOutputSetMixerCallback(void *output, RwaMixerCallback *callback);
/*005f835c*/ extern undefined g_sound_5f835c;      /* the DirectX audio output definition uuid */

/* one mix period: clears the dry and reverb buses, stamps every voice with the timer value, resamples and mixes it, then converts
   the buses; returns the mixer's PCM block (+0x40 of the mixer) */
/* TIER2: unvalidated 2026-11-14 authority=static */
char *rwa_soft_output_mix_005d8d00(void *unused, char *mixer)
{
  RwaSoftLink *head = (RwaSoftLink *)(mixer + 0x1c);
  RwaSoftLink *link;
  RwUInt32 now;
  char *link_c;

  memset(*(void **)(mixer + 0x38), 0, *(RwUInt32 *)(mixer + 0x34));
  memset(*(void **)(mixer + 0x3c), 0, *(RwUInt32 *)(mixer + 0x34));
  WaitForSingleObject(*(void **)(mixer + 4), 0xffffffff);
  now = rwa_timer_get_value_005bb5d0();
  for (link = head->next; link != head; link = link->next) {
    link_c = (char *)link;
    *(RwUInt32 *)(link_c - 0x30) = now;
    *(RwUInt32 *)(link_c - 0x40) = *(RwUInt32 *)(link_c - 0x48);
    *(RwUInt32 *)(link_c - 0x3c) = *(RwUInt32 *)(link_c - 0x44);
    rwa_soft_voice_resample_005daf60(link_c - 0xd0);
    rwa_soft_voice_mix_005dab50(link_c - 0xd0);
  }
  ReleaseSemaphore(*(void **)(mixer + 4), 1, 0);
  rwa_soft_mixer_bus_to_pcm16_005dab20(mixer);
  return mixer + 0x40;
}

/* creates the software mixer of output object obj (+0x28) for params; an existing DirectX output is looked up first, else one is
   created (flag bit 0).  obj, or NULL on failure (a created output is destroyed again) */
/* TIER2: unvalidated 2026-11-14 authority=static */
char *rwa_soft_output_create_005d8f10(char *obj, void *unused, RwaSoftParams *params, void *arg_d, void *arg_e)
{
  RwaSoftOutput *s = (RwaSoftOutput *)(obj + 0x28);
  void *found[3];
  RwaMixerCallback cb;
  RwUInt32 period;
  void *def;

  s->flags = 0;
  s->gain = 1.0f;
  s->w0c = 0;
  s->owner_word = params->owner_word;
  s->output = params->output_given;
  s->rate = params->rate;
  s->voices.next = &s->voices;
  s->voices.prev = &s->voices;
  s->state = 1;
  if (!params->output_given) {
    def = RwaObjDefFindByUUID(0, &g_sound_5f835c, 0);
    if (!def)
      return 0;
    found[0] = 0;
    found[1] = def;
    RwaObjEnum(rwa_obj_005bac10, found);
    s->output = found[0];
    if (found[0])
      s->flags &= ~1u;
    else {
      s->output = rwa_dxaudio_output_create_005cb1d0(0, 0, 0, arg_d, arg_e);
      s->flags |= 1;
      if (!s->output)
        return 0;
    }
    s->rate = *(RwUInt32 *)(*(char **)((char *)s->output + 0x20) + 4);
  }
  period = ((params->period_ms * s->rate * 4) / 1000 + 3) & ~3u;
  s->bus_bytes = (period >> 1) << 2;
  s->period_words = period >> 2;
  s->buffer = rwa_calloc_005bf130((s->bus_bytes + period) * 2, 1, 0x3080c);
  if (!s->buffer) {
    if (s->flags & 1) {
      RwaObjDestroy(s->output, 0, 0);
      s->flags &= ~1u;
    }
    return 0;
  }
  s->bus_b = s->buffer + period;
  s->dry = s->buffer + 2 * period;
  s->reverb = s->dry + s->bus_bytes;
  s->buses = &s->bus_b;
  s->bus_count = 2;
  if (!RwaSemaphoreCreate(&s->lock, 1, 1)) {
    rwa_free_005bf120(s->buffer);
    if (s->flags & 1) {
      RwaObjDestroy(s->output, 0, 0);
      s->flags &= ~1u;
    }
    return 0;
  }
  cb.mix = (void *(*)(void *, char *))rwa_soft_output_mix_005d8d00;
  cb.bytes = period;
  cb.mixer = s;
  _rwaDXAudioOutputSetMixerCallback(s->output, &cb);
  return obj;
}
