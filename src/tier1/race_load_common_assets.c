#include "mfl_tier2.h"
/*00401000*/ extern int load_cup_model();
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00416200*/ extern int trail_particles_create();
/*004241e0*/ extern int car_damage_smoke_create();
/*0042f860*/ extern int __cdecl texdict_load_named_b(char *p0, undefined4 p1, undefined4 p2);
/*00485560*/ extern int particle_pools_init();
/*004855b0*/ extern undefined __cdecl particle_pool_add(struct RwTexture *p0, uint p1);
/*004860c0*/ extern int smoke_particles_init();
/*00487480*/ extern int shockwave_load_models();
/*00487ec0*/ extern int effect_pool_init();
/*004983b0*/ extern int fx_spritecomp_create();
/*004988e0*/ extern int fx_explosion_systems_init();
/*00499ae0*/ extern int load_scorch_decal();
/*0049a940*/ extern undefined __cdecl fx_dust_create_default(undefined4 p0, undefined4 p1);
/*0049b750*/ extern int fx_slots_clear();
/*0049c920*/ extern int fx_objects_clear();
/*0049d1e0*/ extern int load_waterfall_fx();
/*0049dfc0*/ extern int fx_texture_load();
/*0049fe00*/ extern int tri_model_load();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004b322b*/ extern __declspec(noreturn) void __cdecl exit(int);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*00652dd0*/ extern undefined4 g_effects_652dd0;
/*00652dd4*/ extern undefined4 g_effects_652dd4;
/*00652dd8*/ extern undefined4 g_load_652dd8;
/*00652ddc*/ extern undefined4 g_effects_652ddc;
/*00652de0*/ extern undefined4 g_effects_652de0;
/*00652de4*/ extern undefined4 g_effects_652de4;
/* particle pool capacities for the "scorch" and "wfall" textures (inferred: pool size) */
#define SCORCH_POOL_SIZE (0x18)
#define WFALL_POOL_SIZE (0x10)
/* race_load_common_assets */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "opens sfx.piz, fx.txd; loads cup / tri / shockwave models"
 *
 * Loads the assets shared by all races: opens sfx.piz, loads the fx, badges,
 * track-images and interface texture dictionaries, loads the cup, tri and
 * shockwave models, creates the particle / effect pools and the fx systems,
 * then closes sfx.piz. Takes no parameters and returns nothing. Sets
 * g_effects_652dd4 to 1 at the end (inferred: "common assets loaded" flag).
 * If sfx.piz cannot be read (g_load_652dd8 == 0) it prints an error and exits.
 */
void race_load_common_assets(void)
{
  RwTexture *texture_dict;

  /* open the sfx archive; everything below loads from it */
  g_effects_652dd0 = 0;
  game_open_piz("d:\\toastart\\common\\sfx.piz");

  g_load_652dd8 = texdict_load_named_b("fx.txd",0,0);
  fx_texture_load();
  g_effects_652ddc = texdict_load_named_b("badges.txd",0,0);
  g_effects_652de0 = texdict_load_named_b("TrackImages.txd",0,0);
  g_effects_652de4 = texdict_load_named_b("Interface.txd",0,0);
  /* clear the current texture dictionary before loading models */
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);

  /* models and particle / effect pools */
  load_cup_model();
  tri_model_load();
  shockwave_load_models();
  smoke_particles_init();
  particle_pools_init(2);
  texture_dict = (RwTexture *)find_texture_dict_b("scorch");
  particle_pool_add(texture_dict,SCORCH_POOL_SIZE);
  texture_dict = (RwTexture *)find_texture_dict_b("wfall");
  particle_pool_add(texture_dict,WFALL_POOL_SIZE);
  effect_pool_init();
  load_waterfall_fx();
  fx_spritecomp_create();
  /* done with sfx.piz */
  game_close_piz();

  /* fx.txd failed to load: report it and stop */
  if (g_load_652dd8 == 0) {
    FID_conflict__wprintf("cant open sfx dic\n");
    exit(0x0);
  }
  /* fx systems: dust default uses the RWObjShad dictionary's texture (inferred) */
  fx_slots_clear();
  fx_dust_create_default(100,find_texture_dict_b("RWObjShad"));
  fx_explosion_systems_init();
  load_scorch_decal();
  trail_particles_create();
  car_damage_smoke_create();
  fx_objects_clear();
  g_effects_652dd4 = 1;
  return;
}
