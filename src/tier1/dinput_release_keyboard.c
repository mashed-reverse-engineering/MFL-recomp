#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/*00770f70*/ extern int *g_app_770f70;
/* method of a DirectInput device (COM, stdcall: the callee pops the this pointer) */
typedef int (__stdcall *DeviceMethod)(int *device);
#define DEVICE_VTBL_UNACQUIRE (0x20)
#define DEVICE_VTBL_RELEASE (0x8)
/* dinput_release_keyboard */
/* "unacquires and releases the DirectInput keyboard device" */
void dinput_release_keyboard(void)
{
  (*(DeviceMethod *)(*g_app_770f70 + DEVICE_VTBL_UNACQUIRE))(g_app_770f70);
  if (g_app_770f70 != (int *)0x0) {
    debug_printf("Releasing _lpDIDKeyboard\n");
    (*(DeviceMethod *)(*g_app_770f70 + DEVICE_VTBL_RELEASE))(g_app_770f70);
    g_app_770f70 = (int *)0x0;
  }
  return;
}
