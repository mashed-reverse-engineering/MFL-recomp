// cluster-entries: rwa_voice_manager_prioritize_005b7960
/* Installed cluster rwa_voice_manager_prioritize_005b7960 (tools/cluster_install.py from output/cluster_pilot_l2/rwa_voice_manager_prioritize_005b7960.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA virtual voice manager (voices/rwavoicemanager.c): a voice's priority from its volume and its distance to the listener
   (3D voices: linear roll off between min and max distance, table square root), then its place in the priority sorted voice
   list (register function, voice in ecx).  Cluster: rwa_voice_manager_prioritize_005b7960 is entered through the generated ecx
   thunk. */
#include "mfl_tier2.h"

typedef struct RwaVoice {
  struct RwaVoice *next, *prev;     /* +0x00 in the playing (priority sorted) list or the free list */
  void *handle;                     /* +0x08 the real voice */
  RwUInt32 flags;                   /* +0x0c 1 in the playing list */
  RwInt32 weight;                   /* +0x10 */
  float frequency;                  /* +0x14 */
  float volume;                     /* +0x18 */
  RwUInt32 w1c[3];
  char *wave;                       /* +0x28 (+0x54: flag 2 = 3D) */
  RwInt32 timeStamp;                /* +0x2c */
  RwV3d pos;                        /* +0x30 */
  RwV3d velocity;                   /* +0x3c */
  float minDistance;                /* +0x48 */
  float maxDistance;                /* +0x4c */
  float basePriority;               /* +0x50 */
  float priorityWeight;             /* +0x54 */
  float priority;                   /* +0x58 */
} RwaVoice;

/*007dab58*/ extern RwaVoice *g_data_7dab58;      /* playing list head {next, prev} */
/*007dab5c*/ extern RwaVoice *g_data_7dab5c;
/*007dab84*/ extern float g_data_7dab84;          /* listener position x, y, z */
/*007dab88*/ extern float g_data_7dab88;
/*007dab8c*/ extern float g_data_7dab8c;
/*006494a0*/ extern float g_data_6494a0;          /* listener roll off */
/*0064a0e0*/ extern short g_data_64a0e0[];        /* square root mantissa table */

#define PLAYING ((RwaVoice *)&g_data_7dab58)

/* square root from the table: exponent halved, mantissa (odd exponent: with the hidden bit) looked up by its top 7 bits */
static float table_sqrt(float x)
{
  RwUInt32 bits = *(RwUInt32 *)&x, m = bits & 0x7fffff;
  RwInt32 e = (RwInt32)(bits >> 23) - 0x7f;
  RwUInt32 r;

  if (e & 1)
    m |= 0x800000;
  r = (RwUInt32)(RwInt32)g_data_64a0e0[m >> 16] << 16 | (RwUInt32)((RwInt32)(short)((short)e >> 1) + 0x7f) << 23;
  return *(float *)&r;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_voice_manager_prioritize_005b7960_n(RwaVoice *v)
{
  RwaVoice *e, *n, *p;
  float f, d2, dx, dy, dz, a, b, prio;

  *(RwUInt32 *)&v->priority = 0;
  if (v->volume > 0.0f) {
    f = 1.0f;
    if (*(unsigned char *)(v->wave + 0x54) & 2) {
      float maxd = v->maxDistance, mind = v->minDistance;
      dx = g_data_7dab84 - v->pos.x;
      dy = g_data_7dab88 - v->pos.y;
      dz = g_data_7dab8c - v->pos.z;
      d2 = dy * dy + dx * dx + dz * dz;
      if (!(maxd * maxd > d2))
        goto sort;
      if (d2 != 0.0f)
        d2 = table_sqrt(d2);
      if (!(d2 < mind)) {
        a = d2 - mind;
        b = maxd - mind;
        f = (b - a) / (g_data_6494a0 * a + b) * v->volume;
        if (!(f > 0.0f))
          goto sort;
      }
    }
    v->priority = (1.0f - v->priorityWeight) * f + v->basePriority * v->priorityWeight;
  }
sort:
  if (v->flags & 1) {
    prio = v->priority;
    for (e = v;;) {
      n = e->next;
      p = e->prev;
      a = n == PLAYING ? 2.0f : n->priority;
      b = p == PLAYING ? -1.0f : p->priority;
      if (prio > a)
        e = n;
      else if (prio < b)
        e = p;
      else
        break;
    }
    if (e != v) {
      v->prev->next = v->next;
      v->next->prev = v->prev;
      v->prev = e->prev;
      v->next = e;
      e->prev->next = v;
      e->prev = v;
    }
    v->flags |= 1;
    return;
  }
  v->prev->next = v->next;
  v->next->prev = v->prev;
  for (e = g_data_7dab58; e != PLAYING; e = e->next)
    if (v->priority < e->priority) {
      v->prev = e->prev;
      v->next = e;
      e->prev->next = v;
      e->prev = v;
      v->flags |= 1;
      return;
    }
  v->prev = g_data_7dab5c;
  v->next = PLAYING;
  g_data_7dab5c->next = v;
  g_data_7dab5c = v;
  v->flags |= 1;
}

/* entry thunk at the original address of rwa_voice_manager_prioritize_005b7960: register inputs (ecx) -> natural C rwa_voice_manager_prioritize_005b7960_n; ret 0 */
__declspec(naked) void rwa_voice_manager_prioritize_005b7960(void)
{
    __asm {
        push ecx
        call rwa_voice_manager_prioritize_005b7960_n
        add esp, 4
        ret
    }
}
