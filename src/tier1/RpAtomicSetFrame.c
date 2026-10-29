/* compile: /G5 */
#include "mfl_tier2.h"
extern void *__cdecl rwObjectHasFrameSetFrame(void *obj, void *frame);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the frame of an atomic and marks the atomic dirty; returns the atomic. */
void *RpAtomicSetFrame(void *atomic, void *frame)
{
  rwObjectHasFrameSetFrame(atomic, frame);
  FLD(atomic, unsigned char, 3) |= 1;
  return atomic;
}
