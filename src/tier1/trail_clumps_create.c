#include "mfl_tier2.h"
extern struct TrailSlot g_trails[6];
/*0048e390*/ extern int rwp_object_set_radius_squared();
/*00491530*/ extern int __cdecl rwp_object_create_from_atomic(struct RpAtomic *p0, void *p1, float p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*005de16c*/ extern undefined g_trail_clumps_5de16c;
/*00653148*/ extern undefined4 g_trail_653148;
#define TRAIL_CLUMP_ID_BASE (0xcafe)
#define TRAIL_CLUMP_ID_LIMIT (0xcb0a) /* BASE + 6 slots * 2 ids */
#define TRAIL_SLOT_WORDS (0xb) /* sizeof(struct TrailSlot) / 4 */
#define TRAIL_RADIUS_SQUARED_BITS (0x3c23d70a) /* float 0.01 */
/* trail_clumps_create */
/* "clones the master clump for ids 0xCAFE.. into the 0x653040 table" */
/*
 * Creates the trail clumps: for each of the 6 g_trails slots, clones the
 * master clump (g_trail_653148) twice, once per id (clump_id, clump_id + 1),
 * stores the results in the slot's object_a and object_b, and sets each
 * clump's radius squared to 0.01.
 * Returns nothing.
 */
void trail_clumps_create(void)
{
  uint created_object;
  uint *slot_object_b;
  int clump_id;

  slot_object_b = &g_trails[0].object_b;
  clump_id = TRAIL_CLUMP_ID_BASE;
  /* one pass per trail slot: two clumps, object_a then object_b */
  do {
    created_object = rwp_object_create_from_atomic((RpAtomic *)clump_first_atomic(g_trail_653148),&g_trail_clumps_5de16c,0.0f,0x3dcccccd,0x3f7ff972,0x40400000,clump_id);
    slot_object_b[-1] = created_object;
    created_object = rwp_object_create_from_atomic((RpAtomic *)clump_first_atomic(g_trail_653148),&g_trail_clumps_5de16c,0.0f,0x3dcccccd,0x3f7ff972,0x40400000,clump_id + 1);
    *slot_object_b = created_object;
    rwp_object_set_radius_squared(slot_object_b[-1],TRAIL_RADIUS_SQUARED_BITS);
    rwp_object_set_radius_squared(*slot_object_b,TRAIL_RADIUS_SQUARED_BITS);
    clump_id = clump_id + 2;
    slot_object_b = slot_object_b + TRAIL_SLOT_WORDS;
  } while (clump_id < TRAIL_CLUMP_ID_LIMIT);
  return;
}
