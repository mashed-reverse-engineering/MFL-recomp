#include "mfl_tier2.h"
#include <math.h>

/* One contact point record: 40 bytes. */
typedef struct MflContact {
  int pad0[3];
  float pos[3];   /* 0xc */
  int tag;   /* 0x18 */
  int flag;   /* 0x1c */
  int pad20[2];
} MflContact;

/* Contact list: the records start at 0x0, the count is at 0xac. */
typedef struct MflContactList {
  MflContact c[4];
  int pad_a0[3];
  unsigned int count;   /* 0xac */
} MflContactList;

/* Adds the contact point pos (tag as given) to the list of at most 4 records.  When mindist >= 0 and the list is not empty, the new
   point is dropped (result 0) if it is within mindist (sum of the axis distances) of the first or of the last record.  Returns 1 when added. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_005857c0(float *pos, int tag, MflContactList *list, float mindist)
{
  unsigned int n = list->count;
  double acc;
  float *lp;

  if (n) {
    if (mindist >= 0.0f) {
      lp = list->c[0].pos;
      acc = fabs(lp[2] - pos[2]);
      acc = acc + fabs(lp[1] - pos[1]);
      if ((float)(acc + fabs(lp[0] - pos[0])) <= mindist) return 0;
      if (n > 1) {
        lp = list->c[n - 1].pos;
        acc = fabs(lp[0] - pos[0]);
        acc = acc + fabs(lp[1] - pos[1]);
        if ((float)(acc + fabs(lp[2] - pos[2])) <= mindist) return 0;
      }
    }
  }
  list->c[n].pos[0] = pos[0];
  list->c[n].pos[1] = pos[1];
  list->c[n].pos[2] = pos[2];
  list->c[n].flag = 0;
  list->c[n].tag = tag;
  list->count = list->count + 1;
  return 1;
}
