// cluster-entries: app_boot_init
/* Installed cluster app_boot_init (tools/cluster_install.py from output/cluster_pilot_ql1/app_boot_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5b60*/ extern int d3d9_pipeline_create_default(void);
/*004a5530*/ extern void __cdecl log_print(const char *text);
/*004c66c0*/ extern void __cdecl piz_fs_create_004c67c0(int use_fast_path);
/*004c66e0*/ extern void __cdecl piz_set_progress_callback_004c6790(void (*callback)(void), int unused);
/*0042d970*/ extern void build_info_frame(void);
/*004a6b70*/ extern void render_pipelines_create_two(void);
/*004dafe0*/ extern int **RwD3D9GetCurrentD3DDevice(void);
/*004d3090*/ extern int RwEngineGetCurrentVideoMode(void);
/*004d3060*/ extern int RwEngineGetVideoModeInfo(RwVideoMode *mode, int index);
/*004a8830*/ extern int app_get_app_62c5c8(void);
/*004a8840*/ extern int app_get_app_62c5cc(void);
/*004a44a0*/ extern void video_play_loop(void);
/*004a43d0*/ extern void __cdecl game_open_piz(const char *path);
/*004a4440*/ extern void game_close_piz(void);
/*0042c9e0*/ extern void speaker_mode_names(void);
/*0042c2e0*/ extern void ui_rt2d_init(void);
/*00475730*/ extern void *__cdecl get_viewport_camera(int index);
/*0042bb50*/ extern void __cdecl ui_2d_begin(void *camera);
/*0042ba10*/ extern int language_offset_store(void);
/*0042ba20*/ extern void dat_load_language(void);
/*00475750*/ extern void *car_get_car_6c2944(void);
/*0047fb10*/ extern void __cdecl load_ingame_font(void *car);
/*00413900*/ extern void replay_buffer_alloc(void);
/*00568810*/ extern void *__cdecl RtCharsetCreate(const RwRGBA *foreground, const RwRGBA *background);
/*00495d30*/ extern int sector_table_clear(void);
/*00495cd0*/ extern void *__cdecl node_create_from_two_vectors(void *camera, const void *width_source, const float *position);
/*004733c0*/ extern void sound_stuff_init(void);
/*0040d150*/ extern void race_load_common_assets(void);
/*00481d50*/ extern void surface_flag_table_init(void);
/*00473ab0*/ extern void course_object_pair_counts_clear(void);
/*0050b5e0*/ extern void nop_stub(void);
/*0040d0f0*/ extern void *__cdecl find_texture_dict_c(const char *name);
/*0040d0d0*/ extern void *__cdecl find_texture_dict_b(const char *name);
/*0049ad40*/ extern void __cdecl fx_ptank_atomics_create(void *texture);
/*00403f20*/ extern void __cdecl movie_texture_open(void *camera);
/*0048cfd0*/ extern void __cdecl video_raster_create_forward(void *camera, int unused);
/*004a9dc0*/ extern int app_counters_clear(void);
/*004679a0*/ extern void powerups_load_assets(void);
/*0041b960*/ extern void hudpanel_load_assets_0041b960(void);
/*0041e4d0*/ extern void score_panel_load(void);
/*0041ef40*/ extern void load_team_panels(void);
/*0041fb90*/ extern void load_endpoint_panel(void);
/*00420760*/ extern void load_team_endpoint_panel(void);
/*00420fc0*/ extern void load_stopclock(void);
/*00420930*/ extern void load_start_lights(void);
/*0041f180*/ extern void load_kc_panel(void);
/*00420c10*/ extern void stopclock_load(void);
/*0041d260*/ extern void load_btb_panel(void);
/*00421b90*/ extern void car_shadow_setup(void);
/*00423f00*/ extern void car_damage_init(void);
/*00414f10*/ extern void trail_indices_init(void);
/*00480bb0*/ extern void physics_scenarios_reset_00480bb0(void);
/*00405230*/ extern void scene_tables_clear(void);
/*00429c70*/ extern void load_copters_txd(void);
/*00402f40*/ extern void cup_scene_load_models(void);
/*004c3d80*/ extern void *__cdecl FUN_004c3d80(const char *path, int a, int b);
/*00437770*/ extern void __cdecl menu_set_69cfc4(float value);
/*00403830*/ extern void attract_timer_init(void);
/*0042c9d0*/ extern void __cdecl build_info_set_page(int page);
/*00416610*/ extern void noise_table_init(void);
/*00402fb0*/ extern void __cdecl cup_scene_clear_screen(int mode);
/*00771a20*/ extern int g_os_771a20;
/*0076e898*/ extern int **g_d3dx_76e898;
/*007eeb64*/ extern void *g_ai_7eeb64;
/*007eebb0*/ extern void *g_app_7eebb0;
/*008d9978*/ extern void *g_app_boot_8d9978;
/*008d9974*/ extern void *g_app_boot_8d9974;
/*0064d888*/ extern void *g_app_64d888;

typedef int (__stdcall *D3DSetRenderState)(void *device, int state, int value);
#define D3DDEV_SET_RENDER_STATE 0xe4        /* byte offset in the IDirect3DDevice9 vtable */
#define D3DRS_ALPHAREF 0x18
#define D3DRS_ALPHAFUNC 0x19
#define D3DCMP_GREATER 5

