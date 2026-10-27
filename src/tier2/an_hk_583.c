#include "mfl_tier2.h"
/*006af5e4*/ extern undefined4 g_car_6af5e4[0x1000];
/*006c2684*/ extern undefined4 g_course_object_pair_count;
static int course_object_pair_build_clump_r(int reg_esi, int reg_ebx, int pair_obj_b, int pair_index);
extern void course_object_pair_build_clump(void);
/* dwords per object pair record in g_car_6af5e4 (inferred from the loop stride) */
#define OBJECT_PAIR_STRIDE (3)
__declspec(naked) int course_object_pair_build_clump_r(int reg_esi, int reg_ebx, int pair_obj_b, int pair_index)
{
    __asm {
        push ebx
        push esi
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        mov esi, dword ptr [esp + 20]
        mov ebx, dword ptr [esp + 24]
        call course_object_pair_build_clump
        add esp, 8
        pop esi
        pop ebx
        ret
    }
}

/* course_object_pairs_build_clumps */

/* "builds a clump for every object pair entry"
 * Calls course_object_pair_build_clump_r once per object pair record in g_car_6af5e4
 * (g_course_object_pair_count records, OBJECT_PAIR_STRIDE dwords each).
 * esi_arg is forwarded unchanged as the callee's esi (inferred).
 * Returns nothing; params 2..16 are unused. */

void __cdecl course_object_pairs_build_clumps(undefined4 esi_arg,char *unused_2,char *unused_3,char *unused_4,
          char *unused_5,char *unused_6,char *unused_7,char *unused_8,
          char *unused_9,char *unused_10,char *unused_11,char *unused_12,
          char *unused_13,char *unused_14,char *unused_15,char *unused_16)

{
  undefined4 *pair_ptr;
  int pair_index;

  pair_index = 0;
  /* walk every pair record; pair_ptr[-1] and *pair_ptr are the two object values passed on */
  if (0 < (int)g_course_object_pair_count) {
    pair_ptr = g_car_6af5e4;
    do {
      course_object_pair_build_clump_r(esi_arg,*pair_ptr,pair_ptr[-1],pair_index);
      pair_index = pair_index + 1;
      pair_ptr = pair_ptr + OBJECT_PAIR_STRIDE;
    } while (pair_index < (int)g_course_object_pair_count);
  }
  return;
}


