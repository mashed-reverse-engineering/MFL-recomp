#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define PARTICLES_ATOMIC_OFFSET (4)
#define PARTICLES_SIZE (0x40)
/* particles_destroy */

/* Tears down one particle effect block (inferred from the name): detaches its atomic from the
   frame, destroys the frame and the atomic, frees each owned buffer at +0x10..+0x28 that is
   non-NULL, then zeroes the whole block. particles is the block pointer passed as an int.
   Returns nothing. */

void particles_destroy(char *particles)

{
  RwFrame *frame;

  /* detach the atomic from its frame, then destroy frame and atomic */
  frame = RpAtomic_object_object_parent((*(RpAtomic **)(particles + PARTICLES_ATOMIC_OFFSET)));
  RpAtomicSetFrame(*(RpAtomic **)(particles + PARTICLES_ATOMIC_OFFSET),(RwFrame *)0x0);
  RwFrameDestroy(frame);
  RpAtomicDestroy(*(RpAtomic **)(particles + PARTICLES_ATOMIC_OFFSET));

  /* free the owned buffers, skipping any that are NULL (+0x10 .. +0x28, step 4) */
  if (*(void **)(particles + 0x10) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x10));
  }
  if (*(void **)(particles + 0x14) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x14));
  }
  if (*(void **)(particles + 0x18) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x18));
  }
  if (*(void **)(particles + 0x1c) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x1c));
  }
  if (*(void **)(particles + 0x20) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x20));
  }
  if (*(void **)(particles + 0x24) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x24));
  }
  if (*(void **)(particles + 0x28) != (void *)0x0) {
    RwFree(*(void **)(particles + 0x28));
  }
  mem_zero(particles,PARTICLES_SIZE);
  return;
}


