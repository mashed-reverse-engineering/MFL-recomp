#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c49b0*/ extern void FUN_004c49b0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*0064d88c*/ extern undefined4 g_game_64d88c;
/*00659d60*/ extern undefined4 g_game_659d60;
/*00659d74*/ extern undefined4 g_game_659d74;
/*00659d78*/ extern undefined4 g_game_659d78;
/*00659d80*/ extern undefined g_car_shadow_659d80;
/*00659dc0*/ extern undefined g_car_shadow_659dc0;
/*0065a9b0*/ extern undefined4 g_load_65a9b0;
/*0065a9b4*/ extern undefined4 g_load_65a9b4;
/*0065a9cc*/ extern undefined4 g_particles_65a9cc;
/*0065a9d0*/ extern undefined4 g_particles_65a9d0;
/*00659d70*/ extern char g_particles_659d70[];
/*
 * Sets up the car shadow resources. Takes no parameters, returns nothing.
 * Fills the global frame slots (g_game_659d60 up to g_particles_659d70) with new RwFrames,
 * creates two particle systems ("beam" and "car_shadow" textures), and, when
 * g_game_64d88c is 0, sets both cameras to parallel projection with a 0.75 view window.
 */
void car_shadow_setup(void)

{
  RwFrame *frame;
  RwTexture *texture_dict;
  undefined4 *frame_slot;
  RwV2d view_window;

  /* create one frame per slot until the end of the frame table (inferred) */
  frame_slot = &g_game_659d60;
  do {
    frame = RwFrameCreate();
    *frame_slot = frame;
    frame_slot = frame_slot + 1;
  } while ((LONG_PTR)frame_slot < ((LONG_PTR)&g_particles_659d70));
  g_particles_65a9cc = (uint *)&g_car_shadow_659dc0;
  texture_dict = (RwTexture *)find_texture_dict_b("beam");
  particle_system_setup_ptank(g_particles_65a9cc,0x80c,4,texture_dict);
  ptank_set_blend_modes(g_particles_65a9cc,5,2);
  g_particles_65a9d0 = (uint *)&g_car_shadow_659d80;
  texture_dict = (RwTexture *)find_texture_dict_b("car_shadow");
  particle_system_setup_ptank(g_particles_65a9d0,0x80c,4,texture_dict);
  /* two parallel-projection cameras, only when g_game_64d88c is 0 */
  if (g_game_64d88c == 0) {
    view_window.x = 0.75;
    view_window.y = 0.75;
    FUN_004c49b0(0x100,0x100,&g_load_65a9b0,&g_game_659d74);
    RwCameraSetViewWindow(g_load_65a9b0,&view_window);
    RwCameraSetProjection(g_load_65a9b0,rwPARALLEL);
    FUN_004c49b0(0x40,0x40,&g_load_65a9b4,&g_game_659d78);
    RwCameraSetViewWindow(g_load_65a9b4,&view_window);
    RwCameraSetProjection(g_load_65a9b4,rwPARALLEL);
  }
  return;
}


