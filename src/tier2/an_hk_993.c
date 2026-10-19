#include "mfl_tier2.h"
/*004c6600*/ extern void FUN_004c6600(undefined4 p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00770f70*/ extern undefined4 *g_app_770f70;
/* keyboard_state_to_bits */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "reads the 256 DirectInput key states and sets the bit per pressed key" */

/* IDirectInputDevice methods (stdcall: the callee pops the arguments) */
typedef int (__stdcall *DeviceGetStateMethod)(undefined4 *self,int size,char *state);
typedef int (__stdcall *DeviceAcquireMethod)(undefined4 *self);
#define DIDEVICE_VTBL_ACQUIRE (0x1c)
#define DIDEVICE_VTBL_GETDEVICESTATE (0x24)
#define KEY_COUNT (0x100)

undefined4 __cdecl keyboard_state_to_bits(int param_1,undefined4 param_2,undefined4 param_3)

{
  int result;
  int key;
  char keys[KEY_COUNT];
  
  FUN_004c6600(param_1,KEY_COUNT);
  mem_zero(keys,KEY_COUNT);
  result = (*(DeviceGetStateMethod *)(*g_app_770f70 + DIDEVICE_VTBL_GETDEVICESTATE))(g_app_770f70,KEY_COUNT,keys);
  if ((-1 < result) ||
     ((result = (*(DeviceAcquireMethod *)(*g_app_770f70 + DIDEVICE_VTBL_ACQUIRE))(g_app_770f70), -1 < result &&
      (result = (*(DeviceGetStateMethod *)(*g_app_770f70 + DIDEVICE_VTBL_GETDEVICESTATE))(g_app_770f70,KEY_COUNT,keys), -1 < result)))) {
    /* one bit per key, set when the high bit of the key state is set */
    for (key = 0; key < KEY_COUNT; key = key + 1) {
      if (keys[key] < '\0') {
        *(byte *)((key >> 3) + param_1) = *(byte *)((key >> 3) + param_1) | (byte)(1 << ((byte)key & 7));
      }
    }
  }
  return 1;
}
