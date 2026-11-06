// cluster-entries: rwa_voice_manager_stop_voice_005b78a0 rwa_voice_manager_update_005b7bb0
/* Installed cluster rwa_voice_manager_stop_voice_005b78a0 (tools/cluster_install.py from output/cluster_pilot_l2/rwa_voice_manager_stop_voice_005b78a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA virtual voice manager (voices/rwavoicemanager.c): stopping a voice (register function, voice in esi) and the per frame
   update that maps the highest priority virtual voices onto real voices.  Cluster: rwa_voice_manager_stop_voice_005b78a0 is
   entered through the generated esi thunk. */
#include "mfl_tier2.h"

typedef struct RwaVoice {
  struct RwaVoice *next, *prev;     /* +0x00 in the playing (priority sorted) list or the free list */
  void *handle;                     /* +0x08 the real voice */
  RwUInt32 flags;                   /* +0x0c 1 playing, 2 started, 4 looped, 8 streamed, 0x10 stop request */
  RwInt32 weight;                   /* +0x10 real voices the voice needs */
  float frequency;                  /* +0x14 (Hz) */
  RwUInt32 volume;                  /* +0x18 */
  RwUInt32 pan;                     /* +0x1c */
  RwUInt32 position;                /* +0x20 play position (samples) */
  RwUInt32 startPosition;           /* +0x24 */
  char *wave;                       /* +0x28 */
  RwInt32 timeStamp;                /* +0x2c */
  RwV3d pos;                        /* +0x30 */
  RwV3d velocity;                   /* +0x3c */
  RwUInt32 minDistance;             /* +0x48 */
  RwUInt32 maxDistance;             /* +0x4c */
  RwUInt32 w50, w54;
  RwUInt32 priority;                /* +0x58 */
  RwUInt32 w5c;
  RwUInt32 userParam;               /* +0x60 */
} RwaVoice;

/*005b73b0*/ extern void *FUN_005b73b0(void *handle, int param, int count, int value);      /* get a handle (parameter) */
/*005b7370*/ extern RwUInt32 FUN_005b7370(void *handle, int param, int count, int value);      /* RwaObjHandleGetParamInt32 */
/*005b7300*/ extern void FUN_005b7300(void *handle, int param, int count, RwUInt32 value);      /* RwaObjHandleSetParamInt16 */
/*005b7330*/ extern void FUN_005b7330(void *handle, int param, int count, void *data);      /* set a handle parameter */
/*005b72a0*/ extern void FUN_005b72a0(void *handle, int param, int count, void *data);      /* RwaObjHandleSetParamData */
/*005bd580*/ extern RwUInt32 FUN_005bd580(void *format, RwUInt32 bytes);      /* RwaWaveFormatGetSamples */

/*007dab58*/ extern RwaVoice *g_data_7dab58;      /* playing list head {next, prev} */
/*007dab60*/ extern RwaVoice *g_data_7dab60;      /* free list head {next, prev} */
/*007dab74*/ extern void *g_data_7dab74;          /* the output handle */
/*007dab78*/ extern RwInt32 g_data_7dab78;        /* voice count */
/*007dab7c*/ extern RwInt32 g_data_7dab7c;        /* playing voices */
/*007dab80*/ extern RwInt32 g_data_7dab80;        /* real voices in use */
/*007dab84*/ extern RwUInt32 g_data_7dab84;       /* listener position x, y, z */
/*007dab88*/ extern RwUInt32 g_data_7dab88;
/*007dab8c*/ extern RwUInt32 g_data_7dab8c;
/*007dab90*/ extern RwInt32 g_data_7dab90;        /* the time stamp (ms) */
/*006494a0*/ extern RwUInt32 g_data_6494a0;       /* listener roll off */

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

#define PLAYING ((RwaVoice *)&g_data_7dab58)
#define FREE ((RwaVoice *)&g_data_7dab60)

