/* RenderWare Audio (Criterion Software) timer module, MFL.exe 0x005bb620 and 0x005bb680 (the two words of the module table row
 * at 0x6491b8).  Written from the MFL.exe disassembly (capstone).  The performance counter is used when QueryPerformanceCounter
 * works, else timeGetTime. */
#include "mfl_tier2.h"

extern __declspec(dllimport) int __stdcall QueryPerformanceCounter(void *count);
extern __declspec(dllimport) int __stdcall QueryPerformanceFrequency(void *frequency);
extern __declspec(dllimport) RwUInt32 __stdcall timeGetTime(void);

/*007daf30*/ extern RwUInt32 g_data_7daf30[2];      /* counter frequency (LARGE_INTEGER) */
/*007daf38*/ extern RwUInt32 g_data_7daf38[2];      /* start tick (LARGE_INTEGER) */
/*007daf40*/ extern int g_data_7daf40;              /* 1 = the performance counter is used */

/* module open: picks the counter and stores the start tick.  Always 1. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool rwa_timer_module_open_005bb620(void)
{
  RwUInt32 count[2];
  RwUInt32 start;

  g_data_7daf40 = QueryPerformanceCounter(g_data_7daf38);
  if (g_data_7daf40 == 1) {
    QueryPerformanceFrequency(g_data_7daf30);
    if (g_data_7daf40 == 1) {
      QueryPerformanceCounter(count);
      start = count[0];
      goto store;
    }
  }
  start = timeGetTime();
store:
  g_data_7daf38[0] = start;
  g_data_7daf38[1] = 0;
  return 1;
}

/* second word of the module row.  Unlike RwaTimerGetValue 0x5bb5d0 it returns QueryPerformanceCounter's result (1), not the
   counter: the original discards the value it read. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwUInt32 rwa_timer_module_close_005bb680(void)
{
  RwUInt32 count[2];

  if (g_data_7daf40 == 1)
    return QueryPerformanceCounter(count);
  return timeGetTime();
}
