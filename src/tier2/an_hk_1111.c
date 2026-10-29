#include "mfl_tier2.h"
/* gap_4b24d0 */

/* no context */

void gap_4b24d0(char *param_1)

{
  int *target;

  /* param_1 - 4 forwarded to gap_4b2490: method +4 of the object at param_1+8, passing that object */
  target = *(int **)(param_1 + 8);
  (**(void (**)(int *))(*target + 4))(target);
  return;
}


