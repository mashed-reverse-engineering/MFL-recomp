#include "mfl_tier2.h"
/*004a2d20*/ extern int rw_set_value_at_root_plus_947144();
/*004a34f0*/ extern int app_get_app_76e8a4();
/*004a37f0*/ extern int movie_renderer_poll();
/*0062ad50*/ extern undefined4 g_app_62ad50;
/*0076e89c*/ extern undefined4 g_d3dx_76e89c;
/*00475730*/ extern int get_viewport_camera();
/*004a20d0*/ extern int input_update();
/*004a2d20*/ extern int rw_set_value_at_root_plus_947144();
/*004a3490*/ extern int __cdecl app_get_app_76e890_3490(undefined4 p0);
/*004a34a0*/ extern int stub_return_one();
/*004a34b0*/ extern int app_get_two_size_pairs();
/*004a34f0*/ extern int app_get_app_76e8a4();
/*004a3500*/ extern int video_draw_textured_quad();
/*004a37f0*/ extern int movie_renderer_poll();
/*004a39a0*/ extern undefined __cdecl movie_stop(undefined4 p0);
/*004a3a20*/ extern int movie_renderer_create();
/*004a4260*/ extern int timer_init_performance_counter();
/*004a4700*/ extern int dinput_init();
/*004a4710*/ extern int dinput_release_object_004a4710();
/*004a4720*/ extern int game_register_native_filesystems();
/*004a5160*/ extern int dinput_release_all();
/*004a5190*/ extern int joypads_search();
/*004a5500*/ extern int dinput_keyboard_init();
/*004a5520*/ extern int dinput_release_keyboard_004a5520();
/*004a8180*/ extern undefined __stdcall controller_dialog_run(int p0, int p1);
/*004a8420*/ extern int debug_printf();
/*004a87d0*/ extern int video_dialog_free();
/*004a8860*/ extern int app_get_app_7712f4();
/*004a9380*/ extern int app_get_app_7e7704();
/*004ab220*/ extern int DirectInput8Create();
/*004b4244*/ extern int __cdecl _fputs(char *p0, FILE *p1);
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4ba7*/ extern int entry();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*0050b5e0*/ extern int nop_stub();
/*005e12e4*/ extern undefined g_dinput_release_5e12e4;
/*005e1f6c*/ extern undefined g_dinput_create_5e1f6c;
/*0062ad38*/ extern undefined4 g_app_62ad38;
/*0062ad50*/ extern undefined4 g_app_62ad50;
/*0076e870*/ extern undefined4 g_app_76e870;
/*0076e874*/ extern undefined4 g_language_offset;
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e894*/ extern undefined4 g_app_76e894;
/*0076e89c*/ extern undefined4 g_d3dx_76e89c;
/*0076e8a4*/ extern undefined4 g_app_76e8a4;
/*0076e8dc*/ extern undefined4 g_app_76e8dc;
/*0076e8e0*/ extern undefined4 g_app_76e8e0;
/*0076ed10*/ extern undefined4 g_dinput_76ed10;
/*0076ed18*/ extern undefined4 g_app_76ed18;
/*0076ed20*/ extern undefined4 g_dinput_76ed20;
/*0076ed4c*/ extern undefined g_controller_dialog_76ed4c;
/*0076efe8*/ extern undefined g_joypad_get_76efe8;
/*0076efec*/ extern undefined g_joypad_get_76efec;
/*0076f068*/ extern undefined g_joypad_get_76f068;
/*0076f06c*/ extern undefined g_joypad_get_76f06c;
/*0076f128*/ extern undefined g_joypad_get_76f128;
/*00770f60*/ extern float g_app_770f60;
/*00770f64*/ extern undefined4 g_app_770f64;
/*00770f68*/ extern undefined4 g_app_770f68;
/*00770f74*/ extern undefined4 g_log_770f74;
/*00770fb4*/ extern undefined4 g_app_770fb4;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/* movie_poll_and_set_surface */

/* "movie_renderer_poll and sets the current surface via app_fn_4a2d20" */

undefined4 __cdecl movie_poll_and_set_surface(undefined4 param_1,undefined4 param_2)
{
  undefined4 uVar1;
  
  movie_renderer_poll();
  uVar1 = app_get_app_76e8a4((&g_d3dx_76e89c)[g_app_62ad50]);
  rw_set_value_at_root_plus_947144(uVar1);
  return 1;
}
