// cluster-entries: video_config_dialog_cb_4a8e60
/* Installed cluster video_config_dialog_cb_4a8e60 (tools/cluster_install.py from output/cluster_pilot_md/video_config_dialog_cb_4a8e60.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EndDialog(void *dialog, int result);
extern __declspec(dllimport) void *__stdcall GetDlgItem(void *dialog, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(void *window, unsigned int msg, unsigned int wparam, int lparam);
/*00771494*/ extern int g_video_cfg_no_multitexture;
/*00771498*/ extern int g_video_cfg_no_dynamic_shadows;
/*0077149c*/ extern int g_video_cfg_trilinear;
/*007714a0*/ extern int g_video_cfg_no_env_effects;
/* video_config_dialog_cb_4a8e60 */

#define WM_INITDIALOG 0x110
#define WM_COMMAND 0x111
#define BM_GETCHECK 0xf0
#define NOTIFY_SELCHANGE 1                /* CBN_SELCHANGE */
#define IDOK 1
#define IDCANCEL 2
#define ID_SUBSYSTEM_COMBO 1000           /* combo box of the video subsystems */
#define ID_MODE_COMBO 0x3e9               /* combo box of the display modes */
#define ID_NO_DYNAMIC_SHADOWS 0x417       /* check boxes of the video options */
#define ID_NO_MULTITEXTURE 0x418
#define ID_TRILINEAR 0x419
#define ID_NO_ENV_EFFECTS 0x41b

static void video_dialog_init_004a8bd0_r(void * p0, unsigned int p1, unsigned int p2, int p3);
extern void video_dialog_init_004a8bd0(void);
static void video_dialog_mode_changed_r(void * p0);
extern void video_dialog_mode_changed(void);
static void video_dialog_subsystem_changed_r(void * p0);
extern void video_dialog_subsystem_changed(void);
/* "function pointer pushed/stored by video_config_dialog (PUSH 0x4a8e60): the dialog procedure of the video configuration dialog" */
int video_config_dialog_cb_4a8e60_n(void *dialog, unsigned int msg, unsigned int wparam, int lparam)
{
  unsigned int id;
  unsigned int notify;

  if (msg == WM_INITDIALOG) {
    video_dialog_init_004a8bd0_r(dialog, WM_INITDIALOG, wparam, lparam);
    return 1;
  }
  if (msg != WM_COMMAND) {
    return 0;
  }
  id = wparam & 0xffff;
  notify = wparam >> 16;
  switch (id) {
  case IDOK:
    if (notify == 0) {
      video_dialog_mode_changed_r(dialog);
      EndDialog(dialog, 1);
    }
    return 1;
  case IDCANCEL:
    if (notify == 0) {
      EndDialog(dialog, 0);
    }
    return 1;
  case ID_SUBSYSTEM_COMBO:
    video_dialog_subsystem_changed_r(dialog);
    return 1;
  case ID_MODE_COMBO:
    if (notify == NOTIFY_SELCHANGE) {
      video_dialog_mode_changed_r(dialog);
    }
    return 1;
  case 0x3eb:
  case 0x3ec:
  case 0x3ed:
    return 1;
  case ID_NO_DYNAMIC_SHADOWS:
    g_video_cfg_no_dynamic_shadows = SendMessageA(GetDlgItem(dialog, ID_NO_DYNAMIC_SHADOWS), BM_GETCHECK, 0, 0) == 0;
    return 1;
  case ID_NO_MULTITEXTURE:
    g_video_cfg_no_multitexture = SendMessageA(GetDlgItem(dialog, ID_NO_MULTITEXTURE), BM_GETCHECK, 0, 0) == 0;
    return 1;
  case ID_TRILINEAR:
    g_video_cfg_trilinear = SendMessageA(GetDlgItem(dialog, ID_TRILINEAR), BM_GETCHECK, 0, 0);
    return 1;
  case ID_NO_ENV_EFFECTS:
    g_video_cfg_no_env_effects = SendMessageA(GetDlgItem(dialog, ID_NO_ENV_EFFECTS), BM_GETCHECK, 0, 0) == 0;
    return 1;
  default:
    return 0;
  }
}

/* video_dialog_init_004a8bd0_r: C -> original video_dialog_init_004a8bd0 (registers eax,s0,s1,s2) */
__declspec(naked) void video_dialog_init_004a8bd0_r(void * p0, unsigned int p1, unsigned int p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 16]
        call video_dialog_init_004a8bd0
        add esp, 12
        ret
    }
}

/* video_dialog_mode_changed_r: C -> original video_dialog_mode_changed (registers eax) */
__declspec(naked) void video_dialog_mode_changed_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call video_dialog_mode_changed
        ret
    }
}

/* video_dialog_subsystem_changed_r: C -> original video_dialog_subsystem_changed (registers eax) */
__declspec(naked) void video_dialog_subsystem_changed_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call video_dialog_subsystem_changed
        ret
    }
}

/* entry thunk at the original address of video_config_dialog_cb_4a8e60: register inputs (s0,s1,s2,s3) -> natural C video_config_dialog_cb_4a8e60_n; ret 16 */
__declspec(naked) void video_config_dialog_cb_4a8e60(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        call video_config_dialog_cb_4a8e60_n
        add esp, 16
        ret 16
    }
}
