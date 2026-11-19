#include "mfl_tier2.h"
/*00481850*/ extern undefined __cdecl atomic_plugin_copy(void *p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004f68f0*/ extern struct RpAtomic * RpAtomicCreate();
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);


/* object_atomic_create */

/* "(object, index): creates an atomic on frame index sharing the geometry of the object" */

RpAtomic * object_atomic_create(char *object,int frame_index)

{
  RpAtomic *atomic;
  RpGeometry *geometry;
  RwFrame *frame;
  
  geometry = *(RpGeometry **)(*(char **)(object + 0x184) + 0x18);
  if (frame_index < *(int *)(object + 0x188)) {
    atomic = RpAtomicCreate();
    frame = *(RwFrame **)(object + frame_index * 4);
    RpAtomicSetFrame(atomic,frame);
    RpAtomicSetGeometry(atomic,geometry,0);
    atomic_plugin_copy(atomic,*(undefined4 *)(object + 0x184),0,0);
    return atomic;
  }
  return (RpAtomic *)0x0;
}
