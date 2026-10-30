/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_SET(bits, id) ((bits)[(id) >> 5] |= 1 << ((id) & 0x1f))
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
typedef struct RwpWorld { int *far_field; char pad[0x64]; unsigned *active_bits; } RwpWorld;
typedef struct RwpFarState { int w[8]; } RwpFarState;
extern void rwp_farfield_00576f60(int *far_field, unsigned short id, unsigned short arg);
extern void rwp_farfield_00576bf0(int *far_field, unsigned short id, RwpFarState *out);
extern void rwp_farfield_00576c50(int *far_field, unsigned short id);
extern void rwp_farfield_00576670(int *far_field, unsigned short id, RwpFarState *in);
/* Moves a body's far-field entry: saves its state if active, resets it, removes the old entry and re-adds the saved state. (Rwp library) */
RwpWorld *rwp_volume_0056b440(RwpWorld *w, RwpBody *b, unsigned short arg)
{
  RwpFarState state;
  RwpFarState *saved;
  rwp_farfield_00576f60(w->far_field, b->id, arg);
  saved = 0;
  if (BIT_TEST(w->active_bits, b->id)) {
    rwp_farfield_00576bf0(w->far_field, b->id, &state);
    saved = &state;
  }
  if (BIT_TEST(w->active_bits, b->id)) rwp_farfield_00576c50(w->far_field, b->id);
  if (saved) {
    rwp_farfield_00576670(w->far_field, b->id, saved);
    BIT_SET(w->active_bits, b->id);
  }
  return w;
}
