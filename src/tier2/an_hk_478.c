#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c6600*/ extern void FUN_004c6600(void *p0, undefined4 p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00555020*/ extern int RpMorphGeometryCreateInterpolators();
/*005550a0*/ extern int RpMorphGeometrySetInterpolator();
/*00555100*/ extern int RpMorphAtomicSetCurrentInterpolator();
/*00555220*/ extern int RpMorphAtomicSetTime();
/*005e097c*/ extern undefined g_load_waterfall_5e097c;
/*0076be30*/ extern undefined g_particles_76be30;
/*0076be70*/ extern undefined g_particles_76be70;
/*0076beb0*/ extern undefined1 g_particles_76beb0;
/*0076bec0*/ extern undefined4 g_particles_76bec0;
/*0076bfe8*/ extern undefined4 g_load_76bfe8;
/*0076dee0*/ extern undefined4 g_particles_76dee0;
/*0076c508*/ extern char g_particles_76c508[];

#define SPLASH_ATOMICS_PER_SLOT (10)
#define SLOT_STRIDE_DWORDS (0x29)
#define FLOAT_ONE_BITS (0x3f800000)

/* Sets up the waterfall and shockwave particle effects. Takes no parameters, returns nothing.
   Creates the "wfall" and "shockwave" particle systems, loads the Splash.dff clump and gives its
   first atomic morph interpolators, then builds the splash slot table at g_load_76bfe8: each slot
   gets a frame plus SPLASH_ATOMICS_PER_SLOT atomic clones, each clone with its own frame as a child
   of the slot frame. g_particles_76dee0 and the slot table are cleared on the way.
   The Splash clump is destroyed at the end. (inferred: table is read by the waterfall render code) */

/* "wfall shockwave" */

void load_waterfall_fx(void)

{
  RpGeometry *splash_geometry;
  undefined4 *slot_cursor;
  RwTexture *texture_dict;
  RpClump *splash_clump;
  RpAtomic *splash_atomic;
  RwFrame *frame;
  RpAtomic *atomic_clone;
  int remaining;
  undefined4 *slot;
  undefined4 *atomic_slot;
  undefined *waterfall_state;
  undefined4 clump_load_arg1;
  undefined4 clump_load_arg2;
  undefined4 *particle_slot;

  /* particle systems for the "wfall" and "shockwave" textures (inferred: args 0x817/0x80 and 0x80c/0x40 are type and count) */
  texture_dict = (RwTexture *)find_texture_dict_b("wfall");
  particle_system_setup_ptank((uint *)&g_particles_76be70,0x817,0x80,texture_dict);
  texture_dict = (RwTexture *)find_texture_dict_b("shockwave");
  particle_system_setup_ptank((uint *)&g_particles_76be30,0x80c,0x40,texture_dict);
  FUN_004c6600(&g_particles_76beb0,0x80);
  clump_load_arg2 = 0;
  clump_load_arg1 = 0;
  waterfall_state = &g_load_waterfall_5e097c;
  /* clear 0x40 dwords starting at g_particles_76bec0 (inferred: per-particle-system state) */
  slot = &g_particles_76bec0;
  for (remaining = 0x40; remaining != 0; remaining = remaining + -1) {
    *slot = 0;
    slot = slot + 1;
  }
  splash_clump = (RpClump *)clump_load_by_name("Splash.dff",clump_load_arg1,clump_load_arg2);
  splash_atomic = (RpAtomic *)clump_first_atomic(splash_clump);
  splash_geometry = RpAtomic_geometry(splash_atomic);
  /* morph setup on the splash geometry: one interpolator, full weight, time reset to 0 (inferred) */
  if (splash_geometry != (RpGeometry *)0x0) {
    RpMorphGeometryCreateInterpolators(splash_geometry,1);
    RpMorphGeometrySetInterpolator(splash_geometry,0,0,1,FLOAT_ONE_BITS);
    RpMorphAtomicSetCurrentInterpolator(splash_atomic,0);
    RpMorphAtomicSetTime(splash_atomic,0);
  }
  /* one outer iteration per slot: slot frame, then SPLASH_ATOMICS_PER_SLOT atomic clones under it */
  particle_slot = &g_particles_76dee0;
  slot = &g_load_76bfe8;
  do {
    *particle_slot = 0;
    frame = RwFrameCreate();
    slot[-7] = 0;
    slot[-6] = 0;
    *slot = frame;
    remaining = SPLASH_ATOMICS_PER_SLOT;
    slot_cursor = slot;
    /* inner loop: clone the splash atomic, give it a frame, and attach it to the slot frame */
    do {
      atomic_slot = slot_cursor + 1;
      frame = RwFrameCreate();
      atomic_clone = RpAtomicClone(splash_atomic);
      *atomic_slot = atomic_clone;
      RpAtomicSetFrame(atomic_clone,frame);
      RwFrameAddChild((RwFrame *)*slot,frame);
      FUN_004c5410(*atomic_slot,0x40,1);
      RpMorphAtomicSetCurrentInterpolator(*atomic_slot,0);
      RpMorphAtomicSetTime(*atomic_slot,0);
      slot_cursor[0x15] = 0;
      remaining = remaining + -1;
      slot_cursor = atomic_slot;
    } while (remaining != 0);
    slot = slot + SLOT_STRIDE_DWORDS;
    particle_slot = particle_slot + 1;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_particles_76c508));
  RpClumpDestroy(splash_clump);
  return;
}


