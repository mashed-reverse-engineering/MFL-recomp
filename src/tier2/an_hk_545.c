#include "mfl_tier2.h"
extern char PTR_g_data_6c696f_005df7f0[];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040e530*/ extern undefined __cdecl load_shine_texture(struct RpClump *p0, undefined4 p1, int p2);
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004f6bf0*/ extern void * FUN_004f6bf0(undefined4 p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a7c38*/ extern undefined g_weapon_6a7c38;
/*006a7c84*/ extern undefined4 g_weapon_6a7c84;
/*006a7ce8*/ extern undefined g_weapon_6a7ce8;
static int weapon_fx_slot_clear_c_r(undefined4 * reg_eax);
extern void weapon_fx_slot_clear_c(void);
extern char g_weapon_6a7cc4[];
/* float 0.4f as raw bits; the value passed to load_shine_texture (inferred: shine strength) */
#define SHINE_ARG_0_4F (0x3ecccccd)
__declspec(naked) int weapon_fx_slot_clear_c_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_fx_slot_clear_c
        ret
    }
}

/* oil_load_assets */

/* "oilShine texture and oilslicker.dff" */

/* Loads the oil slick assets: the two particle textures (set up on the two ptank
   particle systems), and the oilslicker.dff clump. Then it creates one oilslicker
   clump per weapon slot in g_weapon_6a7c84..g_weapon_6a7cc4 (stride 16 bytes), stores
   it in the slot, applies the shine (inferred), clears the slot's weapon fx (inferred) and
   flags the atomics. The base clump is destroyed at the end. Takes no parameters, returns nothing. */
void oil_load_assets(void)

{
  RwTexture *particle_texture;
  RwTexture *shine_texture;
  RpClump *oilslicker_clump;
  RpClump *slot_clump;
  undefined4 *slot_ptr;
  
  particle_texture = (RwTexture *)find_texture_dict_b((RwChar *)&PTR_g_data_6c696f_005df7f0);
  shine_texture = (RwTexture *)find_texture_dict_b("oilShine");
  particle_system_setup_ptank((uint *)&g_weapon_6a7ce8,0x80c,0x40,particle_texture);
  ptank_set_blend_modes(&g_weapon_6a7ce8,5,6);
  particle_system_setup_ptank((uint *)&g_weapon_6a7c38,0x4c,0x40,shine_texture);
  ptank_set_blend_modes(&g_weapon_6a7c38,5,2);
  oilslicker_clump = (RpClump *)clump_load_by_name("oilslicker.dff", 0x0, 0x0);
  slot_ptr = &g_weapon_6a7c84;
  /* per weapon slot: spawn an oilslicker instance, store it in the slot, set up its shine */
  do {
    slot_clump = (RpClump *)FUN_004f6bf0(oilslicker_clump);
    *slot_ptr = slot_clump;
    load_shine_texture(slot_clump,SHINE_ARG_0_4F,0);
    weapon_fx_slot_clear_c_r(slot_ptr + -3);
    clump_set_flag_on_atomics((RpClump *)*slot_ptr, 0x0);
    slot_ptr = slot_ptr + 4;
  } while ((LONG_PTR)slot_ptr < ((LONG_PTR)&g_weapon_6a7cc4));
  RpClumpDestroy(oilslicker_clump);
  return;
}


