#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/*005e12e4*/ extern undefined g_dinput_release_5e12e4;
/*0076ed20*/ extern undefined4 g_dinput_76ed20;
/*00770f64*/ extern int g_app_770f64;


/* dinput_release_device */

/*  */

void dinput_release_device(void)

{
  int *device;
  int dwords_left;
  int device_index;
  undefined4 *device_slot;
  undefined4 *clear_cursor;
  
  device_index = 0;
  if (0 < g_app_770f64) {
    device_slot = &g_dinput_76ed20;
    do {
      device = (int *)*device_slot;
      (*(int (__stdcall **)(int *))(*device + 0x20))(device);
      debug_printf("Releasing device\n");
      (*(int (__stdcall **)(int *))(*device + 8))(device);
      clear_cursor = device_slot;
      for (dwords_left = 0x112; dwords_left != 0; dwords_left = dwords_left + -1) {
        *clear_cursor = 0;
        clear_cursor = clear_cursor + 1;
      }
      debug_printf(&g_dinput_release_5e12e4,device_index);
      device_index = device_index + 1;
      device_slot = device_slot + 0x112;
    } while (device_index < g_app_770f64);
  }
  return;
}
