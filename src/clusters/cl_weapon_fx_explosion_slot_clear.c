// cluster-entries: weapon_fx_explosion_slot_clear
/* Installed cluster weapon_fx_explosion_slot_clear (tools/cluster_install.py from output/cluster_pilot10/weapon_fx_explosion_slot_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/* weapon_fx_explosion_slot_clear */

/* "clears one explosion fx slot and removes its frame / atomic from the world" */

void weapon_fx_explosion_slot_clear_n(undefined4 *in_esi)

{
  RpWorld *world;
  
  in_esi[0xb] = 0;
  in_esi[10] = 0;
  in_esi[0xf] = 0;
  *in_esi = 0;
  in_esi[1] = 0;
  in_esi[2] = 0;
  in_esi[3] = 0;
  in_esi[0x10] = 0;
  RwFrameSetIdentity((RwFrame *)in_esi[0xd]);
  RwFrameSetIdentity((RwFrame *)in_esi[0xe]);
  if (RwFrame_object_parent(((RwFrame *)in_esi[0xd])) != (void *)0x0) {
    RwFrameRemoveChild((RwFrame *)in_esi[0xd]);
  }
  world = RpAtomicGetWorld((RpAtomic *)in_esi[0xc]);
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveAtomic(world,(RpAtomic *)in_esi[0xc]);
  }
  return;
}

/* entry thunk at the original address of weapon_fx_explosion_slot_clear: register inputs (esi) -> natural C weapon_fx_explosion_slot_clear_n; ret 0 */
__declspec(naked) void weapon_fx_explosion_slot_clear(void)
{
    __asm {
        push esi
        call weapon_fx_explosion_slot_clear_n
        add esp, 4
        ret
    }
}
