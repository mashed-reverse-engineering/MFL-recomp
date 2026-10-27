// cluster-entries: controller_dialog_close_cb_4a7c40
/* Installed cluster controller_dialog_close_cb_4a7c40 (tools/cluster_install.py from output/cluster_pilot_md/controller_dialog_close_cb_4a7c40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EndDialog(void *dialog, int result);
extern __declspec(dllimport) int __stdcall KillTimer(void *window, unsigned int id);
extern __declspec(dllimport) int __stdcall ShowWindow(void *window, int command);
/*004a5160*/ extern void dinput_release_all(void);
/*004a51f0*/ extern void __cdecl joypads_init(void *window);
/*004a5500*/ extern void __cdecl dinput_keyboard_init(void *window);
/*004a5520*/ extern void dinput_release_keyboard_004a5520(void);
/*004a77d0*/ extern void __cdecl controller_dialog_init(void *dialog, unsigned int msg, unsigned int wparam, int lparam);
/*004a9380*/ extern void * app_get_app_7e7704(void);
/*007711e4*/ extern void *g_data_7711e4;
/* controller_dialog_close_cb_4a7c40 */

#define WM_TIMER 2
#define WM_INITDIALOG 0x110
#define WM_COMMAND 0x111
#define KEY_CAPTURE_TIMER 20000      /* poll timer of controller_dialog_start_key_capture */
#define SW_HIDE 0
#define SW_SHOW 5
#define ID_KEY_BUTTON_0 0x3f6        /* the key buttons of the controller dialog and the key slot each one rebinds */
#define ID_KEY_BUTTON_1 0x3f9
#define ID_KEY_BUTTON_2 0x3fb
#define ID_KEY_BUTTON_3 0x3fd
#define ID_KEY_BUTTON_6 0x403
#define ID_KEY_BUTTON_8 0x407
#define ID_KEY_BUTTON_9 0x409
#define ID_KEY_BUTTON_10 0x40b
#define ID_KEY_BUTTON_11 0x40d
#define ID_KEY_BUTTON_12 0x40f
#define ID_OK 0x410
#define ID_CANCEL 0x411

/* "function pointer pushed/stored by controller_dialog_close (PUSH 0x4a7c40): the dialog procedure of the key capture dialog: starts the
   key capture of the pressed button, closes with OK / Cancel; on init and on the capture timer it re-initialises DirectInput" */
static void controller_dialog_start_key_capture_r(int p0, int p1, void * p2);
extern void controller_dialog_start_key_capture(void);
int controller_dialog_close_cb_4a7c40_n(void *dialog, unsigned int msg, unsigned int wparam, int lparam)
{
  unsigned int id;
  unsigned int notify;
  int key;

  if (msg == WM_TIMER) {
    KillTimer(dialog, KEY_CAPTURE_TIMER);
    dinput_release_keyboard_004a5520();
    dinput_release_all();
    joypads_init(app_get_app_7e7704());
    dinput_keyboard_init(app_get_app_7e7704());
    ShowWindow(g_data_7711e4, SW_SHOW);
    return 1;
  }
  if (msg == WM_INITDIALOG) {
    ShowWindow(g_data_7711e4, SW_HIDE);
    dinput_release_keyboard_004a5520();
    dinput_release_all();
    joypads_init(dialog);
    dinput_keyboard_init(dialog);
    controller_dialog_init(dialog, WM_INITDIALOG, wparam, lparam);
    return 1;
  }
  if (msg != WM_COMMAND) {
    return 0;
  }
  id = wparam & 0xffff;
  notify = wparam >> 16;
  switch (id) {
  case ID_KEY_BUTTON_0:
    key = 0;
    break;
  case ID_KEY_BUTTON_1:
    key = 1;
    break;
  case ID_KEY_BUTTON_2:
    key = 2;
    break;
  case ID_KEY_BUTTON_3:
    key = 3;
    break;
  case ID_KEY_BUTTON_6:
    key = 6;
    break;
  case ID_KEY_BUTTON_8:
    key = 8;
    break;
  case ID_KEY_BUTTON_9:
    key = 9;
    break;
  case ID_KEY_BUTTON_10:
    key = 10;
    break;
  case ID_KEY_BUTTON_11:
    key = 11;
    break;
  case ID_KEY_BUTTON_12:
    key = 12;
    break;
  case ID_OK:
    if (notify == 0) {
      EndDialog(dialog, 1);
    }
    return 1;
  case ID_CANCEL:
    if (notify == 0) {
      EndDialog(dialog, 0);
    }
    return 1;
  default:
    return 0;
  }
  if (notify == 0) {
    controller_dialog_start_key_capture_r(key, id, dialog);
  }
  return 1;
}

/* controller_dialog_start_key_capture_r: C -> original controller_dialog_start_key_capture (registers eax,esi,edi) */
__declspec(naked) void controller_dialog_start_key_capture_r(int p0, int p1, void * p2)
{
    __asm {
        push esi
        push edi
        mov eax, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        mov edi, dword ptr [esp + 20]
        call controller_dialog_start_key_capture
        pop edi
        pop esi
        ret
    }
}

/* entry thunk at the original address of controller_dialog_close_cb_4a7c40: register inputs (s0,s1,s2,s3) -> natural C controller_dialog_close_cb_4a7c40_n; ret 16 */
__declspec(naked) void controller_dialog_close_cb_4a7c40(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        call controller_dialog_close_cb_4a7c40_n
        add esp, 16
        ret 16
    }
}
