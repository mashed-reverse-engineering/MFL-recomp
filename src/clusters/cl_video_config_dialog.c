// cluster-entries: video_config_dialog
/* Installed cluster video_config_dialog (tools/cluster_install.py from output/cluster_pilot_uc3/video_config_dialog.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DialogBoxParamA(int, int, int, int, int);
extern __declspec(dllimport) int __stdcall SetWindowLongA(int, int, int);
extern __declspec(dllimport) int __stdcall SetWindowPos(int, int, int, int, int, int, int);
extern char LAB_004a8e60[];
/*004a5550*/ extern int log_printf();
/*004a85c0*/ extern int video_cfg_load();
/*004a8620*/ extern int video_cfg_save();
/*004a8870*/ extern int video_dialog_init();
/*004a8ab0*/ extern int video_mode_find_first_640x480_plus();
/*004a8b10*/ extern int video_cfg_apply_selection();
/*004d3000*/ extern RwBool RwEngineSetSubSystem(RwInt32 p0);
/*004d3060*/ extern struct RwVideoMode * RwEngineGetVideoModeInfo(struct RwVideoMode *p0, RwInt32 p1);
/*004d30c0*/ extern RwBool RwEngineSetVideoMode(RwInt32 p0);
/*005de630*/ extern undefined g_gfx_5de630;
/*005fb098*/ extern undefined4 g_course_5fb098;
/*0062c5c8*/ extern undefined4 g_app_62c5c8;
/*0062c5cc*/ extern undefined4 g_app_62c5cc;
/*0064d88c*/ extern undefined4 g_game_64d88c;
/*0064d890*/ extern undefined4 g_load_64d890;
/*0064d894*/ extern undefined4 g_course_64d894;
/*0064d89c*/ extern undefined4 g_round_64d89c;
/*0064d8a0*/ extern undefined4 g_race_64d8a0;
/*007712e8*/ extern undefined4 g_video_mode_counts;
/*007712ec*/ extern undefined4 g_video_7712ec;
/*007712f0*/ extern undefined4 g_video_7712f0;
/*007712f4*/ extern undefined4 g_app_7712f4;
/*007712f8*/ extern struct VideoCfg g_video_cfg;
/*007714f8*/ extern undefined4 g_video_mode_lists;
/*007714fc*/ extern undefined4 g_video_subsystem_index;
/*00771500*/ extern int g_app_771500;
/*00771504*/ extern undefined4 g_app_771504;
/*007e7700*/ extern undefined4 g_app_7e7700;
/*007e7704*/ extern undefined4 g_app_7e7704;


#define VIDEO_MODE_RECORD_SIZE (0x18) /* stride of one mode record in g_video_mode_lists[subsystem] (inferred) */
#define VIDEO_SUBSYSTEM_NAME_STRIDE (0x50) /* stride of subsystem name strings at g_video_7712ec (inferred) */
#define VIDEO_DIALOG_RESOURCE_ID (0x65) /* DialogBoxParamA template id (inferred) */
/* video_config_dialog */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "'Number of subsystems: %d': video_cfg_load, dialog setup" */

/* Select the video subsystem/mode: log all subsystems and modes, load saved config (or show the settings dialog when show_dialog != 0), apply it to RenderWare, set window style for fullscreen modes, and copy quality options into game globals; returns 0 if the dialog was cancelled, else 1 (inferred). */
static int video_mode_name_format_004a8670_r(int p0, undefined4 p1);
extern void video_mode_name_format_004a8670(void);
undefined4 __cdecl video_config_dialog(int show_dialog)

