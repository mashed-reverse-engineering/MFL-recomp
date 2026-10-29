#include "mfl_tier2.h"
/* gap_4af310 */

/* no context */

void gap_4af310(char *param_1)

{
  char *component;

  /* tail call of method +8 of the component embedded at +0xc of the object at param_1+0x28, passing the component */
  component = *(char **)(param_1 + 0x28) + 0xc;
  (**(void (**)(char *))(*(char **)component + 8))(component);
  return;
}