/* g_os_771a20 values (set at startup from the Windows version) for which the PIZ file system is created with the flag set */
#define OS_PIZ_FLAG(os) ((os) == 2 || (os) == 3 || (os) == 4 || (os) == 7)
#define MENU_CLEAR_ALPHA 0.3f
#define INFO_PAGE_BOOT 3
#define CLEAR_SCREEN_MODE_BOOT 4

/* The original keeps these small objects in one frame block: two RtCharset colours, the opaque white colour and position of the
   first node, and the video mode record. */
typedef struct BootFrame {
  RwRGBA background;
  RwRGBA foreground;
  RwRGBA white;
  float position[2];
  RwVideoMode video_mode;
} BootFrame;

/* app_boot_init: boot of the game resources after the window exists: D3D9 pipeline, PIZ file system, render pipelines, alpha test state,
   intro video, fonts, language, char set, sectors, textures, HUD panels, car and cup assets. Returns 1 when everything came up,
   0 when the pipeline or the counters could not be created. */
int app_boot_init(void)
{
  BootFrame frame;
  int piz_flag;
  int mode_index;
  void *camera;
  void *car;

  frame.foreground.red = 0x40;
  frame.foreground.green = 0xff;
  frame.foreground.blue = 0x40;
  frame.foreground.alpha = 0xff;
  frame.background.red = 8;
  frame.background.green = 8;
  frame.background.blue = 8;
  frame.background.alpha = 0x80;
  piz_flag = 0;
  if (!d3d9_pipeline_create_default()) {
    return 0;
  }
  log_print("Setting up PIZ file system\n");
  if (OS_PIZ_FLAG(g_os_771a20)) {
    piz_flag = 1;
  }
  piz_fs_create_004c67c0(piz_flag);
  piz_set_progress_callback_004c6790(build_info_frame, 0);
  log_print("\tOK\n");
  log_print("Calling PCVS_Init\n");
  render_pipelines_create_two();
  g_d3dx_76e898 = RwD3D9GetCurrentD3DDevice();
  ((D3DSetRenderState)(*g_d3dx_76e898)[D3DDEV_SET_RENDER_STATE / 4])(g_d3dx_76e898, D3DRS_ALPHAFUNC, D3DCMP_GREATER);
  ((D3DSetRenderState)(*g_d3dx_76e898)[D3DDEV_SET_RENDER_STATE / 4])(g_d3dx_76e898, D3DRS_ALPHAREF, 0);
  mode_index = (short)RwEngineGetCurrentVideoMode();
  RwEngineGetVideoModeInfo(&frame.video_mode, mode_index);
  app_get_app_62c5c8();
  app_get_app_62c5cc();
  log_print("Calling HardwareShowIntroVideo\n");
  video_play_loop();
  game_open_piz("d:\\toastart\\common\\font36.piz");
  speaker_mode_names();
  ui_rt2d_init();
  ui_2d_begin(get_viewport_camera(0));
  language_offset_store();
  dat_load_language();
  car = car_get_car_6c2944();
  load_ingame_font(car);
  game_close_piz();
  replay_buffer_alloc();
  g_ai_7eeb64 = RtCharsetCreate(&frame.foreground, &frame.background);
  if (sector_table_clear() != 0) {
    frame.position[0] = 12.0f;
    frame.position[1] = 15.0f;
    frame.white.red = 0xff;
    frame.white.green = 0xff;
    frame.white.blue = 0xff;
    frame.white.alpha = 0xff;
    camera = get_viewport_camera(0);
    g_app_7eebb0 = node_create_from_two_vectors(camera, &frame.white, frame.position);
  }
  sound_stuff_init();
  race_load_common_assets();
  surface_flag_table_init();
  course_object_pair_counts_clear();
  nop_stub();
  g_app_boot_8d9978 = find_texture_dict_c("SemiC");
  g_app_boot_8d9974 = find_texture_dict_c("SemiC2");
  fx_ptank_atomics_create(find_texture_dict_b("lensflare-fs8"));
  log_print("Calling BackBuffRasterOpen\n");
  movie_texture_open(get_viewport_camera(0));
  video_raster_create_forward(get_viewport_camera(0), 0);
  if (app_counters_clear() == 0) {
    return 0;
  }
  game_open_piz("d:\\toastart\\common\\powerups\\powerups.piz");
  powerups_load_assets();
  hudpanel_load_assets_0041b960();
  game_close_piz();
  game_open_piz("d:\\toastart\\common\\Panel\\Panel.piz");
  score_panel_load();
  load_team_panels();
  load_endpoint_panel();
  load_team_endpoint_panel();
  load_stopclock();
  load_start_lights();
  load_kc_panel();
  stopclock_load();
  load_btb_panel();
  game_close_piz();
  car_shadow_setup();
  car_damage_init();
  trail_indices_init();
  physics_scenarios_reset_00480bb0();
  scene_tables_clear();
  game_open_piz("d:\\ToastArt\\common\\perm.piz");
  load_copters_txd();
  cup_scene_load_models();
  game_close_piz();
  g_app_64d888 = FUN_004c3d80("toastart\\pc\\pc.txd", 0, 0);
  menu_set_69cfc4(MENU_CLEAR_ALPHA);
  attract_timer_init();
  build_info_set_page(INFO_PAGE_BOOT);
  noise_table_init();
  cup_scene_clear_screen(CLEAR_SCREEN_MODE_BOOT);
  return 1;
}

