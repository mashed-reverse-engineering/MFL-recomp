#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004661f0*/ extern int weapon_fx_grid_clear();
/*0049b780*/ extern int __cdecl particles_alloc_system(struct RwTexture *p0, int p1, int p2, uint p3);
/*0049b860*/ extern int fx_set_blend_modes();
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(void *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a9588*/ extern undefined g_weapon_6a9588;
/*006a9728*/ extern undefined4 g_weapon_6a9728[0x1000];
/*006aab78*/ extern char *g_weapon_6aab78;
/* flamethrower_load_assets */

/* "FlameThrower.dff and its particle systems" */

void flamethrower_load_assets(void)

{
  RpClump *clump;
  undefined4 clump_or_particles;
  int atomic_count;
  int atomic_flag;
  RwMatrix *ltm;
  undefined4 *slot_end;
  undefined4 *slot;
  RwReal *dst_matrix;
  int atomics [10];
  
  clump = (RpClump *)clump_load_by_name("FlameThrower.dff", 0x0, 0x0);
  g_weapon_6aab78 = (char *)((RwTexture *)find_texture_dict_b("exp_cloud2"));
  slot_end = g_weapon_6a9728;
  do {
    slot = slot_end + -0x1a;
    clump_or_particles = FUN_004f6bf0(clump);
    slot_end[-0x12] = clump_or_particles;
    atomic_count = clump_get_atomics((RpClump *)clump_or_particles,(RpAtomic **)atomics);
    while (atomic_count != 0) {
      atomic_flag = atomic_count + -1;
      atomic_count = atomic_count + -1;
      atomic_flag = FUN_004c52b0(atomics[atomic_flag],0,0);
      if (atomic_flag == 1) {
        atomic_flag = atomics[atomic_count];
        slot_end[-0x11] = atomic_flag;
        ltm = RwFrameGetLTM(*(RwFrame **)(atomic_flag + 4));
        dst_matrix = (RwReal *)(slot_end + -0x10);
        for (atomic_flag = 0x10; atomic_flag != 0; atomic_flag = atomic_flag + -1) {
          *dst_matrix = (ltm->right).x;
          ltm = (RwMatrix *)&(ltm->right).y;
          dst_matrix = dst_matrix + 1;
        }
      }
    }
    weapon_fx_grid_clear(slot);
    clump_or_particles = particles_alloc_system((struct RwTexture *)g_weapon_6aab78,0x1e,6,0x37);
    slot_end[-0x16] = clump_or_particles;
    fx_set_blend_modes(clump_or_particles,5,2);
    slot_end = slot;
  } while (slot != (undefined4 *)&g_weapon_6a9588);
  RpClumpDestroy(clump);
  return;
}


