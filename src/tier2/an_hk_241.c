#include "mfl_tier2.h"
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6c0*/ extern int rwp_object_set_flag();
/*0060d894*/ extern undefined4 g_course_egypt_60d894;
/*0060d898*/ extern undefined4 g_course_egypt_60d898;
/*0060d89c*/ extern undefined4 g_course_egypt_60d89c;
/*0060d8a0*/ extern undefined4 g_course_egypt_60d8a0;

#define FIRST_OBJECT_ID (6000)
#define PAIR_ID_OFFSET (1000)
#define PAIR_COUNT (3)

/* course_egypt_cb7 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/*
 * Egypt course setup (inferred from the body): takes the object ids
 * FIRST_OBJECT_ID .. FIRST_OBJECT_ID + PAIR_COUNT - 1 and, for each one, looks up
 * the object with that id and with the id PAIR_ID_OFFSET lower, and sets its
 * flag to 1 (meaning of the flag: inferred). Then sets four course globals to 1
 * (meaning: inferred). Takes no parameters, returns nothing.
 */
void course_egypt_cb7(void)

{
  undefined4 object_index;
  int object_id;
  int pairs_left;

  object_id = FIRST_OBJECT_ID;
  pairs_left = PAIR_COUNT;
  /* for each id pair: flag the object PAIR_ID_OFFSET below, then the object itself */
  do {
    object_index = rwp_object_find_index_by_id(object_id + -PAIR_ID_OFFSET);
    rwp_object_set_flag(object_index,1);
    object_index = rwp_object_find_index_by_id(object_id);
    rwp_object_set_flag(object_index,1);
    object_id = object_id + 1;
    pairs_left = pairs_left + -1;
  } while (pairs_left != 0);
  /* four course globals set to 1 (inferred: enable flags) */
  g_course_egypt_60d894 = 1;
  g_course_egypt_60d898 = 1;
  g_course_egypt_60d89c = 1;
  g_course_egypt_60d8a0 = 1;
  return;
}


