#include "mfl_tier2.h"

#define COURSE_RECORD_STRIDE 0x1a /* 0x68 bytes per record (26 dwords) */
#define COURSE_RECORDS_END ((int)&g_course_unload_6af5cc)

/*004a5da0*/ extern int object_destroy_frames();
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*006ae24c*/ extern undefined4 g_course_unload_6ae24c;
/*006af594*/ extern undefined4 g_course_unload_6af594;
/*006af5cc*/ extern char g_course_unload_6af5cc[];


/* course_unload_atomics_6ae24c */

/* Destroys the atomics across all course records and releases course object frames. */

void course_unload_atomics_6ae24c(void)
{
  undefined4 *record_atomic;
  
  record_atomic = &g_course_unload_6ae24c;
  /* Destroy atomics in all 0x68-byte course records */
  do {
    if ((RpAtomic *)*record_atomic != (RpAtomic *)0x0) {
      RpAtomicDestroy((RpAtomic *)*record_atomic);
      *record_atomic = 0;
    }
    record_atomic = record_atomic + COURSE_RECORD_STRIDE;
  } while ((int)record_atomic < COURSE_RECORDS_END);
  /* Destroy frames for the course object if allocated */
  if (g_course_unload_6af594 != 0) {
    object_destroy_frames(g_course_unload_6af594);
    g_course_unload_6af594 = 0;
  }
  return;
}
