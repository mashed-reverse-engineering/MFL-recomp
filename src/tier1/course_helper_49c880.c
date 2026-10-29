#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*0075cda8*/ extern undefined g_particles_75cda8;
/*0075cdcc*/ extern undefined g_particles_75cdcc;
/*0075cdec*/ extern undefined g_particles_75cdec;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_helper_49c880 */

/* callers course_rouabout_cb5,course_training_cb5 */

void course_helper_49c880(int param_1)

{
  RpAtomic *atomic;
  RwFrame *frame;
  
  if (*(void **)((char *)&g_particles_75cdec + param_1 * 4) != (void *)0x0) {
    RwFree(*(void **)((char *)&g_particles_75cdec + param_1 * 4));
    ((undefined4 *)&g_particles_75cdec)[param_1] = 0;
    ((undefined4 *)&g_particles_75cda8)[param_1] = 0;
  }
  atomic = *(RpAtomic **)((char *)&g_particles_75cdcc + param_1 * 4);
  if (atomic != (RpAtomic *)0x0) {
    frame = RpAtomic_object_object_parent(atomic);
    RpAtomicSetFrame(atomic,(RwFrame *)0x0);
    RpAtomicDestroy_00545d00(*(RpAtomic **)((char *)&g_particles_75cdcc + param_1 * 4));
    RwFrameDestroy(frame);
    ((undefined4 *)&g_particles_75cdcc)[param_1] = 0;
  }
  return;
}
