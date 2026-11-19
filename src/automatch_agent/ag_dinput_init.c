#include "mfl_tier2.h"


/* dinput_init */

/* "dinput_create() != 0" */

bool dinput_init(void)

{
  int iVar1;
  
  iVar1 = dinput_create();
  return iVar1 != 0;
}