/* Stop a playing voice: unlink, release its real voice, move it to the free list. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_stop_voice_005b78a0_n(RwaVoice *v)
{
  void *h;

  if (!(v->flags & 1))
    return;
  v->prev->next = v->next;
  v->next->prev = v->prev;
  h = v->handle;
  if (h) {
    if (FUN_005b7370(h, 6, 2, 0))
      FUN_005b7300(h, 6, 1, 0);
    FUN_005b7330(h, 9, 1, 0);
    v->handle = 0;
  }
  v->next = g_data_7dab60;
  v->prev = FREE;
  g_data_7dab60->prev = v;
  g_data_7dab60 = v;
  g_data_7dab80 -= v->weight;
  g_data_7dab7c--;
  v->position = v->startPosition;
  v->flags &= ~3u;
  v->priority = 0;
  v->weight = 0;
}

/* _rwaVoiceManagerUpdate: walk the voices by priority; those that fit into the real voices are advanced by the elapsed time
   (stopped or looped at the end of the wave), the others give their real voice back; a voice without one gets one. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_update_005b7bb0(void)
{
  char *listener;
  RwaVoice *v, *next;
  void *h;
  RwInt32 used, elapsed;
  RwUInt32 samples, pos;
  float t;

  if (!g_data_7dab74)
    return;
  listener = FUN_005b73b0(g_data_7dab74, 1, 2, 0);
  if (listener) {
    g_data_7dab84 = *(RwUInt32 *)(listener + 0x28);
    g_data_7dab88 = *(RwUInt32 *)(listener + 0x2c);
    g_data_7dab8c = *(RwUInt32 *)(listener + 0x30);
    g_data_6494a0 = *(RwUInt32 *)(listener + 0x64);
  }
  used = 0;
  for (v = g_data_7dab58; v != PLAYING; v = next) {
    h = v->handle;
    next = v->next;
    if (used < g_data_7dab80 - g_data_7dab78) {
      if (v->flags & 0x10)
        rwa_voice_manager_stop_voice_005b78a0_n(v);
      else if (h) {
        if (v->flags & 8)
          v->position = v->startPosition;
        else
          v->position = FUN_005b7370(h, 7, 2, 0);
        FUN_005b7300(h, 6, 1, 0);
        FUN_005b7330(h, 9, 1, 0);
        v->timeStamp = g_data_7dab90;
        v->handle = 0;
      }
      if ((v->flags & 1) && !(v->flags & 8)) {
        samples = FUN_005bd580(v->wave + 0x10, *(RwUInt32 *)(v->wave + 0x18));
        elapsed = g_data_7dab90 - v->timeStamp;
        t = (float)elapsed;
        if (elapsed < 0)
          t += 4294967296.0f;
        t = t * v->frequency * 0.001f;
        pos = v->position + CHOP(t);
        v->position = pos;
        if (pos >= samples) {
          if (v->flags & 4) {
            do
              pos -= samples;
            while (pos >= samples);
            v->position = pos;
          } else {
            rwa_voice_manager_stop_voice_005b78a0_n(v);
            v->handle = 0;
          }
        }
        v->timeStamp = g_data_7dab90;
      }
    } else if (!h) {
      h = FUN_005b73b0(g_data_7dab74, 4, 2, 0);
      v->handle = h;
      if (h) {
        FUN_005b7330(h, 9, 1, v->wave);
        FUN_005b7300(h, 4, 1, *(RwUInt32 *)&v->frequency);
        FUN_005b7300(h, 5, 1, v->volume);
        FUN_005b7300(h, 8, 1, (v->flags >> 2) & 1);
        if (!(v->flags & 8))
          FUN_005b7300(h, 7, 1, v->position);
        FUN_005b7300(h, 10, 1, v->pan);
        FUN_005b72a0(h, 0, 1, &v->pos);
        FUN_005b72a0(h, 1, 1, &v->velocity);
        FUN_005b7300(h, 2, 1, v->minDistance);
        FUN_005b7300(h, 3, 1, v->maxDistance);
        FUN_005b7300(h, 11, 1, v->userParam);
        FUN_005b7300(h, 6, 1, 1);
      }
    }
    used += v->weight;
  }
}

/* entry thunk at the original address of rwa_voice_manager_stop_voice_005b78a0: register inputs (esi) -> natural C rwa_voice_manager_stop_voice_005b78a0_n; ret 0 */
__declspec(naked) void rwa_voice_manager_stop_voice_005b78a0(void)
{
    __asm {
        push esi
        call rwa_voice_manager_stop_voice_005b78a0_n
        add esp, 4
        ret
    }
}
