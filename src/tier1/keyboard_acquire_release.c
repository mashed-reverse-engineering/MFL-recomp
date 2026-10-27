#include "mfl_tier2.h"
/*00770f70*/ extern int *g_app_770f70;
/* IDirectInputDevice vtable slots (inferred from the DirectInput interface layout) */
#define DIDEVICE_VTBL_ACQUIRE (0x1c)
#define DIDEVICE_VTBL_UNACQUIRE (0x20)
/* keyboard_acquire_release */
/* "acquire / unacquire the DirectInput keyboard device"
 * acquire != 0: call the device's Acquire (slot 7) to take the keyboard.
 * acquire == 0: call the device's Unacquire (slot 8) to release it.
 * Does nothing if the global DirectInput device pointer is NULL (inferred: device not created yet or already torn down). Returns nothing. */
void __cdecl keyboard_acquire_release(int acquire)
{
  if (g_app_770f70 != (int *)0x0) {
    /* acquire the keyboard, then return early */
    if (acquire != 0) {
      (*(int (__stdcall **)(int *))(*g_app_770f70 + DIDEVICE_VTBL_ACQUIRE))(g_app_770f70);
      return;
    }
    /* otherwise release it */
    (*(int (__stdcall **)(int *))(*g_app_770f70 + DIDEVICE_VTBL_UNACQUIRE))(g_app_770f70);
  }
  return;
}
