// cluster-entries: flashunit_load_assets
/* Installed cluster flashunit_load_assets (tools/cluster_install.py from output/cluster_pilot_kc/flashunit_load_assets.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00486a60*/ extern int ptank_set_colour();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int rpclump_clone(undefined4 p0);
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern undefined g_weapon_6a5d78[0x1000];
/*006a5dc8*/ extern undefined4 g_weapon_6a5dc8;
/*008bf000*/ extern undefined g_weapon_8bf000;
/*00460780*/ extern void LAB_00460780(void); /* shared tail: removes the clumps of the flash unit slots and clears the unit states */
/* flashunit_load_assets */

/* "flashunit.dff and lensflare-fs8 texture, flash particles" */

/* Loads flashunit.dff and builds 4 flash units (0x14 bytes each at g_weapon_6a5d78) with their two atomics, then sets up the lensflare-fs8 particle tank (inferred). */
void flashunit_load_assets(void)

{
  undefined4 flash_obj;
  int atomic_kind;
  RwTexture *texture;
  undefined *unit_cursor;
  int atomic_idx;
  undefined4 render_flag;
  undefined4 atomics_and_colour [6];
  
  atomics_and_colour[2] = 0;
  atomics_and_colour[3] = 0;
  atomics_and_colour[4] = 0x3f000000;
  atomics_and_colour[5] = 0x3f800000;
  mem_zero(g_weapon_6a5d78,0x50);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  g_weapon_6a5dc8 = clump_load_by_name("flashunit.dff", 0x0, 0x0);
  unit_cursor = g_weapon_6a5d78;
  do {
    flash_obj = rpclump_clone(g_weapon_6a5dc8);
    *(undefined4 *)(unit_cursor + 0x10) = flash_obj;
    clump_get_atomics(flash_obj,atomics_and_colour);
    atomic_idx = 0;
    do {
      flash_obj = atomics_and_colour[atomic_idx];
      atomic_kind = FUN_004c52b0(flash_obj,0,0);
      if (atomic_kind == 0) {
        render_flag = 1;
LAB_00460844:
        atomic_set_render_flag(flash_obj,render_flag);
      }
      else if (atomic_kind == 1) {
        render_flag = 0;
        goto LAB_00460844;
      }
      atomic_idx = atomic_idx + 1;
      *(undefined4 *)(unit_cursor + atomic_kind * 4) = flash_obj;
    } while (atomic_idx < 2);
    unit_cursor = unit_cursor + 0x14;
    if ((int)g_weapon_6a5d78 + 0x4f < (int)unit_cursor) {
      texture = (RwTexture *)find_texture_dict_b("lensflare-fs8");
      particle_system_setup_ptank((uint *)&g_weapon_8bf000,0x807,4,texture);
      ptank_set_blend_modes(&g_weapon_8bf000,5,2);
      ptank_set_colour(&g_weapon_8bf000,atomics_and_colour + 2);
      LAB_00460780();
      return;
    }
  } while( true );
}

