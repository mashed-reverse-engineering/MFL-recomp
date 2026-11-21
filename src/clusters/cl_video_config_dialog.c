// cluster-entries: video_config_dialog
/* Installed cluster video_config_dialog (tools/cluster_install.py from output/cluster_pilot_uc3/video_config_dialog.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#include "mfl_fix.h"
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
#if MFL_FIX_PREFS
extern __declspec(dllimport) int __stdcall AdjustWindowRect(int, int, int);
extern __declspec(dllimport) int __stdcall ShowCursor(int);
#define GWL_STYLE (-0x10)
#define STYLE_POPUP (-0x80000000)      /* WS_POPUP, as the exclusive branch below */
#define STYLE_FRAMED (0xcf0000)        /* WS_OVERLAPPEDWINDOW, window_create's style */
#define SWP_NOZORDER (0x4)
#define SWP_FRAMECHANGED (0x20)
/*004d3030*/ extern RwInt32 RwEngineGetNumVideoModes(void);
#define MODE_EXCLUSIVE (1)      /* rwVIDEOMODEEXCLUSIVE: a full-screen mode; RenderWare mode 0 = the desktop mode with a windowed device */
#define MODE_LIST_MAX (32)
int mfl_video_mode_list[MODE_LIST_MAX];      /* resolution values (W << 16) | H; [0] = -1 = Desktop (a global, so a test run can read it by the map) */
static int mode_list_n;

static int mode_value(const struct RwVideoMode *mode) { return (mode->width << 16) | mode->height; }

/* the full-screen 32 bit mode of that size, else -1 */
static int mode_find(int value)
{
  struct RwVideoMode mode;
  int i;
  int n = RwEngineGetNumVideoModes();

  for (i = 0; i < n; i++) {
    RwEngineGetVideoModeInfo(&mode, i);
    if ((mode.flags & MODE_EXCLUSIVE) != 0 && mode.depth == 32 && mode_value(&mode) == value) return i;
  }
  return -1;
}

/* mfl_video_modes_api (the Preferences page's Resolution row): count / name / current / choose over mfl_video_mode_list */
static int modes_count(void)
{
  struct RwVideoMode mode;
  int i;
  int k;
  int n;
  int value;

  if (mode_list_n == 0) {
    mfl_video_mode_list[mode_list_n++] = -1;
    n = RwEngineGetNumVideoModes();
    for (i = 0; i < n && mode_list_n < MODE_LIST_MAX; i++) {
      RwEngineGetVideoModeInfo(&mode, i);
      if ((mode.flags & MODE_EXCLUSIVE) == 0 || mode.depth != 32 || mode.width < 640 || mode.height < 480 || mode.width * 3 < mode.height * 4) continue;
      value = mode_value(&mode);
      for (k = 1; k < mode_list_n && mfl_video_mode_list[k] != value; k++) ;
      if (k == mode_list_n) mfl_video_mode_list[mode_list_n++] = value;
    }
  }
  return mode_list_n;
}

static int modes_value(int i) { return i > 0 && i < modes_count() ? mfl_video_mode_list[i] : -1; }

/* the entry of resolution=; 0 (Desktop) for auto or a size the display does not list */
static int modes_current(void)
{
  int i;

  for (i = modes_count() - 1; i > 0 && mfl_video_mode_list[i] != mfl_pref(MFL_PREF_RESOLUTION); i--) ;
  return i;
}

/* applies at the next start: mfl_settings_save writes mfl_prefs[MFL_PREF_RESOLUTION] */
static void modes_choose(int i) { mfl_pref(0); mfl_prefs[MFL_PREF_RESOLUTION] = modes_value(i); }

/* "Desktop", "1280 x 720" */
static const char *modes_name(int i)
{
  static char label[16];
  int value = modes_value(i);
  int n = 0;
  int part;
  int v;
  int d;

  if (value < 0) return "Desktop";
  for (part = 0; part < 2; part++) {
    v = part == 0 ? value >> 16 : value & 0xffff;
    for (d = 10000; d > 1 && v < d; d /= 10) ;
    for (; d != 0; d /= 10) label[n++] = (char)('0' + v / d % 10);
    if (part == 0) { label[n++] = ' '; label[n++] = 'x'; label[n++] = ' '; }
  }
  label[n] = '\0';
  return label;
}

