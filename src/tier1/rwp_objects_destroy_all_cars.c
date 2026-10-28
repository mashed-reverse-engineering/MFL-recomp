#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0056a4b0*/ extern void rwp_world_deactivate_object(undefined4 p0, void *p1);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*0058ca00*/ extern void FUN_0058ca00(void *p0);
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*006fd4a8*/ extern char g_rw_6fd4a8[];
/* rwp_objects_destroy_all_cars */
/* "destroys every car body (g_rw_6fd498) atomic, frame and world link" */
/* Destroys every car body atomic in the g_rw_6fd498 table (no parameters, no return value).
   Each non-null slot is an RpAtomic *: its owner object (from rwp_owner_object_get) is notified
   and released, its frame is detached and destroyed, it is removed from its world and destroyed,
   and the slot is cleared. The table ends at g_rw_6fd4a8. */
void rwp_objects_destroy_all_cars(void)
{
  RwFrame *frame;
  char *owner;
  RpWorld *world;
  int *atomic_slot;

  /* walk the atomic table from g_rw_6fd498 up to g_rw_6fd4a8 */
  atomic_slot = &g_rw_6fd498;
  do {
    if (*atomic_slot != 0) {
      /* owner object of this atomic (inferred: the car that owns the body) */
      owner = rwp_owner_object_get(*atomic_slot);
      if (owner != 0) {
        /* (inferred) notify the owner's pointer at +0x24, then release the owner */
        if (*(int *)(owner + 0x24) != 0) {
          rwp_world_deactivate_object(*(int *)(owner + 0x24),owner);
        }
        FUN_0058ca00(owner);
      }
      /* detach the atomic from its frame and destroy the frame */
      frame = RpAtomic_object_object_parent(((RpAtomic *)*atomic_slot));
      if (frame != (RwFrame *)0x0) {
        RpAtomicSetFrame((RpAtomic *)*atomic_slot,(RwFrame *)0x0);
        RwFrameDestroy(frame);
      }
      /* remove the atomic from its world before destroying it */
      world = RpAtomicGetWorld((RpAtomic *)*atomic_slot);
      if (world != (RpWorld *)0x0) {
        RpWorldRemoveAtomic(world,(RpAtomic *)*atomic_slot);
      }
      RpAtomicDestroy((RpAtomic *)*atomic_slot);
      *atomic_slot = 0;
    }
    atomic_slot = atomic_slot + 1;
  } while ((LONG_PTR)atomic_slot < ((LONG_PTR)&g_rw_6fd4a8));
  return;
}
