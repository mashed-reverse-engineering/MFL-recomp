/* rwa_soft_dsp_reset_005da030 (0x005da030): software dsp: reset the delay state, on request also stop its voice */
#include "mfl_tier2.h"

/*005ca690*/ extern int FUN_005ca690(void *voice);      /* DXAudio voice: is it triggered */
/*005ca650*/ extern void rwa_dxaudio_voice_set_trigger_005ca650(char *voice, int trigger);

typedef struct RwaSoftCallback { void (*fn)(void *item, void *arg); void *arg; } RwaSoftCallback;

/* dsp+0x20 -> state: +0x34 an embedded DXAudio voice, +0x28 state, +0x1e8 position, +0x1ec delay length, +0x1f0 pending item
   (its +0x10 = completion callback), +0x20c / +0x210 counters, +0x224 = 2 * delay length.  With reset == 0 only +0x224 is set. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_dsp_reset_005da030(char *dsp, int reset)
{
  char *st = *(char **)(dsp + 0x20);
  char *item;
  RwaSoftCallback *done;

  *(int *)(st + 0x224) = *(int *)(st + 0x1ec) * 2;
  if (!reset)
    return;
  item = *(char **)(st + 0x1f0);
  if (item) {
    done = *(RwaSoftCallback **)(item + 0x10);
    if (done)
      done->fn(item, done->arg);
    *(char **)(st + 0x1f0) = 0;
  }
  if (FUN_005ca690(st + 0x34)) {
    char *st2 = *(char **)(dsp + 0x20);      /* read again, as the original does */

    *(int *)(st2 + 0x20c) = 0;
    *(int *)(st2 + 0x210) = 0;
    rwa_dxaudio_voice_set_trigger_005ca650(st2 + 0x34, 0);
    *(int *)(st + 0x28) = 7;
  }
  *(int *)(st + 0x1e8) = 0;
  *(int *)(st + 0x20c) = 0;
  *(int *)(st + 0x210) = 0;
  *(int *)(st + 0x224) = *(int *)(st + 0x1ec) * 2;
}
