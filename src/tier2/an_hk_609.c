#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00464060*/ extern int weapon_effects_clear_6a8af8();
/*00486a60*/ extern int ptank_set_colour();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*0060da4c*/ extern undefined g_mine_load_60da4c;
/*006a7d38*/ extern undefined g_weapon_6a7d38[0x1000];
/*006a8ab8*/ extern undefined g_weapon_6a8ab8;
/*006a8af8*/ extern undefined g_weapon_6a8af8;
/*006a8b0c*/ extern undefined4 g_weapon_6a8b0c[0x1000];
/*006a8b58*/ extern undefined4 g_weapon_6a8b58;
/*006a8b5c*/ extern undefined4 g_weapon_6a8b5c;
/*006a7e04*/ extern char g_weapon_mines_6a7e04[];
/*006a8b6c*/ extern char g_weapon_6a8b6c[];

/* mine record: 0x6c bytes, 2 atomic pointers at +0x50, clump at +0x58 */
#define MINE_RECORD_SIZE (0x6c)
#define MINE_RECORD_ARRAY_SIZE (0xd80)
#define MINE_RECORD_ATOMICS_OFFSET (0x50)
#define MINE_RECORD_CLUMP_OFFSET (0x58)
#define MINE_ATOMICS_PER_CLUMP (2)
#define DROPPER_SLOT_STRIDE (6)
/* ptank blend factors passed to ptank_set_blend_modes (values inferred: RwBlendFunction rwBLENDSRCALPHA, rwBLENDONE) */
#define PTANK_BLEND_SRC (5)
#define PTANK_BLEND_DST (2)

/*
 * mine_load_assets (inferred): loads the mine assets used by the weapon code.
 * Loads MineDropper.dff and stores a clump copy for each dropper slot (6 words apart)
 * in g_weapon_6a8b0c. Loads Mine.dff and fills the MINE_RECORD_ARRAY_SIZE / MINE_RECORD_SIZE
 * records in g_weapon_6a7d38: each record gets a clump copy at +MINE_RECORD_CLUMP_OFFSET and
 * the two atomics of that clump at +MINE_RECORD_ATOMICS_OFFSET, each with a render flag set.
 * Finally sets up the lens flare particle system in g_weapon_6a8ab8 with the "lensflare-fs8" texture.
 * Takes no parameters and returns nothing.
 */

/* "MineDropper.dff, Mine.dff and the lens flare" */

void mine_load_assets(void)

{
  undefined4 clump;
  int atomic_slot;
  RwTexture *lensflare_texture;
  undefined *mine_record;
  undefined4 *dropper_slot;
  int atomic_index;
  undefined4 render_flag;
  undefined4 atomics [2];

  mem_zero(&g_weapon_6a8af8,0x60);
  mem_zero(g_weapon_6a7d38,MINE_RECORD_ARRAY_SIZE);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  g_weapon_6a8b5c = clump_load_by_name("MineDropper.dff", 0x0, 0x0);
  /* fill the dropper slots: one clump copy per slot, until the end of g_weapon_6a8b0c */
  dropper_slot = g_weapon_6a8b0c;
  do {
    clump = rpclump_clone(g_weapon_6a8b5c);
    *dropper_slot = clump;
    dropper_slot = dropper_slot + DROPPER_SLOT_STRIDE;
  } while ((LONG_PTR)dropper_slot < ((LONG_PTR)&g_weapon_6a8b6c));
  g_weapon_6a8b58 = clump_load_by_name("Mine.dff", 0x0, 0x0);
  /* one loop iteration per mine record: store the clump, then set the render flag of its atomics */
  mine_record = g_weapon_6a7d38;
  do {
    clump = rpclump_clone(g_weapon_6a8b58);
    *(undefined4 *)(mine_record + MINE_RECORD_CLUMP_OFFSET) = clump;
    clump_get_atomics(clump,atomics);
    atomic_index = 0;
    /* for each of the two atomics: set render flag by atomic_geometry_user_data_int result (0 -> flag 1, 1 -> flag 0) */
    do {
      clump = atomics[atomic_index];
      atomic_slot = atomic_geometry_user_data_int(clump,0,0);
      if (atomic_slot == 0) {
        render_flag = 1;
LAB_00464161:
        atomic_set_render_flag(clump,render_flag);
      }
      else if (atomic_slot == 1) {
        render_flag = 0;
        goto LAB_00464161;
      }
      atomic_index = atomic_index + 1;
      *(undefined4 *)(mine_record + atomic_slot * 4 + MINE_RECORD_ATOMICS_OFFSET) = clump;
    } while (atomic_index < MINE_ATOMICS_PER_CLUMP);
    mine_record = mine_record + MINE_RECORD_SIZE;
    if (((LONG_PTR)&g_weapon_mines_6a7e04 + 0xcb3) < (LONG_PTR)mine_record) {
      weapon_effects_clear_6a8af8();
      lensflare_texture = (RwTexture *)find_texture_dict_b("lensflare-fs8");
      particle_system_setup_ptank((uint *)&g_weapon_6a8ab8,0,0x20,lensflare_texture);
      ptank_set_blend_modes(&g_weapon_6a8ab8,PTANK_BLEND_SRC,PTANK_BLEND_DST);
      ptank_set_colour(&g_weapon_6a8ab8,&g_mine_load_60da4c);
      return;
    }
  } while( true );
}


