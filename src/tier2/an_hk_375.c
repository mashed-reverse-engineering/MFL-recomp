#include "mfl_tier2.h"
/*008bf16c*/ extern undefined4 g_course_object_8bf16c;

/* Size of one course object record, in 4-byte words (inferred) */
#define COURSE_OBJECT_RECORD_WORDS (0x3f)

/* course_object_get_float_16c */

/* Returns the float at the start of course object record `object_index`
   in g_course_object_8bf16c. Records are COURSE_OBJECT_RECORD_WORDS words
   long; the float is the record's first word (inferred). */

float10 course_object_get_float_16c(int object_index)

{
  return (float10)*(float *)&(&g_course_object_8bf16c)[object_index * COURSE_OBJECT_RECORD_WORDS];
}


