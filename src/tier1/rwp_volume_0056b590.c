/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBody { unsigned short id; } RwpBody;
typedef struct RwpHandle { struct RwpWorld *w; unsigned short id; char pad[0x96]; } RwpHandle;
typedef struct RwpWorld { char pad[0x14]; unsigned bound; char pad2[0x1c]; RwpHandle *list; int pad4[2]; unsigned count; char pad3[0x28]; unsigned *bits; } RwpWorld;
typedef RwpHandle *(*RwpVisit)(RwpHandle *h, void *user);
/* Calls a callback for every active handle of the world until it returns something other than the handle; returns the world. (Rwp library) */
RwpWorld *rwp_volume_0056b590(RwpWorld *w, RwpVisit visit, void *user)
{
  RwpHandle *h = w->list;
  unsigned i;
  for (i = 0; i < w->count; i++, h++) {
    if (w == h->w && h->id < w->bound && (w->bits[h->id >> 5] & (1 << (h->id & 0x1f)))) {
      if (visit(h, user) != h) break;
    }
  }
  return w;
}
