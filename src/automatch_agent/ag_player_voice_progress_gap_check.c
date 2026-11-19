#include "mfl_tier2.h"
/*00409f30*/ extern float player_progress_b();
/*005dd620*/ extern float float_10_0;
/*005df998*/ extern float k_f_n15;


/* player_voice_progress_gap_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "true when the progress gap between two players is in (-15, 10)" */

int __cdecl player_voice_progress_gap_check(int param_1)
{
  float a = player_progress_b(0);
  float d = a - player_progress_b(param_1);
  if (d > k_f_n15 && d < float_10_0) {
    return 1;
  }
  return 0;
}