static const struct mfl_video_modes k_modes = { modes_count, modes_name, modes_current, modes_choose };

/* the game window: framed with a client of width x height (made smaller so the frame fits the desktop: a client larger than the screen would be
   resized later, which resets the device), centred; or frameless over the whole desktop.  g_app_62c5c8 / cc = the back buffer size. */
static void window_place(int borderless, int width, int height, const struct RwVideoMode *desktop)
{
  int rect[4];
  int x;
  int y;

  if (borderless) {
    width = desktop->width;
    height = desktop->height;
    SetWindowLongA(g_app_7e7704, GWL_STYLE, STYLE_POPUP);
    SetWindowPos(g_app_7e7704, 0, 0, 0, width, height, SWP_NOZORDER | SWP_FRAMECHANGED);
  }
  else {
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = width;
    rect[3] = height;
    AdjustWindowRect((int)rect, STYLE_FRAMED, 0);
    x = rect[2] - rect[0] - width;      /* frame width / height */
    y = rect[3] - rect[1] - height;
    if (width + x > desktop->width) width = desktop->width - x;
    if (height + y > desktop->height) height = desktop->height - y;
    SetWindowLongA(g_app_7e7704, GWL_STYLE, STYLE_FRAMED);
    SetWindowPos(g_app_7e7704, 0, (desktop->width - width - x) / 2, (desktop->height - height - y) / 2, width + x, height + y, SWP_NOZORDER | SWP_FRAMECHANGED);
  }
  g_app_62c5c8 = width;
  g_app_62c5cc = height;
}

/* display_mode + resolution (mflng_settings.cfg), before RwEngineSetVideoMode.  resolution auto = the desktop size.
   fullscreen: the full-screen 32 bit mode of that size; not listed (other monitor) -> the one of the desktop size, else the mode videocfg.bin /
   the dialog chose (the original; without videocfg.bin that is the first mode of at least 640 x 480, often 16 bit).
   windowed: RenderWare mode 0 (windowed device, back buffer = client area), framed window of that size.  borderless: mode 0, frameless window
   over the whole (primary) desktop, cursor hidden; resolution is ignored.  The video cfg keeps the dialog's mode.
   ponytail: borderless renders at the desktop size; a smaller back buffer stretched to the screen needs the present parameters in mflrw.dll. */
static void display_mode_apply(void)
{
  int display_mode = mfl_pref(MFL_PREF_DISPLAY_MODE);
  int size = mfl_pref(MFL_PREF_RESOLUTION);
  struct RwVideoMode desktop;
  int mode;

  modes_count();      /* the Resolution row's list, built while no mode is set yet */
  mfl_video_modes_api = &k_modes;
  RwEngineGetVideoModeInfo(&desktop, 0);
  if (size < 0 || display_mode == MFL_DISPLAY_BORDERLESS) size = mode_value(&desktop);
  if (display_mode == MFL_DISPLAY_FULLSCREEN || (desktop.flags & MODE_EXCLUSIVE) != 0) {      /* no windowed entry 0 (desktop format): full screen */
    mode = mode_find(size);
    if (mode < 0) mode = mode_find(mode_value(&desktop));      /* not listed (other monitor): the desktop size */
    if (mode >= 0) g_video_7712f0 = mode;
    return;
  }
  g_video_7712f0 = 0;
  window_place(display_mode == MFL_DISPLAY_BORDERLESS, size >> 16, size & 0xffff, &desktop);
  if (display_mode == MFL_DISPLAY_BORDERLESS) ShowCursor(0);
}

/* rw_engine_init: RwEngineStart failed (the device refused the mode): once, a 640 x 480 window on the desktop mode, logged */
int mfl_video_start_fallback(void)
{
  static int tried;
  struct RwVideoMode desktop;

  RwEngineGetVideoModeInfo(&desktop, 0);
  if (tried || (desktop.flags & MODE_EXCLUSIVE) != 0) return 0;
  tried = 1;
  log_printf("RwEngineStart failed in video mode %d: trying a 640 x 480 window\n", g_video_7712f0);
  g_video_7712f0 = 0;
  g_app_7712f4 = 0;      /* not full screen */
  RwEngineSetVideoMode(0);
  window_place(0, 640, 480, &desktop);
  return 1;
}
#endif
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
#if MFL_FIX_PREFS
  display_mode_apply();
#endif
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
