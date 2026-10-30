/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
#define BIT_CLR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
typedef struct RwpWorld { char pad0[0x4c]; int count; char pad1[0x10]; unsigned *bits; } RwpWorld;
struct RwpBody;
typedef struct RwpBodyVt { char pad[0x14]; void (*remove)(struct RwpBody *); } RwpBodyVt;
typedef struct RwpBody { RwpBodyVt *vt; char pad0[0x1c]; unsigned short id; char pad1[2]; RwpWorld *link; } RwpBody;
/* Detaches body b: if it is linked to an active volume, calls its remove method (vtable slot 5), clears its bit in the world set and decrements the world count; returns b. (Rwp library) */
RwpBody *rwp_volume_0056b3b0(RwpWorld *w, RwpBody *b)
{
  if (b->link) {
    if (BIT_TEST(b->link->bits, b->id)) {
      b->vt->remove(b);
      BIT_CLR(w->bits, b->id);
      w->count--;
    }
  }
  return b;
}
