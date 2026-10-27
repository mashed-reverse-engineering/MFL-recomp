// cluster-entries: course_warzone_cb0_sub_45d6b0
/* Installed cluster course_warzone_cb0_sub_45d6b0 (tools/cluster_install.py from output/cluster_pilot_kb/course_warzone_cb0_sub_45d6b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_warzone_cb0_sub_45d6b0 */

/* single caller course_warzone_cb0 */

void * course_warzone_cb0_sub_45d6b0(int count, int base_ptr)

{
  void *items;
  float *item_cursor;
  float10 rand_val;
  
  items = RwMalloc(count * 0xc,0);
  if (0 < count) {
    item_cursor = (float *)((int)items + 8);
    do {
      *(int *)&item_cursor[-2] = base_ptr;
      rand_val = (float10)random_range(0,0x40a00000);
      item_cursor[-1] = (float)rand_val;
      rand_val = (float10)random_range(0x40000000,0x40a00000);
      *item_cursor = (float)rand_val;
      item_cursor = item_cursor + 3;
      base_ptr = base_ptr + 0x40;
      count = count + -1;
    } while (count != 0);
  }
  return items;
}

