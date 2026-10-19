#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0058ca00*/ extern void FUN_0058ca00(undefined4 p0);
static int rwp_world_current_deactivate_r(int reg_eax);
extern void rwp_world_current_deactivate(void);
#define VEHICLE_SLOTS_OFFSET (0x10)
#define VEHICLE_CLUMP_OFFSET (0x200)
#define VEHICLE_SIZE (0x208)
#define SLOT_GROUP_COUNT (2)
#define SLOTS_PER_GROUP (10)
__declspec(naked) int rwp_world_current_deactivate_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rwp_world_current_deactivate
        ret
    }
}

/* debris_vehicle_destroy_all */

/* "destroys every debris slot's body and atomic of one vehicle record" */

/*
 * Tears down all debris slots of one vehicle record (layout inferred).
 * vehicle: pointer to the vehicle record, typed int because it is used for byte offsets.
 * Slots start at vehicle + VEHICLE_SLOTS_OFFSET: SLOT_GROUP_COUNT groups of SLOTS_PER_GROUP slots.
 * Each slot is 6 ints: slot[0] is its object, slot[-1] is its atomic (inferred).
 * Destroys the clump at VEHICLE_CLUMP_OFFSET, then zeroes the whole VEHICLE_SIZE-byte record.
 * Returns nothing.
 */
void debris_vehicle_destroy_all(char *vehicle)

{
  RpAtomic *atomic;
  RwFrame *frame;
  int slots_left;
  int *slot_group;
  int *slot;
  int groups_left;

  slot_group = (int *)(vehicle + VEHICLE_SLOTS_OFFSET);
  groups_left = SLOT_GROUP_COUNT;
  /* each slot group, 0x40 ints apart */
  do {
    slots_left = SLOTS_PER_GROUP;
    slot = slot_group;
    /* each slot in the group: release its object, then detach and destroy its atomic and frame */
    do {
      if (*slot != 0) {
        /* object flag at +0x24 set (inferred): deactivate it before releasing */
        if (*(int *)(*slot + 0x24) != 0) {
          rwp_world_current_deactivate_r(*slot);
        }
        /* release the object (FUN_0058ca00, inferred) */
        FUN_0058ca00(*slot);
      }
      atomic = (RpAtomic *)slot[-1];
      if (atomic != (RpAtomic *)0x0) {
        /* the atomic's frame is its object parent; detach the atomic, then free the frame */
        frame = RpAtomic_object_object_parent(atomic);
        RpAtomicSetFrame(atomic,(RwFrame *)0x0);
        RwFrameDestroy(frame);
      }
      slot = slot + 6;
      slots_left = slots_left + -1;
    } while (slots_left != 0);
    slot_group = slot_group + 0x40;
    groups_left = groups_left + -1;
  } while (groups_left != 0);
  RpClumpDestroy(*(RpClump **)(vehicle + VEHICLE_CLUMP_OFFSET));
  mem_zero(vehicle,VEHICLE_SIZE);
  return;
}


