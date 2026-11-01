#include "mfl_tier2.h"
#include "rwp_common.h"

extern int g_rwp_world_full_warned;
extern int rwp_capsule_00575a30(void *graph, unsigned short id, unsigned short *out, unsigned int *visited, int sort);

/* Collects the bodies reachable from the wanted set `bits_a` (via neighbour ids of 575a30 and joints), wakes the ones that are not awake,
   puts the awake ones into objs, emits body pairs and joints and drops the pairs / joints that no longer touch an awake body.
   objs / nobjs: max and out count of awake bodies; pairs / pairmax / npairs; joints / jointmax / njoints.  Returns w. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflDynWorld *rwp_dyn_0056a7c0(MflDynWorld *w, MflDynBody **objs, unsigned int objmax, unsigned int *nobjs,
                              MflDynPair *pairs, unsigned int pairmax, unsigned int *npairs,
                              MflDynJoint **joints, unsigned int jointmax, unsigned int *njoints)
{
  unsigned int i, j, b, k, np, nj, ncnt, m, nn;
  unsigned int idx, id, other;
  MflDynBody *o;
  MflDynNode *node;
  MflDynJoint *jt;
  MflDynBody *waking;

  k = 0;
  np = 0;
  nj = 0;
  for (i = 0; i < w->words; i++)
    w->seen[i] = 0;
  for (i = 0; i < w->words; i++) {
    if (w->bits_a[i]) {
      for (b = 0; b < 32; b++)
        if (w->bits_a[i] & (1 << b))
          w->ids[k++] = (unsigned short)(i * 32 + b);
    }
    w->seen2[i] = w->bits_a[i];
  }
  for (idx = 0; idx < k; idx++) {
    id = w->ids[idx];
    o = w->bodies[id];
    if (!o->owner) continue;
    if (!BIT_TEST(o->owner->bits_c, o->id) && !BIT_TEST(o->owner->bits_a, o->id)) continue;
    BIT_SET(w->seen, id);
    ncnt = rwp_capsule_00575a30(w->graph, (unsigned short)id, w->near_ids, w->seen, w->flags & 1);
    for (j = 0; j < ncnt; j++) {
      unsigned short nb = w->near_ids[j];
      if (np < pairmax) {
        pairs[np].a = w->bodies[id];
        pairs[np].kind_a = (short)0x8000;
        pairs[np].b = w->bodies[nb];
        pairs[np].kind_b = (short)0x8000;
        pairs[np].zero = 0;
        np++;
      }
      if (k < w->idcap && !BIT_TEST(w->seen2, nb)) {
        w->ids[k++] = nb;
        BIT_SET(w->seen2, nb);
      }
    }
    if (!w->links) continue;
    node = w->links[id];
    if (!node) continue;
    for (; node; node = node->next) {
      jt = node->joint;
      if (jt->has_a != 0xffff && jt->body_a->owner && BIT_TEST(jt->body_a->owner->bits_c, jt->body_a->id)
          && jt->has_b != 0xffff && jt->body_b->owner && BIT_TEST(jt->body_b->owner->bits_c, jt->body_b->id)) {
        other = (unsigned short)(jt->body_b->id ^ jt->body_a->id ^ id);
        if (k < w->idcap && !BIT_TEST(w->seen2, other)) {
          w->ids[k++] = (unsigned short)other;
          BIT_SET(w->seen2, other);
        }
        if ((unsigned short)id < (unsigned short)other && nj < jointmax)
          joints[nj++] = jt;
      } else if (nj < jointmax) {
        joints[nj++] = jt;
      }
    }
  }
  for (i = 0; i < w->words; i++)
    w->added[i] = 0;
  for (idx = 0; idx < k; idx++) {
    id = w->ids[idx];
    BIT_SET(w->added, id);
  }
  for (i = 0; i < w->words; i++) {
    m = ~w->added[i] & w->bits_awake[i];
    if (m) {
      for (b = 0; b < 32; b++) {
        if (m & (1 << b)) {
          o = w->bodies[i * 32 + b];
          if (o->flags & 2) {
            if (o->owner && BIT_TEST(o->owner->bits_awake, o->id)) {
              o->vtbl[5](o);
              BIT_CLR(w->bits_awake, o->id);
              w->awake_count--;
            }
            BIT_CLR(w->bits_awake, i * 32 + b);
          }
        }
      }
    }
  }
  for (idx = 0; idx < k; idx++) {
    id = w->ids[idx];
    if (BIT_TEST(w->added, id) && !BIT_TEST(w->bits_awake, id) && BIT_TEST(w->bits_c, id)) {
      o = w->bodies[id];
      waking = 0;
      if (o->owner && BIT_TEST(o->owner->bits_awake, o->id)) {
        BIT_SET(w->bits_awake, id);
      } else {
        if (w->awake_count < w->awake_max) {
          if (o->vtbl[4](o)) {
            waking = o;
            BIT_SET(w->bits_awake, o->id);
            g_rwp_world_full_warned = 0;
            w->awake_count++;
          } else if (!g_rwp_world_full_warned) {
            g_rwp_world_full_warned = 1;
          }
        }
        if (waking)
          BIT_SET(w->bits_awake, id);
      }
    }
  }
  j = 0;
  for (idx = 0; idx < k && j < objmax; idx++) {
    id = w->ids[idx];
    if (BIT_TEST(w->bits_awake, id))
      objs[j++] = w->bodies[id];
  }
  *nobjs = j;
  if (np > 0) {
    MflDynPair *rec = pairs;
    MflDynPair *end = pairs + np;
    idx = 0;
    nn = np;
    while (idx < nn) {
      o = rec->a;
      if (o->owner && BIT_TEST(o->owner->bits_awake, o->id)) { idx++; rec++; continue; }
      o = rec->b;
      if (o->owner && BIT_TEST(o->owner->bits_awake, o->id)) { idx++; rec++; continue; }
      nn--;
      end--;
      *rec = *end;
    }
    np = nn;
  }
  *npairs = np;
  for (i = 0; i < nj;) {
    jt = joints[i];
    if (jt->has_a != 0xffff && jt->body_a->owner && BIT_TEST(jt->body_a->owner->bits_awake, jt->body_a->id)) { i++; continue; }
    if (jt->has_b != 0xffff && jt->body_b->owner && BIT_TEST(jt->body_b->owner->bits_awake, jt->body_b->id)) { i++; continue; }
    nj--;
    joints[i] = joints[nj];
  }
  *njoints = nj;
  return w;
}
