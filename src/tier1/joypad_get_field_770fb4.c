#include "mfl_tier2.h"
/*00770fb4*/ extern undefined4 g_app_770fb4;


/* joypad_get_field_770fb4 */

/* "(index): g_app_770fb4[index]" */

/* Returns the joypad field at the given index of g_app_770fb4. */
undefined4 __cdecl joypad_get_field_770fb4(int field_index)
{
  return (&g_app_770fb4)[field_index];
}