{
  int dialog_result;
  int zero_words_left;
  int subsystem;
  int mode;
  undefined4 *zero_ptr;
  char loop_offsets[32];
  struct RwVideoMode mode_info;
  char mode_name_buf[64];
  
  video_dialog_init();
  log_printf("Number of subsystems: %d\n",g_app_771500);
  subsystem = 0;
  if (0 < g_app_771500) {
    (*(int *)(loop_offsets + 4)) = 0;
    do {
      log_printf(&g_gfx_5de630,(*(int *)(loop_offsets + 4)) + g_video_7712ec);
      mode = 0;
      if (0 < *(int *)(g_video_mode_counts + subsystem * 4)) {
        (*(int *)loop_offsets) = 0;
        do {
          (*(undefined1 *)mode_name_buf) = 0;
          zero_ptr = ((undefined4 *)(mode_name_buf + 1));
          for (zero_words_left = 0xf; zero_words_left != 0; zero_words_left = zero_words_left + -1) {
            *zero_ptr = 0;
            zero_ptr = zero_ptr + 1;
          }
          *(undefined2 *)zero_ptr = 0;
          *(undefined1 *)((int)zero_ptr + 2) = 0;
          video_mode_name_format_004a8670_r
                    (*(int *)(g_video_mode_lists + subsystem * 4) + (*(int *)loop_offsets),(undefined4)mode_name_buf);
          log_printf("%d\t%s\n",mode,mode_name_buf);
          (*(int *)loop_offsets) = (*(int *)loop_offsets) + 0x18;
          mode = mode + 1;
        } while (mode < *(int *)(g_video_mode_counts + subsystem * 4));
      }
      subsystem = subsystem + 1;
      (*(int *)(loop_offsets + 4)) = (*(int *)(loop_offsets + 4)) + 0x50;
    } while (subsystem < g_app_771500);
  }
  if (show_dialog == 0) {
    subsystem = video_cfg_load();
    if (subsystem != 0) {
      g_video_subsystem_index = g_video_cfg.subsystem_index;
      g_video_7712f0 = g_video_cfg.mode_index;
      goto LAB_004a9200;
    }
    g_video_subsystem_index = 0;
    g_video_7712f0 = video_mode_find_first_640x480_plus();
  }
  else {
    subsystem = video_cfg_load();
    if (subsystem == 0) {
      g_video_cfg.no_dynamic_shadows = 0;
      g_video_cfg.trilinear = 1;
      g_video_cfg.no_env_effects = 0;
      g_video_cfg.no_multitexture = 0;
    }
    else {
      g_video_subsystem_index = g_video_cfg.subsystem_index;
      g_video_7712f0 = g_video_cfg.mode_index;
    }
    dialog_result = DialogBoxParamA(g_app_7e7700,VIDEO_DIALOG_RESOURCE_ID,g_app_7e7704,(int)LAB_004a8e60,0);
    if (dialog_result == 0) {
      return 0;
    }
  }
  video_cfg_apply_selection();
  video_cfg_save();
LAB_004a9200:
  RwEngineSetSubSystem(g_video_subsystem_index);
  RwEngineSetVideoMode(g_video_7712f0);
  log_printf("Current subsystem: %s\n",g_video_subsystem_index * VIDEO_SUBSYSTEM_NAME_STRIDE + g_video_7712ec);
  log_printf("Current mode: %d\n",g_video_7712f0);
  RwEngineGetVideoModeInfo(&mode_info,g_video_7712f0);
  if ((mode_info.flags & 1) != 0) {
    g_app_62c5c8 = mode_info.width;
    g_app_7712f4 = 1;
    g_app_62c5cc = mode_info.height;
    g_app_771504 = mode_info.depth;
    SetWindowLongA(g_app_7e7704,-0x10,-0x80000000);
    SetWindowPos(g_app_7e7704,0,0,0,0,0,0x27);
  }
  g_course_64d894 = g_video_cfg.no_multitexture;
  g_load_64d890 = g_video_cfg.no_multitexture;
  g_game_64d88c = g_video_cfg.no_dynamic_shadows;
  g_race_64d8a0 = g_video_cfg.no_env_effects;
  g_round_64d89c = g_video_cfg.no_env_effects;
  g_course_5fb098 = g_video_cfg.trilinear;
  return 1;
}

/* video_mode_name_format_004a8670_r: C -> original video_mode_name_format_004a8670 (registers ebx,s0) */
__declspec(naked) int video_mode_name_format_004a8670_r(int p0, undefined4 p1)
{
    __asm {
        push ebx
        push dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 12]
        call video_mode_name_format_004a8670
        add esp, 4
        pop ebx
        ret
    }
}
