/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBody RwpBody;
struct RwpBody { void (**vtbl)(RwpBody *, void *); char pad[0x20]; void *owner; };
extern void rwp_volume_0056b100(void *world, RwpBody *b, void *state);
/* Reads the object's state through its virtual method and hands it to the volume (rwp_volume_0056b100). (Rwp library) */
RwpBody *rwp_obj_0056e610(RwpBody *b)
{
  char state[32];
  b->vtbl[6](b, state);
  rwp_volume_0056b100(b->owner, b, state);
  return b;
}
