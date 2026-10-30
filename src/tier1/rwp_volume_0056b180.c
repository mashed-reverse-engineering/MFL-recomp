/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
typedef struct RwpWorld { int *far_field; char pad[0x64]; unsigned *active_bits; } RwpWorld;
typedef struct RwpFarState { int w[8]; } RwpFarState;
extern void rwp_farfield_00576bf0(int *far_field, unsigned short id, RwpFarState *out);
/* Copies the far-field state of body b into out when the body is active; returns out, or 0 when it is not. (Rwp library) */
RwpFarState *rwp_volume_0056b180(RwpWorld *w, RwpBody *b, RwpFarState *out)
{
  RwpFarState *r = 0;
  unsigned short id = b->id;
  if (BIT_TEST(w->active_bits, id)) {
    rwp_farfield_00576bf0(w->far_field, id, out);
    r = out;
  }
  return r;
}
