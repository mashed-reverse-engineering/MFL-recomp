// cluster-entries: course_object_id_register
/* Installed cluster course_object_id_register (tools/cluster_install.py from output/cluster_pilot10/course_object_id_register.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006c2688*/ extern undefined4 g_car_6c2688;
/*006c28c0*/ extern undefined4 g_car_6c28c0;
/* course_object_id_register */

/* "appends the id to the list at 0x6C2688 if not present" */

int course_object_id_register_n(int in_edx)

{
  int iVar1;
  int iVar2;
  
  iVar1 = g_car_6c28c0;
  iVar2 = 0;
  if (0 < (int)g_car_6c28c0) {
    do {
      if ((&g_car_6c2688)[iVar2] == in_edx) {
        return iVar2;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)g_car_6c28c0);
  }
  (&g_car_6c2688)[g_car_6c28c0] = in_edx;
  g_car_6c28c0 = g_car_6c28c0 + 1;
  return iVar1;
}

/* entry thunk at the original address of course_object_id_register: register inputs (edx) -> natural C course_object_id_register_n; ret 0 */
__declspec(naked) void course_object_id_register(void)
{
    __asm {
        push edx
        push edx
        call course_object_id_register_n
        add esp, 4
        pop edx
        ret
    }
}
