#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004601d0*/ extern int weapon_effects_clear_6a5c60();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005df73c*/ extern undefined g_weapon_drum_5df73c;
/*0060da34*/ extern undefined g_weapon_drum_60da34;
/*006a5a40*/ extern undefined g_weapon_6a5a40;
/*006a5a70*/ extern undefined4 g_weapon_drum_6a5a70[0x1000];
/*006a5c60*/ extern undefined g_weapon_6a5c60[0x1000];
/*006a5d10*/ extern undefined4 g_weapon_6a5d10;
/*006a5d14*/ extern undefined4 g_weapon_drum_6a5d14;
/*006a5c90*/ extern char g_weapon_drum_6a5c90[];
/*006a5cd8*/ extern char g_weapon_drum_6a5cd8[];

#define CLUMP_TABLE_BYTES (0xb0)
#define DRUM_SLOT_TABLE_BYTES (0x220)
#define CLUMP_ENTRY_SIZE (0x2c)
#define CLUMP_ENTRY_CLUMP_OFFSET (0x20)
#define CLUMP_ENTRY_PART_OFFSET (0x14)
#define ATOMIC_PARTS (3)
#define DRUM_SLOT_WORDS (0x11)
/*
 * Loads the depth charge weapon-drum assets (inferred).
 * Clears the clump and drum slot tables, then loads DepthCharge.dff and sorts each of its
 * clumps into CLUMP_TABLE entries: the clump at CLUMP_ENTRY_CLUMP_OFFSET, its atomics at
 * CLUMP_ENTRY_PART_OFFSET + 4 * part_index (part 0 render flag 1, parts 1-2 render flag 0).
 * Then loads DepthChargeRound.dff and fills each drum slot with a clone of its first atomic,
 * parented to a new frame. Takes no parameters and returns nothing.
 */

void weapon_drum_load_assets(void)

{
  undefined4 clump_or_atomic;
  int part_index;
  RpAtomic *round_atomic;
  RpAtomic *clone;
  RwFrame *frame;
  int atomic_idx;
  undefined *entry;
  int *drum_slot;
  undefined4 render_flag;
  undefined4 atomics [ATOMIC_PARTS];

  mem_zero(g_weapon_6a5c60,CLUMP_TABLE_BYTES);
  mem_zero(&g_weapon_6a5a40,DRUM_SLOT_TABLE_BYTES);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  g_weapon_drum_6a5d14 = clump_load_by_name("DepthCharge.dff", 0x0, 0x0);
  entry = g_weapon_6a5c60;
  /* One pass per clump entry (4 entries; the loop ends when entry passes the table end). */
  do {
    clump_or_atomic = rpclump_clone(g_weapon_drum_6a5d14);
    *(undefined4 *)(entry + CLUMP_ENTRY_CLUMP_OFFSET) = clump_or_atomic;
    clump_get_atomics(clump_or_atomic,atomics);
    atomic_idx = 0;
    /* Sort this clump's ATOMIC_PARTS atomics into entry slots by part_index; render flag 1 only for part 0. */
    do {
      clump_or_atomic = atomics[atomic_idx];
      part_index = atomic_geometry_user_data_int(clump_or_atomic,0,0);
      if (part_index == 0) {
        render_flag = 1;
LAB_004602a4:
        atomic_set_render_flag(clump_or_atomic,render_flag);
      }
      else if ((0 < part_index) && (part_index < ATOMIC_PARTS)) {
        render_flag = 0;
        goto LAB_004602a4;
      }
      atomic_idx = atomic_idx + 1;
      *(undefined4 *)(entry + part_index * 4 + CLUMP_ENTRY_PART_OFFSET) = clump_or_atomic;
    } while (atomic_idx < ATOMIC_PARTS);
    entry = entry + CLUMP_ENTRY_SIZE;
    if (((LONG_PTR)&g_weapon_drum_6a5cd8 + 0x37) < (LONG_PTR)entry) {
      render_flag = 0;
      clump_or_atomic = 0;
      entry = &g_weapon_drum_5df73c;
      g_weapon_6a5d10 = clump_load_by_name("DepthChargeRound.dff",clump_or_atomic,render_flag);
      round_atomic = (RpAtomic *)clump_first_atomic(g_weapon_6a5d10);
      drum_slot = g_weapon_drum_6a5a70;
      /* Per drum slot: drum_slot[0] = atomic clone, drum_slot[1] = new parent frame, drum_slot[2] = the clone's own frame. */
      do {
        clone = RpAtomicClone(round_atomic);
        *drum_slot = (int)clone;
        frame = RwFrameCreate();
        RpAtomicSetFrame((RpAtomic *)*drum_slot,frame);
        drum_slot[2] = *(int *)(*drum_slot + 4);
        frame = RwFrameCreate();
        drum_slot[1] = (int)frame;
        RwFrameAddChild(frame,(RwFrame *)drum_slot[2]);
        atomic_set_render_flag(*drum_slot,1);
        drum_slot = drum_slot + DRUM_SLOT_WORDS;
      } while ((LONG_PTR)drum_slot < ((LONG_PTR)&g_weapon_drum_6a5c90));
      weapon_effects_clear_6a5c60();
      RwV3dNormalize((RwV3d *)&g_weapon_drum_60da34,(RwV3d *)&g_weapon_drum_60da34);
      return;
    }
  } while( true );
}


