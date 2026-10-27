// cluster-entries: video_dialog_mode_changed
/* Installed cluster video_dialog_mode_changed (tools/cluster_install.py from output/cluster_pilot11/video_dialog_mode_changed.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetDlgItem(int hDlg, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(int hWnd, int Msg, int wParam, int lParam);
/*007712f0*/ extern undefined4 g_video_7712f0;
/* video_dialog_mode_changed */

/* "reads the selected mode of combo 0x3E9 into g_video_7712f0" */

void video_dialog_mode_changed_n(int in_eax)

{
  int hWnd;
  int wParam;
  
  hWnd = GetDlgItem(in_eax,0x3e9);
  wParam = SendMessageA(hWnd,0x147,0,0);
  g_video_7712f0 = SendMessageA(hWnd,0x150,wParam,0);
  return;
}

/* entry thunk at the original address of video_dialog_mode_changed: register inputs (eax) -> natural C video_dialog_mode_changed_n; ret 0 */
__declspec(naked) void video_dialog_mode_changed(void)
{
    __asm {
        push eax
        call video_dialog_mode_changed_n
        add esp, 4
        ret
    }
}
