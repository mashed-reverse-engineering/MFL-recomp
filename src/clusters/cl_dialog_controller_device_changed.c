// cluster-entries: dialog_controller_device_changed
/* Installed cluster dialog_controller_device_changed (tools/cluster_install.py from output/cluster_pilot11/dialog_controller_device_changed.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetDlgItem(int hDlg, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(int hWnd, int Msg, int wParam, int lParam);
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* dialog_controller_device_changed */

/* "reads the selected device of the combo box into g_controller_cfg[player].device" */

int dialog_controller_device_changed_n(int in_eax, int in_ecx, int in_edx)

{
  int hWnd;
  int wParam;
  
  hWnd = GetDlgItem(in_ecx,in_eax);
  wParam = SendMessageA(hWnd,0x147,0,0);
  g_controller_cfg[in_edx].device = SendMessageA(hWnd,0x150,wParam,0);
  return 1;
}

/* entry thunk at the original address of dialog_controller_device_changed: register inputs (eax,ecx,edx) -> natural C dialog_controller_device_changed_n; ret 0 */
__declspec(naked) void dialog_controller_device_changed(void)
{
    __asm {
        push edx
        push ecx
        push eax
        call dialog_controller_device_changed_n
        add esp, 12
        ret
    }
}
