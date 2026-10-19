// cluster-entries: sprite_slot_destroy_atomic_44
/* Installed cluster sprite_slot_destroy_atomic_44 (tools/cluster_install.py from output/cluster_pilot10/sprite_slot_destroy_atomic_44.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/* sprite_slot_destroy_atomic_44 */

/* WARNING: Unknown calling convention */
/* "destroys the frame and atomic stored at +0x44" */

void sprite_slot_destroy_atomic_44_n(int in_esi)

{
  RwFrameDestroy(*(RwFrame **)(*(char **)(in_esi + 0x44) + 4));
  RpAtomicSetFrame(*(RpAtomic **)(in_esi + 0x44),(RwFrame *)0x0);
  RpAtomicDestroy(*(RpAtomic **)(in_esi + 0x44));
  *(undefined4 *)(in_esi + 0x44) = 0;
  return;
}

/* entry thunk at the original address of sprite_slot_destroy_atomic_44: register inputs (esi) -> natural C sprite_slot_destroy_atomic_44_n; ret 0 */
__declspec(naked) void sprite_slot_destroy_atomic_44(void)
{
    __asm {
        push esi
        call sprite_slot_destroy_atomic_44_n
        add esp, 4
        ret
    }
}
