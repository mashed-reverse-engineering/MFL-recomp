#include "mfl_tier2.h"
/* course_helper_484b20 */

/* callers course_forest_cb1,course_keys_cb1,course_nt1_cb1,course_nt1_s_cb1 */

/* Advances cursor's timer (+0xc) by delta; once it exceeds 299 moves to the next list node (**(cursor+4)) and resets it; returns node[1] (inferred). */
undefined4 course_helper_484b20(char *cursor,int delta)

{
  int next_node;
  
  delta = *(int *)(cursor + 0xc) + delta;
  *(int *)(cursor + 0xc) = delta;
  if (299 < delta) {
    next_node = **(int **)(cursor + 4);
    *(int *)(cursor + 4) = next_node;
    *(undefined4 *)(cursor + 0xc) = 0;
    return *(undefined4 *)(next_node + 4);
  }
  return *(undefined4 *)(*(char **)(cursor + 4) + 4);
}


