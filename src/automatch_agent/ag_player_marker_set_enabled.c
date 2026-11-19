#include "mfl_tier2.h"
/*0047a470*/ extern int car_set_wheel_contact();
/*006c3f30*/ extern undefined4 g_car_6c3f30;
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *name);
/*00475760*/ extern int aux_camera_get_position();
/*0047a470*/ extern int car_set_wheel_contact();
/*004846b0*/ extern int alloc_zeroed();
/*00485ae0*/ extern int particle_templates_init();
/*00485b80*/ extern int smoke_emitter_update();
/*00486470*/ extern undefined __cdecl fx_emitter_spawn_a(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00486530*/ extern undefined __cdecl fx_emitter_spawn_b(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004865f0*/ extern undefined __cdecl fx_emitter_spawn_c(undefined4 position, undefined4 direction, undefined4 angle, undefined4 size_a, undefined4 size_b);
/*00486710*/ extern int particles_destroy();
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00486ef0*/ extern int particles_render();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *bits_a, uint bits_b, uint bits_c, struct RwTexture *texture);
/*004b2b48*/ extern int _ftol();
/*004b38f7*/ extern int finite();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *vec);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *clump);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *clump, RpAtomicCallBack atomic, void *data);
/*004f8b60*/ extern struct RpGeometry * RpGeometryForAllMaterials(struct RpGeometry *geometry, RpMaterialCallBack callback, void *data);
/*00544e80*/ extern int RpRandom();
/*00555ff0*/ extern struct RwV3d * RpSplineFindPosition(struct RpSpline *spline, RwInt32 segments, RwReal t, struct RwV3d *position, struct RwV3d *tangent);
/*005625f0*/ extern RwBool __cdecl RpMaterialUVAnimExists(struct RpMaterial *material);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd61c*/ extern float k_f_0_001;
/*005de498*/ extern float k_f_4_656613en10;
/*005df83c*/ extern float k_f_1_0en6;
/*00629460*/ extern undefined4 g_gfx_629460;
/*00629468*/ extern undefined4 g_gfx_629468;
/*006c3f30*/ extern undefined4 g_car_6c3f30;
/*006c3f34*/ extern undefined4 g_car_6c3f34;
/*006c3f38*/ extern undefined4 g_gfx_6c3f38;
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;
/*006c40a4*/ extern undefined4 g_particles_6c40a4;
/*006c40a8*/ extern undefined4 g_particles_6c40a8;
/*006c40ac*/ extern undefined4 g_particles_6c40ac;
/*006c40b0*/ extern undefined4 g_particles_6c40b0;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c4100*/ extern undefined g_particles_6c4100;
/*006c4210*/ extern undefined4 g_effects_update_6c4210;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5040*/ extern undefined g_particles_6c5040;
/*006c5080*/ extern undefined g_particles_6c5080;
/*006c50c0*/ extern undefined4 g_particles_6c50c0;
/*006c50c8*/ extern float g_gfx_6c50c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef7f4*/ extern float g_tick_seconds;
/*0089ddfc*/ extern undefined4 g_gfx_89ddfc;
/* player_marker_set_enabled */

/* "(player, flag): enables the marker of the player" */

void player_marker_set_enabled(int player,int enable)

{
  int wheel;
  char *base;
  
  wheel = 0;
  base = (char *)&g_car_6c3f30 + player * 0x10;
  do {
    car_set_wheel_contact(base,wheel,player,enable == 0);
    car_set_wheel_contact(base + 4,wheel,player + 10,enable);
    wheel = wheel + 1;
  } while (wheel < 4);
  *(undefined4 *)(base + 8) = enable;
  return;
}
