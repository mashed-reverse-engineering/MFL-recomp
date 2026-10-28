// cluster-entries: course_object_pair_build_clump
/* Installed cluster course_object_pair_build_clump (tools/cluster_install.py from output/cluster_pilot_uc3/course_object_pair_build_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5cd0*/ extern int object_frames_create_forward(int atomic, int count, int arg2, int arg3);
/*004a5df0*/ extern int object_atomic_create(int frames, int index, int object_index);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*006c2658*/ extern int g_car_6c2658;
#define CLUMP_TABLE_OFFSET (0xc110) /* clump pointers, indexed by clump_index (inferred) */
#define ATOMIC_COUNT_TABLE_OFFSET (0xc000) /* atomic count per set (inferred) */
#define ATOMIC_SET_BYTES (0xc00) /* bytes of atomic slots per set */
#define ATOMIC_SLOT_BYTES (0x40) /* bytes per atomic slot */
/* course_object_pair_build_clump */

/* Removes the old clump from its world, creates the frames for its first atomic and registers one atomic per slot of set_index (inferred) */

static int course_object_atomic_register_r(int p0, char * p1, int p2, int p3);
extern void course_object_atomic_register(void);
void course_object_pair_build_clump_n(char *car_data, int set_index, int clump_index, int object_index)

{
  RpWorld *world;
  int atomic;
  int atomic_index;
  char *slot;
  
  atomic_index = 0;
  slot = car_data + set_index * ATOMIC_SET_BYTES;
  world = clump_get_world(*(RpClump **)(car_data + CLUMP_TABLE_OFFSET + clump_index * 4));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(car_data + CLUMP_TABLE_OFFSET + clump_index * 4));
  }
  (&g_car_6c2658)[object_index] =
       object_frames_create_forward((int)clump_first_atomic(*(RpClump **)(car_data + CLUMP_TABLE_OFFSET + clump_index * 4)),
                                    *(int *)(car_data + ATOMIC_COUNT_TABLE_OFFSET + set_index * 4),*(int *)(car_data + 0xc9e0),
                                    *(int *)(car_data + 0xc9e4));
  if (0 < *(int *)(car_data + ATOMIC_COUNT_TABLE_OFFSET + set_index * 4)) {
    do {
      atomic = object_atomic_create((&g_car_6c2658)[object_index],atomic_index,object_index);
      course_object_atomic_register_r(atomic,slot,*(int *)(car_data + 0xc090),object_index);
      atomic_index = atomic_index + 1;
      slot = slot + ATOMIC_SLOT_BYTES;
    } while (atomic_index < *(int *)(car_data + ATOMIC_COUNT_TABLE_OFFSET + set_index * 4));
  }
  return;
}

/* course_object_atomic_register_r: C -> original course_object_atomic_register (registers ecx,eax,s0,s1) */
__declspec(naked) int course_object_atomic_register_r(int p0, char * p1, int p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call course_object_atomic_register
        add esp, 8
        ret
    }
}

/* entry thunk at the original address of course_object_pair_build_clump: register inputs (esi,ebx,s0,s1) -> natural C course_object_pair_build_clump_n; ret 0 */
__declspec(naked) void course_object_pair_build_clump(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ebx
        push esi
        call course_object_pair_build_clump_n
        add esp, 16
        ret
    }
}
