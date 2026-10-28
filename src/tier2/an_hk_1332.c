#include "mfl_tier2.h"
/*005dda20*/ extern float k_f_4_2949673e9;
/* gap_5d9a20 */

/* no context */

/* Stores *value_ptr (read as unsigned, converted to float) at target+0x60 of obj->+0x20 and sets flag 0x20 at target+0x5c (inferred). */
char *gap_5d9a20(char *obj,char *unused_2,int *value_ptr)

{
  char *target;
  float value_f;
  
  target = *(char **)(obj + 0x20);
  value_f = (float)*value_ptr;
  if (*value_ptr < 0) {
    value_f = value_f + k_f_4_2949673e9;
  }
  *(float *)(target + 0x60) = value_f;
  *(uint *)(target + 0x5c) = *(uint *)(target + 0x5c) | 0x20;
  return obj;
}


