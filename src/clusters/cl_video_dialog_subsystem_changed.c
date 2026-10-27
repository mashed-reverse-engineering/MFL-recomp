// cluster-entries: video_dialog_subsystem_changed
/* Installed cluster video_dialog_subsystem_changed (tools/cluster_install.py from output/cluster_pilot11/video_dialog_subsystem_changed.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetDlgItem(int hDlg, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(int hWnd, int Msg, int wParam, int lParam);
/*004a89d0*/ extern void video_dialog_fill_modes(int hWnd);
/*004d3000*/ extern RwBool RwEngineSetSubSystem(RwInt32 p0);
/*004d3090*/ extern RwInt32 RwEngineGetCurrentVideoMode();
/*007712f0*/ extern undefined4 g_video_7712f0;
/*007714fc*/ extern undefined4 g_video_subsystem_index;
/* video_dialog_subsystem_changed */

/* "reads the subsystem combo (1000) and refreshes the mode list" */

void video_dialog_subsystem_changed_n(int in_eax)

{
  int hWnd;
  int hWnd_00;
  int wParam;
  
  hWnd = GetDlgItem(in_eax,1000);
  hWnd_00 = GetDlgItem(in_eax,0x3e9);
  wParam = SendMessageA(hWnd,0x147,0,0);
  if (wParam != g_video_subsystem_index) {
    g_video_subsystem_index = SendMessageA(hWnd,0x150,wParam,0);
    RwEngineSetSubSystem(g_video_subsystem_index);
    SendMessageA(hWnd_00,0x14b,0,0);
    video_dialog_fill_modes(hWnd_00);
    g_video_7712f0 = RwEngineGetCurrentVideoMode();
    SendMessageA(hWnd_00,0x14e,g_video_7712f0,0);
  }
  return;
}

/* entry thunk at the original address of video_dialog_subsystem_changed: register inputs (eax) -> natural C video_dialog_subsystem_changed_n; ret 0 */
__declspec(naked) void video_dialog_subsystem_changed(void)
{
    __asm {
        push eax
        call video_dialog_subsystem_changed_n
        add esp, 4
        ret
    }
}
