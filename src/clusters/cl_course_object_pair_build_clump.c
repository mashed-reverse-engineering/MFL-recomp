// cluster-entries: course_object_pair_build_clump
/* Installed cluster course_object_pair_build_clump (tools/cluster_install.py from output/cluster_pilot_uc3/course_object_pair_build_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5cd0*/ extern int object_frames_create_forward(int atomic, int count, int arg2, int arg3);
/*004a5df0*/ extern int object_atomic_create(int frames, int index, int param_4);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern RpWorld * clump_get_world(RpClump * p0);
/*006c2658*/ extern int g_car_6c2658;
/* course_object_pair_build_clump */

/* WARNING: Unknown calling convention */
/* "removes the old clump and builds frames/atomics for entry param_2" */

static int course_object_atomic_register_r(int p0, char * p1, int p2, int p3);
extern void course_object_atomic_register(void);
void course_object_pair_build_clump_n(char *in_esi, int in_ebx, int param_3, int param_4)

{
  RpWorld *world;
  int atomic;
  int iVar2;
  char *slot;
  
  iVar2 = 0;
  slot = in_esi + in_ebx * 0xc00;
  world = clump_get_world(*(RpClump **)(in_esi + 0xc110 + param_3 * 4));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(in_esi + 0xc110 + param_3 * 4));
  }
  (&g_car_6c2658)[param_4] =
       object_frames_create_forward((int)clump_first_atomic(*(RpClump **)(in_esi + 0xc110 + param_3 * 4)),
                                    *(int *)(in_esi + 0xc000 + in_ebx * 4),*(int *)(in_esi + 0xc9e0),
                                    *(int *)(in_esi + 0xc9e4));
  if (0 < *(int *)(in_esi + 0xc000 + in_ebx * 4)) {
    do {
      atomic = object_atomic_create((&g_car_6c2658)[param_4],iVar2,param_4);
      course_object_atomic_register_r(atomic,slot,*(int *)(in_esi + 0xc090),param_4);
      iVar2 = iVar2 + 1;
      slot = slot + 0x40;
    } while (iVar2 < *(int *)(in_esi + 0xc000 + in_ebx * 4));
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
