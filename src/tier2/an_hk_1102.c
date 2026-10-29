#include "mfl_tier2.h"
/* gap_4af2f0 */

/* no context */

void gap_4af2f0(char *param_1)

{
  char *component;

  /* tail call of method +4 of the component embedded at +0xc of the object at param_1+0x28, passing the component */
  component = *(char **)(param_1 + 0x28) + 0xc;
  (**(void (**)(char *))(*(char **)component + 4))(component);
  return;
}


