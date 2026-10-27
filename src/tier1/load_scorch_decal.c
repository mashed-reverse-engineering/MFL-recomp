#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00486a30*/ extern int particle_system_set_value_c0();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00752180*/ extern undefined g_load_752180;
/*007521c0*/ extern undefined g_sound_7521c0;
extern void fx_ring_clear(void);


/* load_scorch_decal */

/* scorch */

void load_scorch_decal(void)
{
  RwTexture *texture_dict;
  
  texture_dict = (RwTexture *)find_texture_dict_b("exp_cloud2");
  particle_system_setup_ptank((uint *)&g_load_752180,0x427,0x100,texture_dict);
  ptank_set_blend_modes(&g_load_752180,5,2);
  particle_system_set_value_c0(&g_load_752180,0);
  texture_dict = (RwTexture *)find_texture_dict_b("scorch");
  particle_system_setup_ptank((uint *)&g_sound_7521c0,0x80c,0x100,texture_dict);
  fx_ring_clear();
  return;
}
