// cluster-entries: window_create_cb_4a9830
/* Installed cluster window_create_cb_4a9830 (tools/cluster_install.py from output/cluster_pilot_kg/window_create_cb_4a9830.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetMenu(int hwnd);
extern __declspec(dllimport) int __stdcall CheckMenuItem(int hmenu, unsigned int id, unsigned int flags);
extern __declspec(dllimport) int __stdcall SetCapture(int hwnd);
extern __declspec(dllimport) int __stdcall ReleaseCapture(void);
extern __declspec(dllimport) int __stdcall DefWindowProcA(int hwnd, unsigned int msg, unsigned int wparam, int lparam);
/*004a5230*/ extern void keyboard_acquire_release(int acquire);
/*004a55c0*/ extern void log_file_close(void);
/*004a55e0*/ extern void log_open_write(void);
/*00771a08*/ extern float g_data_771a08;
/*00771a0c*/ extern float g_data_771a0c;
/*00771a10*/ extern float g_video_get_771a10;
/*00771a14*/ extern float g_video_get_771a14;
/*00771a18*/ extern int g_pump_windows_771a18;
/*00771a1c*/ extern int g_pump_windows_771a1c;
/*00771a24*/ extern int g_video_get_771a24;
/*00771a28*/ extern int g_video_get_771a28;
/*00771a2c*/ extern int g_data_771a2c;
/*00771a30*/ extern int g_data_771a30;
/*00771a34*/ extern int g_data_771a34;
/*00771a38*/ extern int g_data_771a38;
/*00771a3c*/ extern int g_data_771a3c;
/*00771a40*/ extern int g_data_771a40;
/*00771a44*/ extern int g_data_771a44;
/*008d8b40*/ extern int player_points;
/* window_create_cb_4a9830 */

#define WM_CREATE 0x01
#define WM_DESTROY 0x02
#define WM_ACTIVATE 0x06
#define WM_KEYDOWN 0x100
#define WM_COMMAND 0x111
#define WM_MOUSEMOVE 0x200
#define WM_LBUTTONDOWN 0x201
#define WM_LBUTTONUP 0x202
#define WM_RBUTTONDOWN 0x204
#define WM_RBUTTONUP 0x205
#define MF_CHECKED 0x08

/* debug menu command ids of the game window (WM_COMMAND, low word of wparam) */
#define MENU_TOGGLE_19A 0x19a
#define MENU_TOGGLE_19B 0x19b
#define MENU_TOGGLE_19C 0x19c
#define MENU_TOGGLE_19D 0x19d
#define MENU_WEAPON_MACHINEGUN 0x19f
#define MENU_WEAPON_DEPTHCHARGE 0x1a0
#define MENU_WEAPON_MISSILE 0x1a1
#define MENU_WEAPON_MINE 0x1a2
#define MENU_WEAPON_FLAMETHROWER 0x1a3
#define MENU_WEAPON_SHOTGUN 0x1a4
#define MENU_WEAPON_FLARE 0x1a5
#define MENU_WEAPON_OIL 0x1a6
#define MENU_WEAPON_MORTAR 0x1a7
#define MENU_TOGGLE_1A8 0x1a8
#define MENU_TOGGLE_1A9 0x1a9
#define MENU_POINTS_UP 0x1ac
#define MENU_POINTS_DOWN 0x1ad

static void players_equip_default_weapons_r(int p0);
extern void players_equip_default_weapons(void);
/* flips a debug flag and updates the check mark of its menu item */
static void toggle_menu_flag(int hwnd, int *flag, unsigned int id)
{
  *flag = (*flag == 0);
  CheckMenuItem(GetMenu(hwnd), id, (*flag != 0) ? MF_CHECKED : 0);
}

/* the game window procedure: mouse position / button state, activation (keyboard acquire), the debug menu and the log file; everything else goes to DefWindowProcA */
int window_create_cb_4a9830_n(int hwnd, unsigned int msg, unsigned int wparam, int lparam)

