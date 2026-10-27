#include "mfl_tier2.h"
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;


/* gamesave_set_autosave_enabled */

/* "0x8D8C0C = value (autosave_request only acts when it is set)" */

/* Sets the autosave-enabled flag (stored at 0x8D8C0C). */
void __cdecl gamesave_set_autosave_enabled(undefined4 enabled)
{
  g_gamesave_flag_c0c = enabled;
  return;
}
