// expect: clean class 21
/* timer_ticks as fixed in T-P19 (recomp/automatch_near/an_hk_496.c) */
extern __declspec(dllimport) int __stdcall QueryPerformanceCounter(int);
/*004b4160*/ extern __int64 __stdcall __alldiv(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004b4210*/ extern __int64 __stdcall __allmul(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0076ed08*/ extern undefined4 g_app_76ed08;
/*0076ed0c*/ extern undefined4 g_app_76ed0c;
#define TIMER_TICKS_PER_SECOND (3000000)
__int64 timer_ticks(void)
{
  __int64 scaled_ticks;
  LARGE_INTEGER perf_counter;

  perf_counter.LowPart = 0;
  perf_counter.HighPart = 0;
  QueryPerformanceCounter(&perf_counter);
  scaled_ticks = __allmul(perf_counter.LowPart,perf_counter.HighPart,TIMER_TICKS_PER_SECOND,0);
  return __alldiv((undefined4)scaled_ticks,(undefined4)((unsigned __int64)scaled_ticks >> 0x20),g_app_76ed08,g_app_76ed0c);
}
