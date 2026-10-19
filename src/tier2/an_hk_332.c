#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0076be30*/ extern undefined g_particles_76be30;
/*0076be70*/ extern undefined g_particles_76be70;
/*0076bfc0*/ extern undefined4 g_particles_76bfc0;
/*0076c4e0*/ extern char g_particles_76c4e0[];
#define FX_SYSTEM_STRIDE (0x29)
#define FX_SYSTEM_ATOMIC_INDEX (0xb)
#define FX_SYSTEM_ATOMIC_COUNT (10)
#define FX_SYSTEM_ROOT_FRAME_INDEX (10)
/* fx_systems_destroy_all */

/* "particles_destroy of the systems at 0x76BE30 / 0x76BE70 / 0x76BFC0" */
/*
 * Destroys all the FX particle systems and their render atomics/frames.
 * Calls particles_destroy on the two standalone systems (g_particles_76be30, with destroy_arg,
 * and g_particles_76be70), then walks the array of FX system blocks from g_particles_76bfc0 up to
 * g_particles_76c4e0. Each block is FX_SYSTEM_STRIDE undefined4 slots long and holds
 * FX_SYSTEM_ATOMIC_COUNT atomic pointers starting at FX_SYSTEM_ATOMIC_INDEX, plus a root frame at
 * FX_SYSTEM_ROOT_FRAME_INDEX.
 * destroy_arg: passed straight through to particles_destroy (inferred: destroy flags or context).
 * Returns nothing.
 */
void __fastcall fx_systems_destroy_all(undefined4 destroy_arg)

{
  RpAtomic *atomic;
  RwFrame *atomic_frame;
  undefined4 *atomic_slot;
  int atomics_left;
  undefined4 *fx_system;

  particles_destroy(&g_particles_76be30,destroy_arg);
  particles_destroy(&g_particles_76be70);
  fx_system = &g_particles_76bfc0;
  do {
    /* per system block: the atomic slots start at FX_SYSTEM_ATOMIC_INDEX */
    atomic_slot = fx_system + FX_SYSTEM_ATOMIC_INDEX;
    atomics_left = FX_SYSTEM_ATOMIC_COUNT;
    /* for each atomic: detach it from its frame, free the frame, then free the atomic */
    do {
      atomic = (RpAtomic *)*atomic_slot;
      atomic_frame = RpAtomic_object_object_parent(atomic);
      RpAtomicSetFrame(atomic,(RwFrame *)0x0);
      RwFrameRemoveChild(atomic_frame);
      RwFrameDestroy(atomic_frame);
      RpAtomicDestroy(atomic);
      atomic_slot = atomic_slot + 1;
      atomics_left = atomics_left + -1;
    } while (atomics_left != 0);
    RwFrameDestroy((RwFrame *)fx_system[FX_SYSTEM_ROOT_FRAME_INDEX]);
    fx_system = fx_system + FX_SYSTEM_STRIDE;
  } while ((LONG_PTR)fx_system < ((LONG_PTR)&g_particles_76c4e0));
  return;
}


