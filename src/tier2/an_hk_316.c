#include "mfl_tier2.h"
/*0048e8d0*/ extern int rwp_objects_destroy_all_cars();
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a4b0*/ extern void FUN_0056a4b0(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*0058ca00*/ extern void FUN_0058ca00(undefined4 p0);
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fa858*/ extern undefined4 g_rw_6fa858;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*00701c9c*/ extern undefined4 g_rw_701c9c;

/* 200 slots of 4 bytes each (one pointer per slot in each g_rw_* table) */
#define PHYSICS_OBJECT_TABLE_BYTES (800)

/* rwp_objects_destroy_all */

/* "destroys all 200 physics objects and the cars"
 *
 * Tears down the physics object table: destroys the cars first, then walks all
 * slots of g_rw_6fab78 (one RpAtomic pointer per slot). Each occupied slot has its
 * owner object released and its atomic removed from its world, detached from its
 * frame and geometry, and destroyed. Finally every slot is cleared: g_rw_6fab78 = 0,
 * g_rw_6fa4b0 = 0 and g_rw_6fa858 = 1 (inferred: reset flag, "free" state).
 *
 * Takes no parameters, returns nothing.
 */

void rwp_objects_destroy_all(void)

{
  int first_field;
  int owner_obj;
  RpWorld *world;
  int slot_offset;

  /* first_field: object returned by rw_first_field(g_rw_701c9c) (inferred) */
  first_field = rw_first_field(g_rw_701c9c);
  rwp_objects_destroy_all_cars();
  /* slot_offset: byte offset into the tables, 4 bytes per slot */
  slot_offset = 0;
  do {
    /* Occupied slot: release the owner object, then tear down the atomic. */
    if (*(int *)((char *)&g_rw_6fab78 + slot_offset) != 0) {
      owner_obj = rwp_owner_object_get(*(int *)((char *)&g_rw_6fab78 + slot_offset));
      if (owner_obj != 0) {
        /* If the owner belongs to first_field (+0x24 match, inferred), notify it first. */
        if ((first_field != 0) && (*(int *)(owner_obj + 0x24) == first_field)) {
          FUN_0056a4b0(first_field,owner_obj);
        }
        FUN_0058ca00(owner_obj);
      }
      world = RpAtomicGetWorld(*(RpAtomic **)((char *)&g_rw_6fab78 + slot_offset));
      if (world != (RpWorld *)0x0) {
        RpWorldRemoveAtomic(world,*(RpAtomic **)((char *)&g_rw_6fab78 + slot_offset));
      }
      RpAtomicSetFrame(*(RpAtomic **)((char *)&g_rw_6fab78 + slot_offset),(RwFrame *)0x0);
      RpAtomicSetGeometry(*(RpAtomic **)((char *)&g_rw_6fab78 + slot_offset),(RpGeometry *)0x0,0);
      RpAtomicDestroy(*(RpAtomic **)((char *)&g_rw_6fab78 + slot_offset));
    }
    *(undefined4 *)((char *)&g_rw_6fab78 + slot_offset) = 0;
    *(undefined4 *)((char *)&g_rw_6fa4b0 + slot_offset) = 0;
    *(undefined4 *)((char *)&g_rw_6fa858 + slot_offset) = 1;
    slot_offset = slot_offset + 4;
  } while (slot_offset < PHYSICS_OBJECT_TABLE_BYTES);
  return;
}


