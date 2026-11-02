/* rwp_world_activate_object (0x0056a210): register a body in the world under an id (-1 = lowest free) */
#include "mfl_tier2.h"
#include "rwp_common.h"

extern int g_rwp_world_full_warned;
extern int rwp_farfield_00576670(void *space, unsigned int id, int what);

/* Registers a body in the world under `id` (id = -1: the lowest free id; 0 when none is free or it is above the capacity): marks the id used, links body
   and world, clears the wanted / awake / second bit sets and, when the body answers its vtable[6] query, registers it with the space; then
   flags bit 0 sets "wanted", bit 1 sets bits_c (else the body is removed from the awake set first), bit 2 wakes the body when there is room.  Returns w. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflDynWorld *rwp_world_activate_object(MflDynWorld *w, MflDynBody *body, unsigned int id, unsigned int flags)
{
  unsigned int i, word, bit;
  int out[8];      /* the original frame has 0x20 bytes here; the vtable[6] callee writes a 6 float box */
  int r;

  if (id == 0xffffffff) {
    i = 0;
    while (i < w->words && w->bits_used[i] == 0xffffffff)
      i++;
    if (i == w->words) return 0;
    word = w->bits_used[i];
    id = i * 32;
    bit = 1;
    if (word & bit) {
      do {
        bit += bit;
        id++;
      } while (bit & word);
    }
    if (id >= w->idcap) return 0;
  }
  BIT_SET(w->bits_used, id);
  body->id = (unsigned short)id;
  body->owner = w;
  w->bodies[id] = body;
  w->bits_a[id >> 5] &= ~(1 << (id & 0x1f));
  w->bits_c[id >> 5] &= ~(1 << (id & 0x1f));
  w->bits_awake[id >> 5] &= ~(1 << (id & 0x1f));
  r = body->vtbl[6](body, out);
  if (r) {
    rwp_farfield_00576670(w->space, id, r);
    w->bits_space[id >> 5] |= 1 << (id & 0x1f);
  } else {
    w->bits_space[id >> 5] &= ~(1 << (id & 0x1f));
  }
  if (flags & 1) BIT_SET(w->bits_a, body->id); else BIT_CLR(w->bits_a, body->id);
  if (flags & 2) {
    BIT_SET(w->bits_c, body->id);
  } else {
    if (BIT_TEST(w->bits_awake, body->id) && body->owner && BIT_TEST(body->owner->bits_awake, body->id)) {
      body->vtbl[5](body);
      BIT_CLR(w->bits_awake, body->id);
      w->awake_count--;
    }
    BIT_CLR(w->bits_c, body->id);
  }
  w->bits_awake[id >> 5] &= ~(1 << (id & 0x1f));
  if (flags & 4) {
    if (!(body->owner && BIT_TEST(body->owner->bits_awake, body->id)) && w->awake_count < w->awake_max) {
      if (body->vtbl[4](body)) {
        BIT_SET(w->bits_awake, body->id);
        g_rwp_world_full_warned = 0;
        w->awake_count++;
        return w;
      }
      if (!g_rwp_world_full_warned)
        g_rwp_world_full_warned = 1;
    }
  }
  return w;
}
