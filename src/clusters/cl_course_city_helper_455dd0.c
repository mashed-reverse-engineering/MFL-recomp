// cluster-entries: course_city_helper_455dd0
/* Installed cluster course_city_helper_455dd0 (tools/cluster_install.py from output/cluster_pilot_k/course_city_helper_455dd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6c0*/ extern int rwp_object_set_flag();
/* course_city_helper_455dd0 */

/* WARNING: Unknown calling convention */
/* callers course_city_cb6,course_city_cb7 */

void course_city_helper_455dd0_n(undefined4 in_esi)

{
  undefined4 uVar1;
  
  uVar1 = rwp_object_find_index_by_id(3000);
  rwp_object_set_flag(uVar1,in_esi);
  uVar1 = rwp_object_find_index_by_id(0xbb9);
  rwp_object_set_flag(uVar1,in_esi);
  return;
}

/* entry thunk at the original address of course_city_helper_455dd0: register inputs (esi) -> natural C course_city_helper_455dd0_n; ret 0 */
__declspec(naked) void course_city_helper_455dd0(void)
{
    __asm {
        push esi
        call course_city_helper_455dd0_n
        add esp, 4
        ret
    }
}
