#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetModuleHandleA(int);
/*004a8420*/ extern int debug_printf(char *fmt, ...);
/*004ab220*/ extern int __stdcall DirectInput8Create(int hinst, int version, void *riid, void *out, int outer);
/*005e1f6c*/ extern undefined g_dinput_create_5e1f6c;
/*0076ed10*/ extern undefined4 g_dinput_76ed10;
/* dinput_create */
/* "Creating DInput object" */
undefined4 dinput_create(void)
{
  int hr;

  debug_printf("Creating DInput object\n");
  hr = DirectInput8Create(GetModuleHandleA(0), 0x800, &g_dinput_create_5e1f6c, &g_dinput_76ed10, 0);
  if (hr < 0) {
    debug_printf("Failed\n");
    return 0;
  }
  return 1;
}
