// cluster-entries: weapon_shotgun_load_assets
/* Installed cluster weapon_shotgun_load_assets (tools/cluster_install.py from output/cluster_pilot_kc/weapon_shotgun_load_assets.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*006aab80*/ extern undefined g_weapon_shotgun_6aab80[0x1000];
/*006aac10*/ extern undefined4 g_weapon_shotgun_6aac10;
/*004676a0*/ extern void LAB_004676a0(void); /* shared tail: removes the clumps of all 4 shotgun slots */
/* weapon_shotgun_load_assets */

void weapon_shotgun_load_assets(void)

{
  int atomic;
  undefined4 clump;
  int part_id;
  int i;
  undefined *slot_cursor;
  RwV3d scale_vec;
  int atomics [3];
  
  mem_zero(g_weapon_shotgun_6aab80,0x90);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  g_weapon_shotgun_6aac10 = clump_load_by_name("shotgun.dff", 0x0, 0x0);
  slot_cursor = g_weapon_shotgun_6aab80;
  do {
    clump = rpclump_clone(g_weapon_shotgun_6aac10);
    *(undefined4 *)(slot_cursor + 0x1c) = clump;
    clump_get_atomics(clump,atomics);
    i = 0;
    do {
      atomic = atomics[i];
      part_id = FUN_004c52b0(atomic,0,0);
      if (part_id == 0) {
        atomic_set_render_flag(atomic,1);
      }
      else if ((0 < part_id) && (part_id < 3)) {
        atomic_set_render_flag(atomic,0);
        scale_vec.x = 4.0;
        scale_vec.y = 4.0;
        scale_vec.z = 4.0;
        RwFrameScale(*(RwFrame **)(atomic + 4),&scale_vec,rwCOMBINEPRECONCAT);
        FUN_004c5410(atomic,0x40,1);
      }
      i = i + 1;
      *(int *)(slot_cursor + part_id * 4) = atomic;
    } while (i < 3);
    slot_cursor = slot_cursor + 0x24;
  } while ((int)slot_cursor < 0x6aac10);
  LAB_004676a0();
  return;
}

