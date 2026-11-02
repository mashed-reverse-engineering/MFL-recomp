/* rwp_items_update_sleep (0x00573650): put items to sleep or wake them from their angular/linear state */
#include "mfl_tier2.h"

/* Two float3 (at 0 and 0x10) of a body table entry (32 bytes). */
typedef struct MflVec2x3 {
  float a[3];
  float pad0c;
  float b[3];
  float pad1c;
} MflVec2x3;
typedef struct MflSleepTables {
  unsigned char pad00[8];
  MflVec2x3 *spin;   /* 0x8: angular state per body */
  unsigned char pad0c[0x34];
  MflVec2x3 *vel;   /* 0x40: velocity state per body */
} MflSleepTables;
typedef struct MflSleepOwner {
  unsigned char pad00[0x10];
  MflSleepTables *tables;   /* 0x10 */
} MflSleepOwner;
typedef struct MflSleepBody {
  MflSleepOwner *owner;
  int index;   /* 0x4 */
} MflSleepBody;
/* A link from an object to one of its bodies (12 bytes, the body at 4). */
typedef struct MflSleepLink {
  int pad0;
  MflSleepBody *body;   /* 0x4 */
  int pad8;
} MflSleepLink;
typedef struct MflSleepItem {
  unsigned char pad00[8];
  unsigned int flags;   /* 0x8: bit 0 = may fall asleep */
  unsigned short nlinks;   /* 0xc */
  unsigned short pad0e;
  MflSleepLink *links;   /* 0x10 */
  unsigned char pad14[0x10];
  void *owner;   /* 0x24 */
} MflSleepItem;
/* A group of items (40 bytes): flag byte at 0x18 (bits 0 / 1 = skip). */
typedef struct MflSleepGroup {
  MflSleepItem **items;   /* 0x0 */
  unsigned char pad04[8];
  unsigned int count;   /* 0xc */
  unsigned char pad10[8];
  unsigned char flags;   /* 0x18 */
  unsigned char pad19[0xf];
} MflSleepGroup;
typedef struct MflSleepWorld {
  unsigned char pad00[0xc];
  float thr[4];   /* 0xc: speed limits (the world compares squared lengths with their squares) */
  unsigned char pad1c[0xac];
  unsigned int ngroups;   /* 0xc8 */
  unsigned char padcc[8];
  MflSleepGroup *groups;   /* 0xd4 */
} MflSleepWorld;

extern void rwp_volume_0056b1d0(void *owner, MflSleepItem *item, int awake);

/* Puts items to sleep or wakes them: for every group without the skip bits and every item of it that may fall asleep, the item sleeps when all
   its bodies have squared angular / linear state vectors below the squared limits (spin a / b: thr[0] / thr[1], velocity a / b: thr[2] /
   thr[3]); 56b1d0 is told the result (awake = 1 for items that do not sleep).  Returns the world. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
MflSleepWorld *rwp_items_update_sleep(MflSleepWorld *w)
{
  float t[4];
  double l;
  unsigned int gi, ii, k;
  MflSleepGroup *g;
  MflSleepItem *it;
  MflSleepBody *bd;
  MflSleepTables *tab;
  MflVec2x3 *v;
  int asleep;

  t[0] = w->thr[0] * w->thr[0];
  t[1] = w->thr[1] * w->thr[1];
  t[2] = w->thr[2] * w->thr[2];
  t[3] = w->thr[3] * w->thr[3];
  for (gi = 0; gi < w->ngroups; gi++) {
    g = w->groups + gi;
    if (g->flags & 3) continue;
    for (ii = 0; ii < g->count; ii++) {
      it = g->items[ii];
      asleep = it->flags & 1;
      if (asleep) {
        for (k = 0; k < it->nlinks; k++) {
          bd = it->links[k].body;
          tab = bd->owner->tables;
          v = &tab->spin[bd->index];
          l = v->a[0] * v->a[0];
          l = l + v->a[1] * v->a[1];
          l = l + v->a[2] * v->a[2];
          if (l > t[0]) { asleep = 0; break; }
          l = v->b[0] * v->b[0];
          l = l + v->b[1] * v->b[1];
          l = l + v->b[2] * v->b[2];
          if (l > t[1]) { asleep = 0; break; }
          v = &tab->vel[bd->index];
          l = v->a[0] * v->a[0];
          l = l + v->a[1] * v->a[1];
          l = l + v->a[2] * v->a[2];
          if (l > t[2]) { asleep = 0; break; }
          l = v->b[0] * v->b[0];
          l = l + v->b[1] * v->b[1];
          l = l + v->b[2] * v->b[2];
          if (l > t[3]) { asleep = 0; break; }
        }
      }
      rwp_volume_0056b1d0(it->owner, it, asleep == 0);
    }
  }
  return w;
}
