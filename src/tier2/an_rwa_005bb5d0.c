/* rwa_timer_get_value_005bb5d0 (0x005bb5d0): RwaTimerGetValue, the raw timer tick (low 32 bits) */
#include "mfl_tier2.h"

extern __declspec(dllimport) int __stdcall QueryPerformanceCounter(void *count);
extern __declspec(dllimport) RwUInt32 __stdcall timeGetTime(void);

/*007daf40*/ extern int g_data_7daf40;      /* 1 = the performance counter is used (set by _rwaTimerModuleOpen) */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_timer_get_value_005bb5d0(void)
{
  RwUInt32 count[2];      /* LARGE_INTEGER */

  if (g_data_7daf40 == 1) {
    QueryPerformanceCounter(count);
    return count[0];
  }
  return timeGetTime();
}
