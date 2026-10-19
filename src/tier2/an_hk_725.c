#include "mfl_tier2.h"
/*0048d540*/ extern int __cdecl course_collision_object_build(undefined4 param_1, int param_2);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*006f35c8*/ extern undefined4 g_course_6f35c8[0x1000];
/*006f4b48*/ extern undefined4 g_course_6f4b48[0x1000];
/*006f6908*/ extern undefined4 g_course_6f6908;
/*006f690c*/ extern undefined4 g_course_6f690c;

#define COLLISION_RECORD_DWORDS (0x56)      /* one g_course_6f35c8 record: 0x158 bytes */
#define COLLISION_SNAPSHOT_DWORDS (0x560)   /* 16 records copied to g_course_6f4b48 */
#define ATOMIC_LIST_CAPACITY (16)           /* local array size for clump_get_atomics */

/* course_collision_objects_build */

/* "builds g_course_6f35c8[0] records (0x158 bytes) from the clump frames" */

/* Builds the course collision object records from a clump.
 * clump: RpClump pointer passed as int (its frame is object.parent, +4; its atomics come from clump_get_atomics).
 * Updates the clump's frame, then calls course_collision_object_build once per atomic, filling
 * consecutive COLLISION_RECORD_DWORDS-sized records of g_course_6f35c8. The atomic count is kept in
 * g_course_6f690c and copied to g_course_6f6908 (inferred: number of collision objects built).
 * Finally the whole record table is copied to g_course_6f4b48 (inferred: snapshot for later use).
 * Returns nothing. */

void __cdecl course_collision_objects_build(int clump)

{
  int index;
  undefined4 *record;
  undefined4 *snapshot_dst;
  int atomics [ATOMIC_LIST_CAPACITY];

  g_course_6f690c = clump_get_atomics((RpClump *)clump,(RpAtomic **)atomics);
  RwFrameUpdateObjects(RpClump_object_parent(((RpClump *)clump)));
  index = 0;
  /* one record per atomic */
  if (0 < (int)g_course_6f690c) {
    record = g_course_6f35c8;
    do {
      course_collision_object_build((undefined4)record,atomics[index]);
      index = index + 1;
      record = record + COLLISION_RECORD_DWORDS;
    } while (index < (int)g_course_6f690c);
  }
  g_course_6f6908 = g_course_6f690c;
  record = g_course_6f35c8;
  snapshot_dst = g_course_6f4b48;
  /* copy the whole record table, dword by dword */
  for (index = COLLISION_SNAPSHOT_DWORDS; index != 0; index = index + -1) {
    *snapshot_dst = *record;
    record = record + 1;
    snapshot_dst = snapshot_dst + 1;
  }
  return;
}


