#include "mfl_tier2.h"
/* gap_4b24a0 */

/* no context */

void gap_4b24a0(char *param_1)

{
  int *target;

  /* tail call of method +8 of the object at param_1+0xc, passing that object */
  target = *(int **)(param_1 + 0xc);
  (**(void (**)(int *))(*target + 8))(target);
  return;
}
