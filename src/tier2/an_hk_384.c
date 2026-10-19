#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*006af5c0*/ extern undefined1 g_car_6af5c0;
/*006af6ec*/ extern undefined4 g_car_6af6ec;
/*006c2680*/ extern undefined4 g_course_object_count;
#define COURSE_OBJECT_STRIDE (0x4c) /* entry size in 4-byte words */
#define COURSE_OBJECT_FRAME_WORD (0x11) /* word index of the entry's RwFrame * */
#define COURSE_OBJECT_FLAG_COUNT (0x100) /* one active bit per entry */
/* course_objects_reset_transforms */

/*
 * Resets the course object table at the start of a course.
 * For each of g_course_object_count entries: sets its active bit in g_car_6af5c0,
 * zeroes the two words before and at the entry start (inferred: state/flags),
 * and applies the entry's matrix (at +0x4) to its RwFrame (at +0x44).
 * Then clears the active bits of the remaining slots up to COURSE_OBJECT_FLAG_COUNT.
 * Takes no parameters and returns nothing.
 */
void course_objects_reset_transforms(void)

{
  byte bit_index;
  int byte_index;
  int object_index;
  undefined4 *course_object;

  object_index = 0;
  if (0 < (int)g_course_object_count) {
    course_object = &g_car_6af6ec;
    /* loop over live entries: activate bit, reset state, transform frame */
    do {
      (&g_car_6af5c0)[object_index >> 3] = (&g_car_6af5c0)[object_index >> 3] | '\x01' << ((byte)object_index & 7);
      course_object[-1] = 0;
      *course_object = 0;
      RwFrameTransform((RwFrame *)course_object[COURSE_OBJECT_FRAME_WORD],(RwMatrix *)(course_object + 1),rwCOMBINEREPLACE);
      object_index = object_index + 1;
      course_object = course_object + COURSE_OBJECT_STRIDE;
    } while (object_index < (int)g_course_object_count);
    if ((COURSE_OBJECT_FLAG_COUNT - 1) < object_index) {
      return;
    }
  }
  /* clear the active bits of all slots past the live entries */
  do {
    byte_index = object_index >> 3;
    bit_index = (byte)object_index;
    object_index = object_index + 1;
    (&g_car_6af5c0)[byte_index] = (&g_car_6af5c0)[byte_index] & ~('\x01' << (bit_index & 7));
  } while (object_index < COURSE_OBJECT_FLAG_COUNT);
  return;
}


