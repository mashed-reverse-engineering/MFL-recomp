/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpWorld { int *far_field; } RwpWorld;
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
/* Resets the far-field entry of a body. (Rwp library)
   Returns FUN_00576f40's 16-bit result (tail jump); rwp_collision_update compares it (cmp ax,6 / 7 at 0x48f789 ...).
   VOID_w16: cl 13 schedules the tail call differently when the result is returned, so the byte-matching cl 13 build keeps
   the void form (the callee's AX passes through, as in the original); every other compiler gets the real return. */
#if defined(_MSC_VER) && _MSC_VER < 1310
extern void FUN_00576f40(int *far_field, unsigned short id);
void rwp_volume_0056b420(RwpWorld *w, RwpBody *b)
{
  FUN_00576f40(w->far_field, b->id);
}
#else
extern unsigned short FUN_00576f40(int *far_field, unsigned short id);
unsigned short rwp_volume_0056b420(RwpWorld *w, RwpBody *b)
{
  return FUN_00576f40(w->far_field, b->id);
}
#endif
