// cluster-entries: joypads_release_all
/* Installed cluster joypads_release_all (tools/cluster_install.py from output/cluster_pilot_zx/joypads_release_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0076ed20*/ extern undefined4 g_dinput_76ed20;
/*00770f64*/ extern undefined4 g_app_770f64;
/* size of one joypad slot in g_dinput_76ed20, in undefined4 units (inferred) */
#define JOYPAD_SLOT_STRIDE (0x112)

/* joypads_release_all */

/* "releases every enumerated joypad"
 * Walks the joypad slot array at g_dinput_76ed20, one slot per enumerated
 * device (count in g_app_770f64), and calls joypad_device_setup on each. */

static int joypad_device_setup_r(int p0);
extern void joypad_device_setup(void);
void joypads_release_all(void)

{
  undefined4 *joypad_slot;
  int joypad_index;

  joypad_index = 0;
  /* loop over every enumerated joypad slot; does nothing when the count is 0 */
  if (0 < (int)g_app_770f64) {
    joypad_slot = &g_dinput_76ed20;
    do {
      joypad_device_setup_r(joypad_slot);
      joypad_index = joypad_index + 1;
      joypad_slot = joypad_slot + JOYPAD_SLOT_STRIDE;
    } while (joypad_index < (int)g_app_770f64);
  }
  return;
}

/* joypad_device_setup_r: C -> original joypad_device_setup (registers esi) */
__declspec(naked) int joypad_device_setup_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call joypad_device_setup
        pop esi
        ret
    }
}
