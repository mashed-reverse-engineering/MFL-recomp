#include "mfl_tier2.h"
/*006512b0*/ extern float g_fx_6512b0[];
/*00651aa8*/ extern int g_fx_651aa8;


/* fx_current_object_get_float */

/* "float at +0x1C of the current fx record" */

float fx_current_object_get_float(void)

{
  return g_fx_6512b0[g_fx_651aa8 * 0x3b];
}