{
  if (msg == WM_MOUSEMOVE) {
    if (g_data_771a44 == 0) {
      g_data_771a44 = 1;
    }
    else {
      g_video_get_771a10 = (float)(int)(short)lparam - g_data_771a08;
      g_video_get_771a14 = (float)(int)(short)(lparam >> 16) - g_data_771a0c;
    }
    g_data_771a08 = (float)(int)(short)lparam;
    g_data_771a0c = (float)(int)(short)(lparam >> 16);
    return 0;
  }
  if (msg <= WM_MOUSEMOVE) {
    if (msg > WM_ACTIVATE) {
      if (msg != WM_KEYDOWN) {
        if (msg != WM_COMMAND) {
          return DefWindowProcA(hwnd, msg, wparam, lparam);
        }
        switch (wparam & 0xffff) {
        case MENU_TOGGLE_19A:
          toggle_menu_flag(hwnd, &g_data_771a2c, MENU_TOGGLE_19A);
          return 0;
        case MENU_TOGGLE_19B:
          toggle_menu_flag(hwnd, &g_data_771a30, MENU_TOGGLE_19B);
          return 0;
        case MENU_TOGGLE_19C:
          toggle_menu_flag(hwnd, &g_data_771a34, MENU_TOGGLE_19C);
          return 0;
        case MENU_TOGGLE_19D:
          toggle_menu_flag(hwnd, &g_data_771a3c, MENU_TOGGLE_19D);
          return 0;
        case MENU_WEAPON_MACHINEGUN:
          players_equip_default_weapons_r(WEAPON_MACHINEGUN);
          return 0;
        case MENU_WEAPON_DEPTHCHARGE:
          players_equip_default_weapons_r(WEAPON_DEPTHCHARGE);
          return 0;
        case MENU_WEAPON_MISSILE:
          players_equip_default_weapons_r(WEAPON_MISSILE);
          return 0;
        case MENU_WEAPON_MINE:
          players_equip_default_weapons_r(WEAPON_MINE);
          return 0;
        case MENU_WEAPON_FLAMETHROWER:
          players_equip_default_weapons_r(WEAPON_FLAMETHROWER);
          return 0;
        case MENU_WEAPON_SHOTGUN:
          players_equip_default_weapons_r(WEAPON_SHOTGUN);
          return 0;
        case MENU_WEAPON_FLARE:
          players_equip_default_weapons_r(WEAPON_FLARE);
          return 0;
        case MENU_WEAPON_OIL:
          players_equip_default_weapons_r(WEAPON_OIL);
          return 0;
        case MENU_WEAPON_MORTAR:
          players_equip_default_weapons_r(WEAPON_MORTAR);
          return 0;
        case MENU_TOGGLE_1A8:
          toggle_menu_flag(hwnd, &g_data_771a40, MENU_TOGGLE_1A8);
          return 0;
        case MENU_TOGGLE_1A9:
          toggle_menu_flag(hwnd, &g_data_771a38, MENU_TOGGLE_1A9);
          return 0;
        case MENU_POINTS_UP:
          player_points = player_points + 1;
          return 0;
        case MENU_POINTS_DOWN:
          player_points = player_points + -1;
          return 0;
        }
      }
      return 0;
    }
    if (msg == WM_ACTIVATE) {
      g_pump_windows_771a1c = (wparam == 1);
      keyboard_acquire_release(g_pump_windows_771a1c);
      return 0;
    }
    if (msg == WM_CREATE) {
      log_open_write();
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_19A, 0);
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_19B, 0);
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_19C, 0);
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_1A9, 0);
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_19D, 0);
      CheckMenuItem(GetMenu(hwnd), MENU_TOGGLE_1A8, 0);
      return 0;
    }
    if (msg == WM_DESTROY) {
      log_file_close();
      g_pump_windows_771a18 = 1;
      return 0;
    }
  }
  else {
    switch (msg) {
    case WM_LBUTTONDOWN:
      g_video_get_771a24 = 1;
      SetCapture(hwnd);
      return 0;
    case WM_LBUTTONUP:
      ReleaseCapture();
      g_video_get_771a24 = 0;
      return 0;
    case WM_RBUTTONDOWN:
      SetCapture(hwnd);
      g_video_get_771a28 = 1;
      return 0;
    case WM_RBUTTONUP:
      ReleaseCapture();
      g_video_get_771a28 = 0;
      return 0;
    }
  }
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

/* players_equip_default_weapons_r: C -> original players_equip_default_weapons (registers edi) */
__declspec(naked) void players_equip_default_weapons_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call players_equip_default_weapons
        pop edi
        ret
    }
}

/* entry thunk at the original address of window_create_cb_4a9830: register inputs (s0,s1,s2,s3) -> natural C window_create_cb_4a9830_n; ret 16 */
__declspec(naked) void window_create_cb_4a9830(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        call window_create_cb_4a9830_n
        add esp, 16
        ret 16
    }
}
