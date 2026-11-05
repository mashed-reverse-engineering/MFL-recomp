/* rwa_soft_dsp_destroy_005da0f0 (0x005da0f0): software dsp: stop and free everything the dsp node holds, then the stream node */
#include "mfl_tier2.h"

/*005da030*/ extern void rwa_soft_dsp_reset_005da030(char *dsp, int reset);
/*005ca650*/ extern void rwa_dxaudio_voice_set_trigger_005ca650(char *voice, int trigger);
/*005ca7d0*/ extern void FUN_005ca7d0(void *voice, void *wave);      /* _rwaDXAudioVoiceSetWave */
/*005baf80*/ extern void *FUN_005baf80(void *obj, void *lookup, void *link);      /* RwaObjDestroy */
/*005ccc00*/ extern void thunk_FUN_005bc0a0(void *wave);      /* RwaWaveDestroy */
/*005bd6c0*/ extern void FUN_005bd6c0(void *intf);      /* RwaObjInterfaceRemove */
/*005c1d80*/ extern void *FUN_005c1d80(void *node, void *lookup, void *arg);      /* _RwaStreamNodeDestroy */
/*005d93e0*/ extern void rwa_soft_dsp_voice_destroy_lookup_005d93e0(void);      /* the destroy lookup of the embedded voice */

typedef struct RwaLookupLink { void *next; void *arg; char *obj; } RwaLookupLink;      /* see 0x5d9390 */
typedef void (*RwaFreeFn)(void *block, void *arg);

/* dsp+0x20 -> state: +0x34 embedded DXAudio voice, +0xb8 != 0 = voice running, +0x160 wave, +0x1d8 / +0x21c / +0x220 a block
   and its free function + argument, +0x30 interface.  lookup / arg are the caller's destroy chain, passed on. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_dsp_destroy_005da0f0(char *dsp, void *lookup, void *arg)
{
  char *st = *(char **)(dsp + 0x20);
  RwaLookupLink link;

  rwa_soft_dsp_reset_005da030(dsp, 1);
  if (*(int *)(st + 0xb8)) {
    char *st2 = *(char **)(dsp + 0x20);      /* read again, as the original does */

    *(int *)(st2 + 0x20c) = 0;
    *(int *)(st2 + 0x210) = 0;
    rwa_dxaudio_voice_set_trigger_005ca650(st2 + 0x34, 0);
  }
  FUN_005ca7d0(st + 0x34, 0);
  link.next = lookup;
  link.arg = arg;
  link.obj = st;
  FUN_005baf80(st + 0x34, (void *)rwa_soft_dsp_voice_destroy_lookup_005d93e0,&link);
  thunk_FUN_005bc0a0(st + 0x160);
  (*(RwaFreeFn *)(st + 0x21c))(*(void **)(st + 0x1d8), *(void **)(st + 0x220));
  FUN_005bd6c0(*(void **)(st + 0x30));
  return FUN_005c1d80(dsp, lookup, arg);
}
