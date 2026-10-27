#include "mfl_tier2.h"
/*0042d9e0*/ extern double hud_get_8c8798(void);
/*005dd330*/ extern float float_1_0;
/*005dd334*/ extern float k_f_0_033333335;
/*005de244*/ extern float k_f_500;
/*005de8f4*/ extern float k_f_470;
/* intro_banner_slide_y */

/* "(t): 470 - (1 - min(t,30)/30) * 500, vertical slide of the intro banner" */
float intro_banner_slide_y(void)
{
  int n;

  n = (int)hud_get_8c8798();
  if (30 < n) {
    n = 30;
  }
  return k_f_470 - (float_1_0 - (float)n * k_f_0_033333335) * k_f_500;
}
