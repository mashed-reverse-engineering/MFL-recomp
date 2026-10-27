#include "mfl_tier2.h"
/*004a4fd0*/ extern int dinput_release_device();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0076ed18*/ extern undefined4 g_app_76ed18;
/*0076ed20*/ extern undefined4 g_dinput_76ed20;
/*00770f60*/ extern float g_app_770f60;
/*00770f64*/ extern undefined4 g_app_770f64;
/*00770f68*/ extern undefined4 g_app_770f68;
#define DINPUT_DEVICE_TABLE_SIZE (0x2240)
/* dinput_release_all */
/* "dinput_release_device, clears the 0x2240 byte device table" */
/* Releases all DirectInput devices (inferred from dinput_release_device), then zeroes the device table and resets the
   related globals (g_app_770f64, g_app_76ed18, g_app_770f60, g_app_770f68) to 0.
   Takes no parameters and returns nothing. */
void dinput_release_all(void)
{
  dinput_release_device();
  mem_zero(&g_dinput_76ed20,DINPUT_DEVICE_TABLE_SIZE);
  g_app_770f64 = 0;
  g_app_76ed18 = 0;
  g_app_770f60 = 0;
  g_app_770f68 = 0;
  return;
}
