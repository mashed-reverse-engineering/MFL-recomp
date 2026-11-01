#include "mfl_tier2.h"
#include "rwp_common.h"

extern int rwp_farfield_00576c50(void *space, unsigned int id);

/* Unregisters a body from the world: removes it from its owner's awake set first (vtable[5], awake count - 1) when both worlds have it awake,
   clears the id from the used / wanted / second bit sets, from the space (576c50) when registered there, empties the id slot and resets the
   body (id 0xffff, no owner).  Returns w. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflDynWorld *rwp_dyn_0056a4b0(MflDynWorld *w, MflDynBody *b)
{
  unsigned int id = b->id;
  MflDynWorld *owner;

  if (BIT_TEST(w->bits_awake, id) && b->owner && BIT_TEST(b->owner->bits_awake, id)) {
    owner = b->owner;
    b->vtbl[5](b);
    BIT_CLR(owner->bits_awake, b->id);
    owner->awake_count--;
  }
  BIT_CLR(w->bits_used, id);
  BIT_CLR(w->bits_a, id);
  BIT_CLR(w->bits_c, id);
  if (BIT_TEST(w->bits_space, id)) {
    rwp_farfield_00576c50(w->space, id);
    BIT_CLR(w->bits_space, id);
  }
  w->bodies[id] = 0;
  b->id = 0xffff;
  b->owner = 0;
  return w;
}
