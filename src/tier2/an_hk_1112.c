#include "mfl_tier2.h"
/* gap_4b24e0 */

/* no context */

void gap_4b24e0(char *param_1)

{
  int *target;

  /* param_1 - 4 forwarded to gap_4b24a0: method +8 of the object at param_1+8, passing that object */
  target = *(int **)(param_1 + 8);
  (**(void (**)(int *))(*target + 8))(target);
  return;
}


