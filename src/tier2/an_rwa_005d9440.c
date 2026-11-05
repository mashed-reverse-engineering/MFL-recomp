/* rwa_soft_dsp_create_005d9440 (0x005d9440): software dsp: create the stream node, its sample block, wave and embedded voice */
#include <string.h>
#include "mfl_tier2.h"

/*005c1c30*/ extern int _RwaStreamNodeCreate(void *node, void *arg2, void *params, int a4, int a5);
/*005c1d80*/ extern void *FUN_005c1d80(void *node, void *lookup, void *arg);      /* _RwaStreamNodeDestroy */
/*005bda80*/ extern void *rwa_unit243b40_005bda80(const void *uuid);
/*005bdb30*/ extern void *sub_243FF0(void *def, void *owner, void *a3, void *a4);      /* RwaObjInterfaceAdd */
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, void *format, int a3);
/*005bd580*/ extern void *FUN_005bd580(void *format, RwUInt32 bytes);
/*005ccbd0*/ extern int rwa_dxaudio_wave_create_005ccbd0(void *output, void *format, void *defFormat, RwUInt32 flags, void *wave);
/*005ba5c0*/ extern void *sub_23DCA0(void *a1, const void *uuid, void *a3);      /* RwaObjDefFindByUUID */
/*005baf50*/ extern void *_rwaObjFindFixed(void *objDef);
/*005c98e0*/ extern void *rwa_dxaudio_voice_create_005c98e0(void *output, void *a2, void *objDef, void *lookup, void *link);
/*005d9390*/ extern void rwa_soft_output_int_lookup_005d9390(void);
/*005ccc00*/ extern void thunk_FUN_005bc0a0(void *wave);      /* RwaWaveDestroy */
/*005ca7d0*/ extern void _rwaDXAudioVoiceSetWave(void *voice, void *wave);
/*005ca780*/ extern void _rwaDXAudioVoiceSetLoop(void *voice, int loop);
/*005c1f10*/ extern void rwa_unit246fd0_005c1f10(void *node, int a2, int a3, int a4);
/*005c1cb0*/ extern void RwaStreamNodeCreationComplete(void *node);
/*005f829c*/ extern undefined g_rwa_soft_dsp_name;
/*005f7dbc*/ extern undefined g_rwa_wavefmt_mono_uuid;
/*005f7de4*/ extern undefined g_rwa_wavefmt_stereo_uuid;
/*005f834c*/ extern undefined g_rwa_soft_output_lookup_uuid;

typedef void *(*RwaAllocFn)(RwUInt32 bytes, void *arg);
typedef void (*RwaFreeFn)(void *block, void *arg);
typedef struct RwaAllocator { RwaAllocFn alloc; RwaFreeFn free; void *arg; } RwaAllocator;      /* st+0x218 */
typedef struct RwaLookupLink { void *next; void *arg; char *obj; } RwaLookupLink;      /* see 0x5d9390 */
typedef struct RwaSoftLink { struct RwaSoftLink *next, *prev; } RwaSoftLink;

typedef struct RwaWaveFormat {      /* 0x1c bytes */
  RwUInt32 w0, w4, bytes;      /* +0x08 data bytes */
  unsigned char b0c, channels;      /* +0x0d */
  unsigned char b0e, b0f;
  RwUInt32 w10, w14;
  unsigned char b18, interleaved;      /* +0x19 */
  unsigned char b1a, b1b;
} RwaWaveFormat;

typedef struct RwaEnv { RwInt32 room, roomHF; float rolloff, decayTime, decayHFRatio; RwInt32 reflections; float reflectionsDelay;
                        RwInt32 reverb; float reverbDelay, diffusion, density, hfReference; } RwaEnv;      /* I3DL2, 0x30 bytes */

typedef struct RwaBlockDesc { RwUInt32 a, b, c, d, e; } RwaBlockDesc;      /* 0x14 bytes each, n of them in front of the samples */

