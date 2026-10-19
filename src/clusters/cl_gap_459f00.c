// cluster-entries: gap_459f00
/* Installed cluster gap_459f00 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_459f00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00459ec0*/ extern int trigger_cb_mark_hit_bit();
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*005dd330*/ extern float float_1_0;
/* gap_459f00 */

void gap_459f00_n(int in_eax)

{
  struct {
    undefined4 x;
    undefined4 y;
    undefined4 z;
    float radius;
    undefined4 kind;
    int owner;
    undefined4 reserved;
    code *callback;
  } sphere;

  sphere.x = *(undefined4 *)(in_eax + 0x80);
  sphere.y = *(undefined4 *)(in_eax + 0x84);
  sphere.z = *(undefined4 *)(in_eax + 0x88);
  sphere.radius = *(float *)(in_eax + 0x8c) + float_1_0;
  sphere.kind = 0x15;
  sphere.callback = trigger_cb_mark_hit_bit;
  sphere.reserved = 0;
  sphere.owner = in_eax;
  trigger_sphere_register(&sphere.x);
  return;
}

/* entry thunk at the original address of gap_459f00: register inputs (eax) -> natural C gap_459f00_n; ret 0 */
__declspec(naked) void gap_459f00(void)
{
    __asm {
        push eax
        call gap_459f00_n
        add esp, 4
        ret
    }
}
