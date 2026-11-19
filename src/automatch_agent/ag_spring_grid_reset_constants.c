#include "mfl_tier2.h"
/*006f2abc*/ extern float g_spring_grid_6f2abc;
/*006f2ac0*/ extern float g_spring_grid_6f2ac0;
/*006f2ac4*/ extern float g_spring_grid_6f2ac4;
/*006f2ac8*/ extern float g_spring_grid_6f2ac8;
/*006f2acc*/ extern float g_spring_grid_6f2acc;
/*006f2ad0*/ extern float g_spring_grid_6f2ad0;
/*006f2ad4*/ extern float g_spring_grid_6f2ad4;


/* spring_grid_reset_constants */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the default constants of the spring grid" */

void spring_grid_reset_constants(void)

{
  g_spring_grid_6f2abc = -4.0f;
  g_spring_grid_6f2ac0 = 1350.0f;
  g_spring_grid_6f2ac4 = 1700.0f;
  g_spring_grid_6f2ac8 = 2.0f;
  g_spring_grid_6f2acc = 0.0f;
  g_spring_grid_6f2ad0 = 0.70999998f;
  g_spring_grid_6f2ad4 = 100.0f;
  return;
}
