#include "mfl_tier2.h"
/*00403030*/ extern int app_boot_init();
/*00403320*/ extern int app_shutdown_resources();
/*00405e40*/ extern int stub_return_zero();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*004a1590*/ extern int app_initialise_rw_and_camera();
/*004a15b0*/ extern int app_main_loop();
/*004a1890*/ extern int game_options_reset();
/*004a2940*/ extern int app_shutdown_sequence_a(undefined4 p0, undefined4 p1);
/*004a2950*/ extern int rw_engine_shutdown();
/*004a2960*/ extern int cursor_show_if_windowed();
/*004a2d40*/ extern int app_parse_command_line();
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004a9360*/ extern int window_show();
/*004a9810*/ extern int window_destroy();
/*004a9c90*/ extern int window_create(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0076e7e4*/ extern undefined4 g_app_76e7e4;
/* app_initialise_on_bootup */

/* "'Calling AppInitialiseOnBootup' ... boot sequence" */

undefined4 __stdcall app_initialise_on_bootup(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  window_create(param_1,param_2,param_3,param_4);
  app_parse_command_line();
  iVar1 = app_shutdown_sequence_a(0,0);
  if (iVar1 != 0) {
    iVar1 = game_options_reset();
    if (iVar1 != 0) {
      iVar1 = app_initialise_rw_and_camera();
      if (iVar1 != 0) {
        stub_return_zero();
        window_show(param_4);
        log_print("Calling AppInitialiseOnBootup\n");
        iVar1 = app_boot_init();
        if (iVar1 != 0) {
          app_main_loop();
        }
        log_print("Calling AppDestroy\n");
        app_shutdown_resources();
        if (g_app_76e7e4 != 0) {
          RwTexDictionaryDestroy_004d5ac0((struct RwTexDictionary *)g_app_76e7e4);
          g_app_76e7e4 = (undefined4)(RwTexDictionary *)0x0;
        }
        log_print("Calling SoftwareTidyUpBeforeExiting\n");
        rw_engine_shutdown();
        log_print("Calling HardwareExitApplication\n");
        cursor_show_if_windowed();
        uVar2 = window_destroy();
        return uVar2;
      }
    }
  }
  return 0xffffffff;
}


