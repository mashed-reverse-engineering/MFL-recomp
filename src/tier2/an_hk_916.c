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
/*0076e7e4*/ extern char *g_app_76e7e4;
/* app_initialise_on_bootup */

/* "'Calling AppInitialiseOnBootup' ... boot sequence" */

/* (inferred) WinMain-style arguments (hInstance, hPrevInstance, command line, nCmdShow); runs the boot sequence step by step, aborts with 0xffffffff when a step fails, otherwise runs the main loop and the shutdown sequence */
undefined4 __stdcall app_initialise_on_bootup(undefined4 hinstance, undefined4 prev_instance, undefined4 cmd_line, undefined4 show_cmd)

{
  int step_ok;
  undefined4 exit_code;
  
  window_create(hinstance,prev_instance,cmd_line,show_cmd);
  app_parse_command_line();
  step_ok = app_shutdown_sequence_a(0,0);
  if (step_ok != 0) {
    step_ok = game_options_reset();
    if (step_ok != 0) {
      step_ok = app_initialise_rw_and_camera();
      if (step_ok != 0) {
        stub_return_zero();
        window_show(show_cmd);
        log_print("Calling AppInitialiseOnBootup\n");
        step_ok = app_boot_init();
        if (step_ok != 0) {
          app_main_loop();
        }
        log_print("Calling AppDestroy\n");
        app_shutdown_resources();
        if (g_app_76e7e4 != 0) {
          RwTexDictionaryDestroy_004d5ac0((struct RwTexDictionary *)g_app_76e7e4);
          g_app_76e7e4 = (char *)(RwTexDictionary *)0x0;
        }
        log_print("Calling SoftwareTidyUpBeforeExiting\n");
        rw_engine_shutdown();
        log_print("Calling HardwareExitApplication\n");
        cursor_show_if_windowed();
        exit_code = window_destroy();
        return exit_code;
      }
    }
  }
  return 0xffffffff;
}


