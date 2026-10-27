#include "mfl_tier2.h"
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*006a27e8*/ extern undefined4 g_course_atomics_6a27e8;
/*006a2830*/ extern struct RpAtomic *g_course_atomics_6a2830;
/*006a2834*/ extern undefined4 g_course_atomics_6a2834;
/*006a2838*/ extern struct RpAtomic *g_course_atomics_6a2838;
/*006a283c*/ extern undefined4 g_course_atomics_6a283c;
/*006a2840*/ extern struct RpAtomic *g_course_atomics_6a2840;
/*006a2844*/ extern undefined4 g_course_atomics_6a2844;
#define COURSE_CLUMP_OFFSET (0xc12c)
#define ATOMIC_MATRIX_0_OFFSET (0x5400)
#define ATOMIC_MATRIX_1_OFFSET (0x5440)
#define ATOMIC_MATRIX_2_OFFSET (0x5480)
/* course_atomics_clone_3 */
/* "clones the atomic of the world record three times (g_course_6a2830 / 6a2838 / 6a2840)" */
/*
 * Makes three copies of the course's world record atomic (the first atomic of the
 * course clump) into g_course_atomics_6a2830 / 6a2838 / 6a2840. Each copy gets its
 * own new frame, placed with one of the three matrices in the course state.
 * param course: course state block (char *); the clump is at COURSE_CLUMP_OFFSET.
 * Returns nothing; globals 6a2834 / 6a283c / 6a2844 / 6a27e8 are cleared.
 */
void course_atomics_clone_3(char *course)
{
  RpAtomic *base_atomic;
  RwFrame *frame;

  /* clone the first atomic of the course clump three times */
  base_atomic = (RpAtomic *)clump_first_atomic(*(undefined4 *)(course + COURSE_CLUMP_OFFSET));
  g_course_atomics_6a2830 = RpAtomicClone(base_atomic);
  g_course_atomics_6a2838 = RpAtomicClone(base_atomic);
  g_course_atomics_6a2840 = RpAtomicClone(base_atomic);
  /* give each clone its own frame */
  frame = RwFrameCreate();
  RpAtomicSetFrame(g_course_atomics_6a2830,frame);
  frame = RwFrameCreate();
  RpAtomicSetFrame(g_course_atomics_6a2838,frame);
  frame = RwFrameCreate();
  RpAtomicSetFrame(g_course_atomics_6a2840,frame);
  /* place each clone's frame with its matrix in the course state */
  RwFrameTransform(RpAtomic_object_object_parent(g_course_atomics_6a2830),(RwMatrix *)(course + ATOMIC_MATRIX_0_OFFSET),
                   rwCOMBINEREPLACE);
  RwFrameTransform(RpAtomic_object_object_parent(g_course_atomics_6a2838),(RwMatrix *)(course + ATOMIC_MATRIX_1_OFFSET),
                   rwCOMBINEREPLACE);
  RwFrameTransform(RpAtomic_object_object_parent(g_course_atomics_6a2840),(RwMatrix *)(course + ATOMIC_MATRIX_2_OFFSET),
                   rwCOMBINEREPLACE);
  /* reset the companion globals */
  g_course_atomics_6a2834 = 0;
  g_course_atomics_6a283c = 0;
  g_course_atomics_6a2844 = 0;
  g_course_atomics_6a27e8 = 0;
  return;
}
