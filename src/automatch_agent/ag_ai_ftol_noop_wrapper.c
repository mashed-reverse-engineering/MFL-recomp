#include "mfl_tier2.h"
extern float g_ftol_tbl_8c9eb0[];
int ai_ftol_noop_wrapper(int i)

{
  return (int)g_ftol_tbl_8c9eb0[i];
}
