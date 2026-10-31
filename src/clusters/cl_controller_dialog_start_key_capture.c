// cluster-entries: controller_dialog_start_key_capture
/* Installed cluster controller_dialog_start_key_capture (tools/cluster_install.py from output/cluster_pilot_a11/controller_dialog_start_key_capture.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/*007711a0*/ extern int g_controller_dialog_7711a0;
/*0077124c*/ extern int g_controller_dialog_77124c;
/*00771250*/ extern int g_controller_dialog_771250;
/*004a7310*/ extern void LAB_004a7310(void);
extern __declspec(dllimport) unsigned int __stdcall SetTimer(void *hwnd, unsigned int id, unsigned int elapse_ms, void *timer_proc);
extern __declspec(dllimport) void *__stdcall GetDlgItem(void *hwnd, int id);
extern __declspec(dllimport) void *__stdcall SetFocus(void *hwnd);

/* Starts waiting for a key for control `control` of the controller dialog `hwnd` (key slot `key`): remembers both, starts the 20 ms poll timer 20000 and focuses the control.
   Does nothing for a gamepad (device 1) when the key slot is 9..12. */
void controller_dialog_start_key_capture_n(int key, int control, void *hwnd)
{
  if (g_controller_cfg[g_controller_dialog_7711a0].device != 1 || key < 9 || key > 0xc) {
    g_controller_dialog_771250 = key;
    g_controller_dialog_77124c = control;
    SetTimer(hwnd, 20000, 0x14, (void *)LAB_004a7310);
    SetFocus(GetDlgItem(hwnd, control));
  }
}

/* entry thunk at the original address of controller_dialog_start_key_capture: register inputs (eax,esi,edi) -> natural C controller_dialog_start_key_capture_n; ret 0 */
__declspec(naked) void controller_dialog_start_key_capture(void)
{
    __asm {
        push edi
        push esi
        push eax
        call controller_dialog_start_key_capture_n
        add esp, 12
        ret
    }
}
