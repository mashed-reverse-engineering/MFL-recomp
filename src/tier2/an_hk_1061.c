#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*004a5c20*/ extern int object_frames_create();
/*004a5e50*/ extern int object_set_colour();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd378*/ extern float k_f_0_1;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_warzone_cb0_sub_45d750 */

/* single caller course_warzone_cb0 */

int * course_warzone_cb0_sub_45d750(char *clump,int count)

{
  float old_x;
  int *group;
  undefined4 atomic;
  RwV3d *items;
  int frames_or_i;
  RwV3d *item_cursor;
  double rand_val;
  
  group = RwMalloc(0xc,0);
  atomic = (undefined4)(clump_first_atomic((RpClump *)clump));
  items = RwMalloc(count * 0x30,0);
  group[1] = (int)items;
  *group = count;
  frames_or_i = object_frames_create(atomic,count);
  group[2] = frames_or_i;
  object_set_colour(frames_or_i,0xffffffff,0x3ecccccd,0x3ecccccd,0x3e4ccccd,0x3f800000);
  frames_or_i = 0;
  if (0 < *group) {
    item_cursor = items + 1;
    do {
      rand_val = (double)random_range(0xbf000000,0x3f000000);
      items->x = (float)rand_val;
      rand_val = (double)random_range(0x3f000000,0x3f800000);
      item_cursor[-1].y = (float)rand_val;
      rand_val = (double)random_range(0,0x3f000000);
      item_cursor[-1].z = (float)rand_val;
      RwV3dNormalize(items,items);
      rand_val = (double)random_range(0xbf800000,0x3f800000);
      item_cursor->x = (float)rand_val;
      rand_val = (double)random_range(0xbf800000,0x3f800000);
      item_cursor->y = (float)rand_val;
      rand_val = (double)random_range(0xbf800000,0x3f800000);
      item_cursor->z = (float)rand_val;
      RwV3dNormalize(item_cursor,item_cursor);
      old_x = items->x;
      item_cursor[2].x = 0.0;
      items->x = old_x * k_f_0_1;
      frames_or_i = frames_or_i + 1;
      item_cursor[-1].y = item_cursor[-1].y * k_f_0_1;
      item_cursor[-1].z = item_cursor[-1].z * k_f_0_1;
      item_cursor = item_cursor + 4;
      items = items + 4;
    } while (frames_or_i < *group);
  }
  return group;
}


