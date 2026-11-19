#include "mfl_tier2.h"
/*004a5da0*/ extern int object_destroy_frames();
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *world, struct RpAtomic *atomic);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *atomic);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *atomic);
/*006af6e4*/ extern undefined4 g_car_6af6e4;
/*006c2658*/ extern undefined4 g_car_6c2658;
/*006c2680*/ extern undefined4 g_course_object_count;
/*006c2684*/ extern undefined4 g_course_object_pair_count;


/* course_objects_destroy_atomics */

/* "removes and destroys the atomic of each entry" */

void course_objects_destroy_atomics(void)

{
  RpWorld *world;
  undefined4 *entry;
  int index;
  
  index = 0;
  if (0 < (int)g_course_object_count) {
    entry = &g_car_6af6e4;
    do {
      if ((RpAtomic *)*entry != (RpAtomic *)0x0) {
        world = RpAtomicGetWorld((RpAtomic *)*entry);
        if (world != (RpWorld *)0x0) {
          RpWorldRemoveAtomic(world,(RpAtomic *)*entry);
        }
        RpAtomicDestroy((RpAtomic *)*entry);
        *entry = 0;
        entry[0x13] = 0;
      }
      index = index + 1;
      entry = entry + 0x4c;
    } while (index < (int)g_course_object_count);
  }
  index = 0;
  if (0 < (int)g_course_object_pair_count) {
    do {
      object_destroy_frames((&g_car_6c2658)[index]);
      index = index + 1;
    } while (index < (int)g_course_object_pair_count);
  }
  g_course_object_count = 0;
  g_course_object_pair_count = 0;
  return;
}
