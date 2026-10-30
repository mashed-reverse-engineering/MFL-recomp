/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_SET(bits, id) ((bits)[(id) >> 5] |= 1 << ((id) & 0x1f))
#define BIT_CLEAR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
typedef struct RwpBody RwpBody;
struct RwpBody { void (**vtbl)(RwpBody *); char pad[0x1c]; unsigned short id; char pad2[2]; struct RwpWorld *owner; };
typedef struct RwpWorld { char pad[0x4c]; int count; char pad2[0x10]; unsigned *bitsA; unsigned *bitsB; } RwpWorld;
/* Marks a body in the world's pending set (flag) or, without flag, deactivates it if it is active in the world and its owner world, then clears the pending bit. (Rwp library) */
RwpWorld *rwp_volume_0056b220(RwpWorld *w, RwpBody *b, int flag)
{
  if (flag) {
    BIT_SET(w->bitsB, b->id);
    return w;
  }
  if (BIT_TEST(w->bitsA, b->id) && b->owner && BIT_TEST(b->owner->bitsA, b->id)) {
    b->vtbl[5](b);
    BIT_CLEAR(w->bitsA, b->id);
    w->count--;
  }
  BIT_CLEAR(w->bitsB, b->id);
  return w;
}
