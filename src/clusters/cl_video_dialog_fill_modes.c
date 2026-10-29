// cluster-entries: video_dialog_fill_modes
/* Installed cluster video_dialog_fill_modes (tools/cluster_install.py from output/cluster_pilot_z4/video_dialog_fill_modes.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall SendMessageA(void *window, unsigned int message, unsigned int wparam, int lparam);

/* One entry of a video mode list (24 bytes). */
typedef struct VideoMode {
  int width;
  int height;
  int unknown_8;
  int usable;          /* 0: the mode is skipped */
  int unknown_10;
  int unknown_14;
} VideoMode;

#define CB_ADDSTRING 0x143
#define CB_SETITEMDATA 0x151

/*007712e8*/ extern int *g_video_mode_counts;           /* per subsystem */
/*007714f8*/ extern VideoMode **g_video_mode_lists;      /* per subsystem */
/*007714fc*/ extern int g_video_subsystem_index;
/*00771508*/ extern int g_video_dialog_771508[];         /* combo box index of mode i */

static void video_mode_name_format_004a8670_r(VideoMode * p0, char * p1);
extern void video_mode_name_format_004a8670(void);
/* Fills the mode combo box `combo` with the name of every usable mode (at least 640x480) of the selected subsystem, and remembers the combo box index of each mode. */
void video_dialog_fill_modes(void *combo)
{
  char name[64];
  int i;

  for (i = 0; i < g_video_mode_counts[g_video_subsystem_index]; i++) {
    VideoMode *mode = &g_video_mode_lists[g_video_subsystem_index][i];
    if (mode->usable != 0 && mode->width >= 640 && mode->height >= 480) {
      int index;
      video_mode_name_format_004a8670_r(mode, name);
      index = SendMessageA(combo, CB_ADDSTRING, 0, (int)name);
      SendMessageA(combo, CB_SETITEMDATA, index, i);
      g_video_dialog_771508[i] = index;
    }
  }
}

/* video_mode_name_format_004a8670_r: C -> original video_mode_name_format_004a8670 (registers ebx,s0) */
__declspec(naked) void video_mode_name_format_004a8670_r(VideoMode * p0, char * p1)
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
