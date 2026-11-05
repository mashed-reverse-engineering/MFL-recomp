/* rwp_atomic_set_object (0x0058c8b0): attach an Rwp object to an atomic (atomic plugin slot) */
#include "mfl_tier2.h"

/*007daa18*/ extern int g_data_7daa18;   /* offset of the Rwp object slot in RpAtomic (rwp_atomic_plugin_attach) */
/*0056e5b0*/ extern void rwp_obj_0056e5b0(void *obj, int flags);

typedef struct MflRwpState {
  void *shape;   /* 0x0 */
  void *sim;   /* 0x4 */
  void *matrix;   /* 0x8: the matrix the object follows */
} MflRwpState;

typedef struct MflRwpObject {
  unsigned char pad00[0x10];
  MflRwpState *state;   /* 0x10 */
  unsigned char pad14[0x1c];
  void *owner;   /* 0x30: the atomic the object is attached to */
} MflRwpObject;

/* Detaches the object that sits in the atomic's plugin slot (its matrix and owner are cleared), then attaches obj (may be 0): obj follows the
   modelling matrix of the atomic's frame and gets the atomic as owner.  Returns the atomic. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwp_atomic_set_object(void *atomic, MflRwpObject *obj)
{
  MflRwpObject **slot = (MflRwpObject **)((char *)atomic + g_data_7daa18);
  MflRwpObject *old = *slot;

  if (old) {
    old->state->matrix = 0;
    old->owner = 0;
  }
  if (obj) {
    obj->state->matrix = (char *)RpAtomic_object_object_parent(atomic) + 0x10;   /* &frame->modelling */
    rwp_obj_0056e5b0(obj, 0);
    obj->owner = atomic;
  }
  *slot = obj;
  return atomic;
}
