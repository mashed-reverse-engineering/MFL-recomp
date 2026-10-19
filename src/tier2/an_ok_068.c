#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetModuleHandleA(int);
extern __declspec(dllimport) int __stdcall ShowCursor(int);
extern __declspec(dllimport) int __stdcall Sleep(int);
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

extern undefined4 __cdecl movie_poll_and_set_surface(char *param_1,char *param_2);


/* video_texture_setup */

/* "creates the 0x84 flag raster and the video texture graph" */

extern void video_texture_setup(char *param_1,char *param_2,char *param_3);


/* stub_return_one_004a4020 */

/* duplicate name: was stub_return_one (first address 004a34a0) */

extern undefined4 stub_return_one_004a4020(void);


/* app_get_app_76e890_4030 */

/* "return g_app_76e890;" */

extern undefined4 __cdecl app_get_app_76e890_4030(char *param_1);


/* video_texture_setup_004a4040 */

/* duplicate name: was video_texture_setup (first address 004a3fe0) */

extern void video_texture_setup_004a4040(char *param_1,char *param_2,char *param_3);


/* movie_poll_and_set_surface_b */

/* "identical wrapper" */

extern undefined4 __cdecl movie_poll_and_set_surface_b(char *param_1,char *param_2);


/* video_get_default_mode */

/* "g_app_76e8dc = g_app_76e8e0 = app_fn_4a34f0()" */

extern void video_get_default_mode(void);


/* platform_init */

/* "timer, dinput, joypads search" */

extern bool platform_init(void);


/* filesystem_register_native */

/* "game_register_native_filesystems() != 0" */

extern bool filesystem_register_native(void);


/* video_get_device_pointer */

/* "stores the device pointer (app_get_app_7e7704)" */

extern void video_get_device_pointer(undefined4 *param_1);


/* video_play_loop */

/* "loops the movie playback until a button is pressed, clearing and drawing each frame" */

extern void video_play_loop(void);


/* cursor_show */

/* "ShowCursor(1) when windowed" */

extern undefined4 cursor_show(void);


/* app_get_dinput_76ed10 */

/* "return g_dinput_76ed10;" */

extern undefined4 app_get_dinput_76ed10(void);


/* dinput_create */

/* "Creating DInput object" */

extern undefined4 dinput_create(void);


/* filesystem_register_native_b */

/* "game_register_native_filesystems() != 0" */

extern bool filesystem_register_native_b(void);


/* app_get_app_770f64 */

/* "return g_app_770f64;" */

extern undefined4 app_get_app_770f64(void);


/* joypad_get_axis_info */

/* "(joypad, axis, out): axis record 0x89 entries x 8 bytes per joypad" */

extern undefined4 __cdecl joypad_get_axis_info (char *param_1,char *param_2,undefined4 *param_3,undefined4 *param_4,undefined4 *param_5);

#define CONTROLLER_ENTRY_SIZE (0x448)


/* controller_dialog_copy_name */

/* "copies the 0x448 byte entry string index entry_index to the buffer" */
/* Copies the NUL-terminated string of entry entry_index (CONTROLLER_ENTRY_SIZE bytes per entry,
 * table g_controller_dialog_76ed4c) into dest. Returns 1 if copied, 0 if entry_index is out of
 * range (inferred: g_app_770f64 is the entry count) or dest is NULL. */

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
undefined4 controller_dialog_copy_name(int entry_index,int dest)
{
  char ch;
  char *src;

  if ((entry_index < (int)g_app_770f64) && (dest != 0)) {      /* diffemu: g_app_770f64 is undefined4, the original compares signed (jge) */
    src = &g_controller_dialog_76ed4c + entry_index * CONTROLLER_ENTRY_SIZE;
    /* dest is rebased to (dest - src) so src[dest] addresses the destination buffer */
    dest = dest - (int)src;
    /* copy byte by byte, including the NUL terminator */
    do {
      ch = *src;
      src[dest] = ch;
      src = src + 1;
    } while (ch != '\0');
    return 1;
  }
  return 0;
}


/* joypads_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "stores the HWND and searches the joypads" */

extern undefined4 joypads_init(char *param_1);


/* log_print */

/* debug log (string only) */

extern void log_print(char *_Str);


/* log_file_close */

/* "fclose(g_log_770f74)" */

extern void log_file_close(void);


/* controller_present_004a70c0 */

extern bool __cdecl controller_present_004a70c0(int controller_index);


/* app_get_app_770fb4 */

/* "return g_app_770fb4;" */

extern undefined4 app_get_app_770fb4(void);


/* joypad_get_field_770fb4 */

/* "(index): g_app_770fb4[index]" */

extern undefined4 __cdecl joypad_get_field_770fb4(char *param_1);



