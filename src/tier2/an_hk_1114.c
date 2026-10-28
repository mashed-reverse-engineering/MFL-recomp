#include "mfl_tier2.h"
/* gap_4c4200 */

/* no context */

/* Appends item to the array list[0] (if non-null) and increments the counter list[1] points to; with a null array it only counts (inferred). */
undefined4 gap_4c4200(undefined4 item,int *list)

{
  int *count_ptr;
  
  count_ptr = (int *)list[1];
  if (*list != 0) {
    *(undefined4 *)(*list + *count_ptr * 4) = item;
    *count_ptr = *count_ptr + 1;
    return item;
  }
  *count_ptr = *count_ptr + 1;
  return item;
}


