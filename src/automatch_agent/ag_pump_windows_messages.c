#include "mfl_tier2.h"
typedef int BOOL;                       /* Windows BOOL (4 bytes) */
typedef char mfl_size_BOOL[sizeof(BOOL) == 4 ? 1 : -1];
typedef struct tagMSG MSG;              /* Windows message record, only used by pointer here */
typedef struct tagMSG *LPMSG;
extern __declspec(dllimport) int __stdcall DispatchMessageA(int);
extern __declspec(dllimport) int __stdcall PeekMessageA(int, int, int, int, int);
extern __declspec(dllimport) int __stdcall TranslateMessage(int);
extern __declspec(dllimport) int __stdcall WaitMessage(void);
/*00771a18*/ extern undefined4 g_pump_windows_771a18;
/*00771a1c*/ extern undefined4 g_pump_windows_771a1c;
/*007e7720*/ extern undefined g_pump_windows_7e7720;
/*007e7724*/ extern undefined4 g_pump_windows_7e7724;


/* pump_windows_messages */

/* PeekMessage loop; returns true on WM_QUIT (0x12) */

int pump_windows_messages(void)

{
  BOOL BVar1;
  
  BVar1 = PeekMessageA((LPMSG)&g_pump_windows_7e7720,(HWND)0x0,0,0,1);
  if (BVar1 != 0) {
    if (g_pump_windows_7e7724 == 0x12) {
      return 1;
    }
    TranslateMessage((MSG *)&g_pump_windows_7e7720);
    DispatchMessageA((MSG *)&g_pump_windows_7e7720);
  }
  if (g_pump_windows_771a1c == 0) {
    WaitMessage();
  }
  return g_pump_windows_771a18 != 0;
}
