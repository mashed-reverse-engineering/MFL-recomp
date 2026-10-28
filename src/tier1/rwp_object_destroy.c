#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a4b0*/ extern void rwp_world_deactivate_object(undefined4 p0, void *p1);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*0058ca00*/ extern void FUN_0058ca00(void *p0);
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa858*/ extern undefined4 g_rw_6fa858;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*00701c9c*/ extern undefined4 g_rw_701c9c;
/* rwp_object_destroy */
/* "destroys the physics object id (<0xC9): removes body, atomic, frame" */
/* Destroys the physics object in slot object_id (0 <= object_id < PHYSICS_OBJECT_COUNT).
 * A slot holds an RpAtomic in g_rw_6fab78[] and an owner object from rwp_owner_object_get().
 * Releases the owner (body), unlinks the atomic from its world, destroys its frame and the
 * atomic itself, decrements the live count g_rw_6fa7d0, then clears the slot in g_rw_6fab78[]
 * and g_rw_6fa4b0[] and marks g_rw_6fa858[] = 1 (inferred: free). Returns nothing.
 * active_owner is rw_first_field(g_rw_701c9c) (inferred: the currently selected owner). */
void __cdecl rwp_object_destroy(int object_id)
{
  RwFrame *frame;
  int active_owner;
  char *owner;
  RpWorld *world;
  
  active_owner = rw_first_field(g_rw_701c9c);
  if ((object_id >= 0) && (object_id <= 200)) {
    if ((&g_rw_6fab78)[object_id] != 0) {
      /* Release the owner body; if it is the active owner, drop it from active_owner first. */
      owner = rwp_owner_object_get((&g_rw_6fab78)[object_id]);
      if (owner != 0) {
        if ((active_owner != 0) && (*(int *)(owner + 0x24) == active_owner)) {
          rwp_world_deactivate_object(active_owner,owner);
        }
        FUN_0058ca00(owner);
      }
      /* Unlink the atomic from its world, if it is in one. */
      world = RpAtomicGetWorld((RpAtomic *)(&g_rw_6fab78)[object_id]);
      if (world != (RpWorld *)0x0) {
        RpWorldRemoveAtomic(world,(RpAtomic *)(&g_rw_6fab78)[object_id]);
      }
      /* Detach the atomic from its frame, then destroy the frame and the atomic. */
      frame = RpAtomic_object_object_parent(((RpAtomic *)(&g_rw_6fab78)[object_id]));
      if (frame != (RwFrame *)0x0) {
        RpAtomicSetFrame((RpAtomic *)(&g_rw_6fab78)[object_id],(RwFrame *)0x0);
        RwFrameDestroy(frame);
      }
      RpAtomicDestroy((RpAtomic *)(&g_rw_6fab78)[object_id]);
      g_rw_6fa7d0 = g_rw_6fa7d0 + -1;
    }
    /* Clear the slot and mark it free (inferred: g_rw_6fa858 = 1 means free). */
    (&g_rw_6fab78)[object_id] = 0;
    (&g_rw_6fa4b0)[object_id] = 0;
    (&g_rw_6fa858)[object_id] = 1;
  }
  return;
}
