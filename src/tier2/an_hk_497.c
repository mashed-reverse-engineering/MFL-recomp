#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall QueryPerformanceFrequency(int);
/*0076ed08*/ extern undefined4 g_app_76ed08;
/*0076ed0c*/ extern undefined4 g_app_76ed0c;
/* timer_init_performance_counter
 * Reads the high-resolution performance counter frequency (ticks per second)
 * via QueryPerformanceFrequency and stores it as two 32-bit halves:
 * g_app_76ed08 = low part, g_app_76ed0c = high part.
 * Takes no parameters. Always returns 1 (success).
 */

undefined4 timer_init_performance_counter(void)

{
  LARGE_INTEGER frequency;

  /* Query the counter frequency and split the 64-bit result into two globals */
  QueryPerformanceFrequency(&frequency);
  g_app_76ed08 = frequency.LowPart;
  g_app_76ed0c = frequency.HighPart;
  return 1;
}


