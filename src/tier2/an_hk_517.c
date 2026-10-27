#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00486710*/ extern int particles_destroy();
/*004a5da0*/ extern int object_destroy_frames();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*006a1818*/ extern undefined4 g_course_6a1818;
/*006a2690*/ extern undefined4 g_course_highway_6a2690;
/*006a2694*/ extern undefined4 g_course_highway_6a2694;
/*006a2698*/ extern undefined4 g_course_highway_6a2698;
/*006a269c*/ extern undefined4 g_course_highway_6a269c;
/*006a26a0*/ extern undefined4 g_course_highway_6a26a0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c3fc4*/ extern char g_course_highway_8c3fc4[];
/*008bf088*/ extern char g_course_sands_8bf088[];
/* course_highway_cb5 */
#define COURSE_HIGHWAYN (12)
#define FIRST_GROUP_OFFSET (0x7c) /* byte offset into g_course_sands_8bf088 of the first group (inferred) */
#define GROUP_STRIDE_INTS (0x3f) /* one group is 0x3f ints (0xfc bytes) (inferred) */
#define ATOMIC_LIST_BACK_INTS (0x16) /* the atomic list starts this many ints before the group (inferred) */
#define ATOMIC_SLOT_STRIDE_INTS (6) /* one list entry is 6 ints (inferred) */
#define GROUP_MAIN_ATOMIC (0x10) /* group slot 16: main atomic (inferred) */
#define GROUP_OPTIONAL_ATOMIC (0x11) /* group slot 17: optional atomic, may be NULL (inferred) */
#define GROUP_AUX_ATOMIC (0xf) /* group slot 15: auxiliary atomic (inferred) */


/*
 * Course teardown for Highway (inferred). Destroys the scenery frames of five object lists,
 * then every atomic (and its frame) in the group table at g_course_sands_8bf088. Afterwards, on
 * COURSE_HIGHWAYN only, destroys the particle system in g_course_6a1818 and frees it.
 * Takes no parameters and returns nothing.
 */
void course_highway_cb5(void)

{
  RpAtomic *atomic;
  RwFrame *frame;
  CourseId current_course;
  int atomic_index;
  int *group;
  int *atomic_slot;

  object_destroy_frames(g_course_highway_6a2690);
  object_destroy_frames(g_course_highway_6a2694);
  object_destroy_frames(g_course_highway_6a2698);
  object_destroy_frames(g_course_highway_6a269c);
  object_destroy_frames(g_course_highway_6a26a0);
  group = (int *)((char *)&g_course_sands_8bf088 + FIRST_GROUP_OFFSET);
  /* Loop over the groups until the end of g_course_highway_8c3fc4 (inferred: each group holds
     an atomic count at [0], a list of atomics, and a few fixed atomic slots). */
  do {
    atomic = (RpAtomic *)group[GROUP_MAIN_ATOMIC];
    frame = RpAtomic_object_object_parent(atomic);
    RwFrameRemoveChild(frame);
    RpAtomicSetFrame(atomic,(RwFrame *)0x0);
    RwFrameDestroy(frame);
    RpAtomicDestroy(atomic);
    /* Optional atomic: same teardown, only if the slot is set. */
    atomic = (RpAtomic *)group[GROUP_OPTIONAL_ATOMIC];
    if (atomic != (RpAtomic *)0x0) {
      frame = RpAtomic_object_object_parent(atomic);
      RwFrameRemoveChild(frame);
      RpAtomicSetFrame(atomic,(RwFrame *)0x0);
      RwFrameDestroy(frame);
      RpAtomicDestroy(atomic);
    }
    /* Walk the list of *group atomics. Each entry: detach its frame from the parent,
       detach the atomic from the frame, then destroy frame and atomic. */
    atomic_index = 0;
    if (0 < *group) {
      atomic_slot = group + -ATOMIC_LIST_BACK_INTS;
      do {
        atomic = (RpAtomic *)*atomic_slot;
        frame = RpAtomic_object_object_parent(atomic);
        RwFrameRemoveChild(frame);
        RpAtomicSetFrame(atomic,(RwFrame *)0x0);
        RwFrameDestroy(frame);
        RpAtomicDestroy(atomic);
        atomic_index = atomic_index + 1;
        atomic_slot = atomic_slot + ATOMIC_SLOT_STRIDE_INTS;
      } while (atomic_index < *group);
    }
    /* Auxiliary atomic: destroyed without removing its frame from the parent (inferred). */
    atomic = (RpAtomic *)group[GROUP_AUX_ATOMIC];
    frame = RpAtomic_object_object_parent(atomic);
    RpAtomicSetFrame(atomic,(RwFrame *)0x0);
    RwFrameDestroy(frame);
    RpAtomicDestroy(atomic);
    group = group + GROUP_STRIDE_INTS;
  } while ((LONG_PTR)group < ((LONG_PTR)&g_course_highway_8c3fc4));
  /* Two unnamed helpers run after the groups are gone (purpose not known). */
  func_0x004541b0();
  func_0x00453c10();
  /* Particle system cleanup belongs to Highway only. */
  current_course = current_course_id();
  if (current_course == COURSE_HIGHWAYN) {
    particles_destroy(g_course_6a1818);
    RwFree(g_course_6a1818);
    g_course_6a1818 = (void *)0x0;
    return;
  }
  return;
}



