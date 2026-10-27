// cluster-entries: controller_dialog_run_cb_4a7fd0
/* Installed cluster controller_dialog_run_cb_4a7fd0 (tools/cluster_install.py from output/cluster_pilot_md/controller_dialog_run_cb_4a7fd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall EndDialog(void *dialog, int result);
/*004a7f10*/ extern void __cdecl controller_dialog_close(int result);
/*007711e4*/ extern void *g_data_7711e4;
/* controller_dialog_run_cb_4a7fd0 */

#define WM_INITDIALOG 0x110
#define WM_COMMAND 0x111
#define NOTIFY_SELCHANGE 1          /* CBN_SELCHANGE of the device combo boxes */
#define IDOK 1
#define IDCANCEL 2
#define ID_DEVICE_COMBO_A 0x3ea      /* the six device combo boxes and the close buttons of the controller dialog */
#define ID_DEVICE_COMBO_B 0x3eb
#define ID_CLOSE_0 0x3ef
#define ID_CLOSE_1 0x3f0
#define ID_CLOSE_2 0x3f1
#define ID_CLOSE_3 0x3f2
#define ID_DEVICE_COMBO_C 0x3f3
#define ID_DEVICE_COMBO_D 0x3f4

static void controller_dialog_set_item_texts_r(void * p0, unsigned int p1, unsigned int p2, int p3);
extern void controller_dialog_set_item_texts(void);
static void dialog_controller_device_changed_r(unsigned int p0, void * p1, int p2);
extern void dialog_controller_device_changed(void);
/* "function pointer pushed/stored by controller_dialog_run (PUSH 0x4a7fd0): the dialog procedure of the controller dialog" */
int controller_dialog_run_cb_4a7fd0_n(void *dialog, unsigned int msg, unsigned int wparam, int lparam)
{
  unsigned int id;
  unsigned int notify;

  if (msg == WM_INITDIALOG) {
    g_data_7711e4 = dialog;
    controller_dialog_set_item_texts_r(dialog, WM_INITDIALOG, wparam, lparam);
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
      EndDialog(dialog, 1);
    }
    return 1;
  case IDCANCEL:
    if (notify == 0) {
      EndDialog(dialog, 0);
    }
    return 1;
  case ID_DEVICE_COMBO_A:
    if (notify == NOTIFY_SELCHANGE) {
      dialog_controller_device_changed_r(id, dialog, 2);
    }
    return 1;
  case ID_DEVICE_COMBO_B:
    if (notify == NOTIFY_SELCHANGE) {
      dialog_controller_device_changed_r(id, dialog, 3);
    }
    return 1;
  case ID_CLOSE_0:
    controller_dialog_close(0);
    return 1;
  case ID_CLOSE_1:
    controller_dialog_close(1);
    return 1;
  case ID_CLOSE_2:
    controller_dialog_close(2);
    return 1;
  case ID_CLOSE_3:
    controller_dialog_close(3);
    return 1;
  case ID_DEVICE_COMBO_C:
    if (notify == NOTIFY_SELCHANGE) {
      dialog_controller_device_changed_r(id, dialog, 0);
    }
    return 1;
  case ID_DEVICE_COMBO_D:
    if (notify == NOTIFY_SELCHANGE) {
      dialog_controller_device_changed_r(id, dialog, 1);
    }
    return 1;
  default:
    return 0;
  }
}

/* controller_dialog_set_item_texts_r: C -> original controller_dialog_set_item_texts (registers eax,s0,s1,s2) */
__declspec(naked) void controller_dialog_set_item_texts_r(void * p0, unsigned int p1, unsigned int p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 16]
        call controller_dialog_set_item_texts
        add esp, 12
        ret
    }
}

/* dialog_controller_device_changed_r: C -> original dialog_controller_device_changed (registers eax,ecx,edx) */
__declspec(naked) void dialog_controller_device_changed_r(unsigned int p0, void * p1, int p2)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        mov edx, dword ptr [esp + 12]
        call dialog_controller_device_changed
        ret
    }
}

/* entry thunk at the original address of controller_dialog_run_cb_4a7fd0: register inputs (s0,s1,s2,s3) -> natural C controller_dialog_run_cb_4a7fd0_n; ret 16 */
__declspec(naked) void controller_dialog_run_cb_4a7fd0(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        call controller_dialog_run_cb_4a7fd0_n
        add esp, 16
        ret 16
    }
}
