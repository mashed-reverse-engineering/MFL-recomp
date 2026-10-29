#include "mfl_tier2.h"
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/* gamesave_set_result_code */

/* Sets the gamesave result code. */

void gamesave_set_result_code(int result_code)
{
  g_gamesave_result_code = result_code;
  return;
}
