#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void clump_collect_atomics_by_id(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00659b18*/ extern undefined g_game_659b18;
/*00659b19*/ extern undefined g_load_stopclock_659b19;
/*00659b1a*/ extern undefined g_load_stopclock_659b1a;
/*00659b1b*/ extern undefined g_load_stopclock_659b1b;
/*00659b1c*/ extern undefined g_load_stopclock_659b1c;
/*00659b1d*/ extern undefined g_load_stopclock_659b1d;
/*00659b1e*/ extern undefined g_load_stopclock_659b1e;
/*00659b1f*/ extern undefined g_load_stopclock_659b1f;
/*00659b20*/ extern undefined g_load_stopclock_659b20;
/*00659b21*/ extern undefined g_load_stopclock_659b21;
/*00659b22*/ extern undefined g_load_stopclock_659b22;
/*00659b23*/ extern undefined g_load_stopclock_659b23;
/*00659b24*/ extern undefined g_load_stopclock_659b24;
/*00659b25*/ extern undefined g_load_stopclock_659b25;
/*00659b26*/ extern undefined g_load_stopclock_659b26;
/*00659b27*/ extern undefined g_load_stopclock_659b27;
/*00659b28*/ extern undefined g_load_stopclock_659b28;
/*00659b29*/ extern undefined g_load_stopclock_659b29;
/*00659b2a*/ extern undefined g_load_stopclock_659b2a;
/*00659b2b*/ extern undefined g_load_stopclock_659b2b;
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659c38*/ extern undefined4 g_load_659c38;
/*00659c60*/ extern undefined4 g_load_stopclock_659c60;
/*00659ca8*/ extern undefined4 g_load_stopclock_659ca8;
/*00659cdc*/ extern undefined4 g_game_659cdc;
/*00659ce0*/ extern undefined4 g_load_659ce0;
/*00659c78*/ extern char g_load_stopclock_659c78[];
/*00659cb8*/ extern char g_game_659cb8[];

#define ATOMICS_PER_CLUMP (0xb)
#define SPECIAL_ATOMIC_INDEX (10)
#define STOPCLOCK_ZERO_BYTES (0x1d4)

/* load_stopclock
 * Loads stopclock2.dff (the stopclock model) and builds the clone set for it.
 * Clears the clone table g_load_659b30, loads the clump and parents its root frame to g_load_659ce0,
 * then fills the table with ATOMICS_PER_CLUMP atomic clones per source clump, each on its own child
 * frame placed at the position of the source clump's frame. The last atomic of a group comes from
 * g_load_stopclock_659ca8 instead of the slot table (inferred from the SPECIAL_ATOMIC_INDEX branch).
 * Finally sets the stopclock state globals (g_load_stopclock_659b19..2b) to their start values.
 * Parameters: none. Returns nothing.
 */

/* stopclock2.dff */

void load_stopclock(void)

{
  int clump;
  RpAtomic *atomic;
  RwFrame *frame;
  int *slot_ptr;
  undefined4 *clone_row;
  char *dff_name;
  undefined4 load_arg_1;
  undefined4 load_arg_2;
  RwV3d clump_pos;

  /* clump_load_by_name gets two extra args, both 0 (inferred: load flags) */
  load_arg_2 = 0;
  load_arg_1 = 0;
  dff_name = "stopclock2.dff";
  clump = clump_load_by_name("stopclock2.dff",load_arg_1,load_arg_2);
  mem_zero(&g_load_659b30,STOPCLOCK_ZERO_BYTES);
  g_load_659ce0 = *(RwFrame **)(clump + 4);
  g_game_659cdc = clump;
  clump_collect_atomics_by_id(clump,&g_load_659c38,0x20,0,0);
  /* walk the slot table g_load_659c38 up to g_game_659cb8; for each non-null slot call FUN_004c5410 (inferred: per-atomic setup) */
  slot_ptr = &g_load_659c38;
  do {
    if (*slot_ptr != 0) {
      FUN_004c5410(*slot_ptr,0x40,1);
    }
    slot_ptr = slot_ptr + 1;
  } while ((LONG_PTR)slot_ptr < ((LONG_PTR)&g_game_659cb8));
  /* one pass per source clump in g_load_stopclock_659c60..659c78: clone its atomics into the next row of the clone table */
  clone_row = &g_load_659b30;
  slot_ptr = &g_load_stopclock_659c60;
  do {
    /* source clump's frame translation (modelling matrix) is the position for all its clones */
    clump = *(int *)(*slot_ptr + 4);
    clump_pos.x = *(RwReal *)(clump + 0x40);
    clump_pos.y = *(RwReal *)(clump + 0x44);
    clump_pos.z = *(RwReal *)(clump + 0x48);
    clump = 0;
    /* inner loop: clone each atomic, give it its own frame under the root frame, and move it to clump_pos */
    do {
      atomic = g_load_stopclock_659ca8;
      /* SPECIAL_ATOMIC_INDEX keeps the single atomic from g_load_stopclock_659ca8; the others come from the slot table */
      if (clump != SPECIAL_ATOMIC_INDEX) {
        atomic = (RpAtomic *)(&g_load_659c38)[clump];
      }
      atomic = RpAtomicClone(atomic);
      clone_row[clump] = atomic;
      frame = RwFrameCreate();
      RpAtomicSetFrame((RpAtomic *)clone_row[clump],frame);
      RwFrameAddChild(g_load_659ce0,*(RwFrame **)(clone_row[clump] + 4));
      RwFrameTranslate(*(RwFrame **)(clone_row[clump] + 4),&clump_pos,rwCOMBINEREPLACE);
      clump = clump + 1;
    } while (clump < ATOMICS_PER_CLUMP);
    slot_ptr = slot_ptr + 1;
    clone_row = clone_row + ATOMICS_PER_CLUMP;
  } while ((LONG_PTR)slot_ptr < ((LONG_PTR)&g_load_stopclock_659c78));
  /* reset the stopclock state globals to their start values (meanings not known, values kept as-is) */
  g_game_659b18 = 0x32;
  g_load_stopclock_659b19 = 0x32;
  g_load_stopclock_659b1a = 0x32;
  g_load_stopclock_659b1d = 0;
  g_load_stopclock_659b20 = 0;
  g_load_stopclock_659b22 = 0;
  g_load_stopclock_659b24 = 0;
  g_load_stopclock_659b25 = 0;
  g_load_stopclock_659b1b = 0xff;
  g_load_stopclock_659b1c = 200;
  g_load_stopclock_659b1e = 0x12;
  g_load_stopclock_659b1f = 0xff;
  g_load_stopclock_659b21 = 0x96;
  g_load_stopclock_659b23 = 0xff;
  g_load_stopclock_659b26 = 200;
  g_load_stopclock_659b27 = 0xff;
  g_load_stopclock_659b28 = 0xff;
  g_load_stopclock_659b29 = 0xff;
  g_load_stopclock_659b2a = 0xff;
  g_load_stopclock_659b2b = 0xff;
  return;
}


