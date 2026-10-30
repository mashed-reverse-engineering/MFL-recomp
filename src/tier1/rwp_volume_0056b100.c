/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpWorld { int *far_field; char pad[0x64]; unsigned *active_bits; } RwpWorld;
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
extern void FUN_00576c50(int *far_field, unsigned short id);
extern void FUN_00576670(int *far_field, unsigned short id, int arg);
/* Removes a body's far-field entry when it is active in the world, then re-adds it with the given state (arg non-zero) and marks it active. (Rwp library) */
RwpWorld *rwp_volume_0056b100(RwpWorld *w, RwpBody *b, int arg)
{
  unsigned bit = 1 << (b->id & 0x1f);
  if (w->active_bits[b->id >> 5] & bit) FUN_00576c50(w->far_field, b->id);
  if (arg) {
    FUN_00576670(w->far_field, b->id, arg);
    w->active_bits[b->id >> 5] |= 1 << (b->id & 0x1f);
  }
  return w;
}