/* params: +0x0c frames per block (n), +0x14 / +0x18 interface args, +0x1c source (+0x14 wave format, +0x20 block bytes),
   +0x20 allocator, +0x24 / +0x28 copied, +0x2c bit 0 = flag for the wave.  dsp+0x20 = state.  Returns dsp, or NULL (node destroyed). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_dsp_create_005d9440(char *dsp, void *output, char *params, void *lookup, void *arg)
{
  char *st = *(char **)(dsp + 0x20);
  char *src;
  char *blk;
  RwUInt32 i;
  RwaLookupLink link;

  if (!_RwaStreamNodeCreate(dsp, output, params, 0, 0))
    return 0;
  *(void **)(st + 0x30) = sub_243FF0(rwa_unit243b40_005bda80(&g_rwa_soft_dsp_name), *(void **)dsp,
                                     *(void **)(params + 0x14), *(void **)(params + 0x18));
  src = *(char **)(params + 0x1c);
  *(RwUInt32 *)(st + 0x1dc) = *(RwUInt32 *)(src + 0x20);
  *(int *)(st + 0x1f0) = 0;
  *(RwUInt32 *)(st + 0x1ec) = *(RwUInt32 *)(params + 0xc);
  *(RwaAllocator *)(st + 0x218) = **(RwaAllocator **)(params + 0x20);
  *(unsigned char *)(st + 0x231) = 0;
  *(int *)(st + 0x1e8) = 0;
  *(int *)(st + 0x20c) = 0;
  *(int *)(st + 0x210) = 0;
  *(int *)(st + 0x228) = *(int *)(params + 0x24);
  *(int *)(st + 0x22c) = *(int *)(params + 0x28);
  *(RwUInt32 *)(st + 0x224) = *(RwUInt32 *)(st + 0x1ec) * 2;
  *(int *)(st + 0x1f4) = 0;
  *(char **)(st + 0x1d8) = (char *)(*(RwaAllocFn *)(st + 0x218))(
      *(RwUInt32 *)(st + 0x1dc) + *(RwUInt32 *)(st + 0x1ec) * sizeof(RwaBlockDesc), *(void **)(st + 0x220));
  if (*(char **)(st + 0x1d8) == 0) {
    FUN_005c1d80(dsp, 0, 0);
    return 0;
  }

  blk = *(char **)(st + 0x1d8) + *(RwUInt32 *)(st + 0x1ec) * sizeof(RwaBlockDesc);      /* the samples follow the descriptors */
  *(RwUInt32 *)(st + 0x200) = *(RwUInt32 *)(st + 0x1dc);
  *(int *)(st + 0x1f8) = 0;
  *(char **)(st + 0x1fc) = blk;
  *(int *)(st + 0x208) = 0;
  *(int *)(st + 0x204) = 0;
  memset(blk, 0, *(RwUInt32 *)(st + 0x1dc));
  for (i = 0; i < *(RwUInt32 *)(st + 0x1ec); i++) {
    RwaBlockDesc *d = (RwaBlockDesc *)(*(char **)(st + 0x1d8)) + i;
    d->a = 0;
    (*(RwaBlockDesc **)(st + 0x1d8))[i].b = 3;
    (*(RwaBlockDesc **)(st + 0x1d8))[i].c = 0;
    (*(RwaBlockDesc **)(st + 0x1d8))[i].d = 2;
    (*(RwaBlockDesc **)(st + 0x1d8))[i].e = 0;
  }
  *(int *)(st + 0x1c8) = 0;
  *(int *)(st + 0x1cc) = 3;
  *(int *)(st + 0x1d0) = 0;
  *(int *)(st + 0x1d4) = 2;

  {      /* the wave: the source's format with the whole block's size; def = the plain mono / stereo definition */
  RwaWaveFormat def;
  RwaWaveFormat fmt;

  fmt = **(RwaWaveFormat **)(src + 0x14);
  def.w0 = 0;
  def.w4 = 0;
  def.bytes = 0;
  def.b0c = 0;
  def.channels = 0;
  def.b18 = 0;
  def.interleaved = 0;
  def.w10 = 0;
  def.w14 = 0;
  if (fmt.channels == 1)
    _rwaWaveDefFormatCopyToWaveFormat(&g_rwa_wavefmt_mono_uuid, &def, 1);
  else if (fmt.channels == 2)
    _rwaWaveDefFormatCopyToWaveFormat(&g_rwa_wavefmt_stereo_uuid, &def, 1);
  fmt.bytes = *(RwUInt32 *)(st + 0x1dc) * *(RwUInt32 *)(st + 0x1ec);
  fmt.interleaved = 1;
  *(unsigned char *)(st + 0x230) = fmt.channels;
  *(void **)(st + 0x1e4) = FUN_005bd580(&fmt, fmt.bytes);
  *(void **)(st + 0x1e0) = FUN_005bd580(&fmt, *(RwUInt32 *)(st + 0x1dc));
  def.w0 = fmt.w0;
  if (!rwa_dxaudio_wave_create_005ccbd0(output, &fmt, &def, (*(RwUInt32 *)(params + 0x2c) & 1) | 8, st + 0x160)) {
    (*(RwaFreeFn *)(st + 0x21c))(*(void **)(st + 0x1d8), *(void **)(st + 0x220));
    FUN_005c1d80(dsp, 0, 0);
    return 0;
  }
  }

  link.next = lookup;
  link.arg = arg;
  link.obj = st;
  if (!rwa_dxaudio_voice_create_005c98e0(output, 0, _rwaObjFindFixed(sub_23DCA0(0, &g_rwa_soft_output_lookup_uuid, 0)),
                                         (void *)rwa_soft_output_int_lookup_005d9390, &link)) {
    thunk_FUN_005bc0a0(st + 0x160);
    (*(RwaFreeFn *)(st + 0x21c))(*(void **)(st + 0x1d8), *(void **)(st + 0x220));
    FUN_005c1d80(dsp, 0, 0);
    return 0;
  }
  _rwaDXAudioVoiceSetWave(st + 0x34, st + 0x160);
  _rwaDXAudioVoiceSetLoop(st + 0x34, 1);
  {
    RwaEnv env;      /* the I3DL2 defaults (room -10000 mB: no reverb until the game sets one) */
    env.room = -10000;
    env.roomHF = 0;
    env.rolloff = 10.0f;
    env.decayTime = 1.49f;
    env.decayHFRatio = 0.83f;
    env.reflections = -2602;
    env.reflectionsDelay = 0.007f;
    env.reverb = 200;
    env.reverbDelay = 0.011f;
    env.diffusion = 100.0f;
    env.density = 100.0f;
    env.hfReference = 5000.0f;
    *(RwaEnv *)(st + 0x74) = env;
  }
  *(RwUInt32 *)(st + 0x5c) |= 0x10;
  *(char **)(st + 0x58) = dsp;
  {
    RwaSoftLink *l = (RwaSoftLink *)(st + 0x48);
    RwaSoftLink *head = (RwaSoftLink *)(dsp + 0xc);
    if (l->next) {      /* unlink first */
      l->prev->next = l->next;
      l->next->prev = l->prev;
    }
    l->next = head->next;
    l->prev = head;
    head->next->prev = l;
    head->next = l;
  }
  rwa_unit246fd0_005c1f10(dsp, *(int *)(dsp + 0x28), 0, 1);
  RwaStreamNodeCreationComplete(dsp);
  return dsp;
}
