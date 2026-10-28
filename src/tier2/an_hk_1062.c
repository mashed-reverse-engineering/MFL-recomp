#include "mfl_tier2.h"
/* course_warzone_cb1_sub_45da00 */

/* single caller course_warzone_cb1_sub_45da60 */

/* Spawns into every free (life == 0) entry of the 0x30-byte-stride pool at src_obj+0x30..0x38 position with life 100 and scale 1.0 (inferred). */
void course_warzone_cb1_sub_45da00(int *pool,char *src_obj)

{
  int *life_ptr;
  int index;
  
  index = 0;
  if (0 < *pool) {
    life_ptr = (int *)(pool[1] + 0x24);
    do {
      if (*life_ptr == 0) {
        life_ptr[-3] = *(int *)(src_obj + 0x30);
        life_ptr[-2] = *(int *)(src_obj + 0x34);
        life_ptr[-1] = *(int *)(src_obj + 0x38);
        *life_ptr = 100;
        life_ptr[2] = 0x3f800000;
      }
      life_ptr = life_ptr + 0xc;
      index = index + 1;
    } while (index < *pool);
  }
  return;
}


