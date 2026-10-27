#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00658300*/ extern undefined4 g_load_658300;
/*00658490*/ extern undefined4 g_particles_658490;
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538;
/*00658b28*/ extern undefined4 g_load_658b28;
/*00658b2c*/ extern undefined4 g_load_658b2c;
/* marker_particles_destroy */

/* "particles_destroy(0x6586E8)" */

extern void marker_particles_destroy(void);
#define CLUMP_SLOT_STRIDE (0x15) /* in undefined4 units, 0x54 bytes per clump slot */
#define CLUMP_TABLE_END (((LONG_PTR)&g_hudpanel_658538)) /* first address after the clump slot table */
#define ATOMIC_SLOT_STRIDE (0x19) /* in undefined4 units, 0x64 bytes per atomic slot */
#define ATOMIC_TABLE_END (((LONG_PTR)&g_particles_658490)) /* first address after the atomic slot table */
/* hudpanel_clumps_destroy_b */
/* "second variant of hudpanel_clumps_destroy" */
/*
 * Destroys the clumps and atomics this HUD panel owns (inferred from the callees):
 * the clump slots at 0x658490, the atomic slots at 0x658300 (each atomic's frame is
 * destroyed before the atomic), the load clump g_load_658b28, the texture dictionary
 * g_load_658b2c, the particle system and the marker particles.
 * Takes no parameters and returns nothing.
 */
void hudpanel_clumps_destroy_b(void)
{
  struct { undefined4 * clump_slot; int * atomic_slot; } cursors;
#define clump_slot cursors.clump_slot
#define atomic_slot cursors.atomic_slot

  /* Walk the clump slot table and destroy each slot's clump. */
  clump_slot = &g_particles_658490;
  do {
    RpClumpDestroy((RpClump *)*clump_slot);
    clump_slot = clump_slot + CLUMP_SLOT_STRIDE;
  } while ((LONG_PTR)clump_slot < CLUMP_TABLE_END);
  /* Walk the atomic slot table: destroy each atomic's frame (+4 is its parent), then the atomic. */
  atomic_slot = &g_load_658300;
  do {
    RwFrameDestroy(*(RwFrame **)(*atomic_slot + 4));
    RpAtomicDestroy((RpAtomic *)*atomic_slot);
    atomic_slot = atomic_slot + ATOMIC_SLOT_STRIDE;
  } while ((LONG_PTR)atomic_slot < ATOMIC_TABLE_END);
  RpClumpDestroy(g_load_658b28);
  RwTexDictionaryDestroy_004d5ac0(g_load_658b2c);
  particles_destroy(&g_hudpanel_6584f8);
  marker_particles_destroy();
  return;
}
#undef clump_slot
#undef atomic_slot
