// cluster-entries: video_dialog_init_004a8bd0
/* Installed cluster video_dialog_init_004a8bd0 (tools/cluster_install.py from output/cluster_pilot_zy/video_dialog_init_004a8bd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007712f8*/ extern struct VideoCfg g_video_cfg;
/*0076e874*/ extern int g_language_offset;
/*00771500*/ extern int g_app_771500;
/*007712ec*/ extern char *g_video_7712ec;
/*007714fc*/ extern int g_video_subsystem_index;
/*007712f0*/ extern int g_video_7712f0;
/*00771508*/ extern int g_video_dialog_771508[];
/*004a93b0*/ extern void dialog_item_set_text(void *hwnd, int text);
/*004a89d0*/ extern void video_dialog_fill_modes(void *combo);
extern __declspec(dllimport) void *__stdcall GetDlgItem(void *hwnd, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(void *hwnd, unsigned int msg, unsigned int wparam, int lparam);
extern __declspec(dllimport) void *__stdcall SetFocus(void *hwnd);

/* Fills in the video options dialog `dialog`: texts from the language table, the 4 option check boxes (BM_SETCHECK 0xf1) from g_video_cfg, the sub system combo box (one entry per sub system, selection = current) and the video mode combo box. */
void video_dialog_init_004a8bd0_n(void *dialog)
{
  void *subsystem_combo;
  void *mode_combo;
  unsigned int index;

  dialog_item_set_text(dialog, g_language_offset + 0x7e4);
  dialog_item_set_text(GetDlgItem(dialog, 1), g_language_offset + 2000);
  dialog_item_set_text(GetDlgItem(dialog, 2), g_language_offset + 0x7da);
  dialog_item_set_text(GetDlgItem(dialog, 0x41a), g_language_offset + 0x8c0);
  dialog_item_set_text(GetDlgItem(dialog, 0x417), g_language_offset + 0x8ca);
  dialog_item_set_text(GetDlgItem(dialog, 0x418), g_language_offset + 0x8d4);
  dialog_item_set_text(GetDlgItem(dialog, 0x419), g_language_offset + 0x8de);
  dialog_item_set_text(GetDlgItem(dialog, 0x41b), g_language_offset + 0x8e8);
  subsystem_combo = GetDlgItem(dialog, 1000);
  mode_combo = GetDlgItem(dialog, 0x3e9);
  SendMessageA(GetDlgItem(dialog, 0x417), 0xf1, g_video_cfg.no_dynamic_shadows == 0, 0);
  SendMessageA(GetDlgItem(dialog, 0x418), 0xf1, g_video_cfg.no_multitexture == 0, 0);
  SendMessageA(GetDlgItem(dialog, 0x419), 0xf1, g_video_cfg.trilinear != 0, 0);
  SendMessageA(GetDlgItem(dialog, 0x41b), 0xf1, g_video_cfg.no_env_effects == 0, 0);
  for (index = 0; (int)index < g_app_771500; index++) {
    SendMessageA(subsystem_combo, 0x143, 0, (int)(g_video_7712ec + index * 0x50));
    SendMessageA(subsystem_combo, 0x151, index, index);
  }
  SendMessageA(subsystem_combo, 0x14e, g_video_subsystem_index, 0);
  video_dialog_fill_modes(mode_combo);
  SendMessageA(mode_combo, 0x14e, g_video_dialog_771508[g_video_7712f0], 0);
  SetFocus(mode_combo);
}

/* entry thunk at the original address of video_dialog_init_004a8bd0: register inputs (eax) -> natural C video_dialog_init_004a8bd0_n; ret 0 */
__declspec(naked) void video_dialog_init_004a8bd0(void)
{
    __asm {
        push eax
        call video_dialog_init_004a8bd0_n
        add esp, 4
        ret
    }
}
