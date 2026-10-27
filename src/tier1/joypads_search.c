#include "mfl_tier2.h"
extern char LAB_004a5030[];
/*004a4670*/ extern int *app_get_dinput_76ed10(void);
/*004a8420*/ extern int debug_printf(char *fmt, ...);
/*00770f64*/ extern undefined4 g_app_770f64;
/* joypads_search */
/* "Searching for joypads / Found %d joypad(s)" */
undefined4 joypads_search(void)
{
  int *dinput;
  int vtable;
  int hr;

  debug_printf("Searching for joypads\n");
  dinput = app_get_dinput_76ed10();
  vtable = *dinput;
  hr = ((int (__stdcall *)(int *, int, char *, undefined4 *, int))*(int *)(vtable + 0x10))(app_get_dinput_76ed10(), 4, LAB_004a5030, &g_app_770f64, 1);
  if (hr < 0) {
    debug_printf("Failed\n");
    return 0;
  }
  debug_printf("Found %d joypad(s)\n", g_app_770f64);
  return 1;
}
