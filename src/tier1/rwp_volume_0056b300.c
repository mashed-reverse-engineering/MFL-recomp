/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_SET(bits, id) ((bits)[(id) >> 5] |= 1 << ((id) & 0x1f))
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
typedef struct RwpBody RwpBody;
struct RwpBody { int (**vtbl)(RwpBody *); char pad[0x1c]; unsigned short id; char pad2[2]; struct RwpWorld *owner; };
typedef struct RwpWorld { char pad[0x48]; unsigned capacity; unsigned count; char pad2[0x10]; unsigned *bitsA; unsigned *bitsB; } RwpWorld;
extern int g_rwp_world_full_warned;
/* Adds a body to the world if it is not in its owner world yet and there is room; returns the body or NULL, and sets the world-full warning flag when it is full. (Rwp library) */
RwpBody *rwp_volume_0056b300(RwpWorld *w, RwpBody *b)
{
  RwpBody *added = 0;
  if (b->owner && BIT_TEST(b->owner->bitsA, b->id)) return b;
  if (w->count < w->capacity) {
    if (b->vtbl[4](b)) {
      BIT_SET(w->bitsA, b->id);
      added = b;
      g_rwp_world_full_warned = 0;
      w->count++;
    } else if (!g_rwp_world_full_warned) {
      g_rwp_world_full_warned = 1;
    }
  }
  return added;
}
