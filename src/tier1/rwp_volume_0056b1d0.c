/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_SET(bits, id) ((bits)[(id) >> 5] |= 1 << ((id) & 0x1f))
#define BIT_CLEAR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
typedef struct RwpWorld { char pad[0x5c]; unsigned *flag_bits; } RwpWorld;
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
/* Sets or clears the flag bit of a body in the world's flag set; returns the world. (Rwp library) */
RwpWorld *rwp_volume_0056b1d0(RwpWorld *w, RwpBody *b, int set)
{
  if (set) BIT_SET(w->flag_bits, b->id);
  else BIT_CLEAR(w->flag_bits, b->id);
  return w;
}
