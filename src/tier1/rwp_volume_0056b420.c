/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpWorld { int *far_field; } RwpWorld;
typedef struct RwpBody { char pad[0x20]; unsigned short id; } RwpBody;
extern void FUN_00576f40(int *far_field, unsigned short id);
/* Resets the far-field entry of a body. (Rwp library) */
void rwp_volume_0056b420(RwpWorld *w, RwpBody *b)
{
  FUN_00576f40(w->far_field, b->id);
}
