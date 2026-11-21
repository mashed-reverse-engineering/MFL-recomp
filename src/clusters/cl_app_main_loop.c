// cluster-entries: app_main_loop
/* Installed cluster app_main_loop (tools/cluster_install.py from output/cluster_pilot_x2/app_main_loop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#include "mfl_fix.h"
extern __declspec(dllimport) void __stdcall Sleep(int milliseconds);
/*00843e7c*/ extern int g_quit_requested;
/*0076e7e8*/ extern int g_sleep_remainder;
/*0076e840*/ extern int g_app_main_76e840;
/*0076e844*/ extern int g_app_main_76e844;
/*0076e848*/ extern int g_app_main_76e848;
/*004a1b20*/ extern void main_loop_init(void);
/*004a9300*/ extern int pump_windows_messages(void);
/*004a41f0*/ extern int timer_ticks(void);
/*004a1d80*/ extern void frontend_init_menus(void);
/*004a20d0*/ extern void input_update(void);
/*004a20e0*/ extern void app_state_frontend_frame(void);
/*004a2240*/ extern void app_state_menu_render(void);
/*004a1a70*/ extern void debug_keys_and_profile(void);
/*004a2880*/ extern void app_frame_wait(void);

#define FRAME_TICKS 50000
#define TICKS_PER_MS 3000

#if MFL_FIX_FPS_CAP
extern __declspec(dllimport) unsigned int __stdcall timeBeginPeriod(unsigned int period_ms);

/* MFL_FIX_FPS_CAP: wait until the next point of a fixed 60 Hz schedule (one FRAME_TICKS step after the last one) in 1 ms Sleeps, the
   last one ending less than half a ms early; a frame more than one budget late restarts the schedule (no burst of short frames after a
   stall).  Every Sleep advances the harness's virtual clock, so the loop also ends there. */
#if MFL_FIX_FIXED_STEP && MFL_FIX_PREFS
static const int k_fps_cap[3] = { 60, 30, 0 };      /* the key fps_cap (index of its word); 0 = no limit (vsync or the GPU sets the rate) */
#endif

static void frame_wait_schedule(void)
{
  static int timer_period_set;
  static int deadline;
  int budget = FRAME_TICKS;
  int late;

  if (timer_period_set == 0) {
    timeBeginPeriod(1);
    timer_period_set = 1;
  }
#if MFL_FIX_FIXED_STEP && MFL_FIX_PREFS      /* the simulation follows real time, so the frame rate is free */
  budget = mfl_pref(MFL_PREF_FPS_CAP);
  budget = budget >= 0 && budget < 3 ? k_fps_cap[budget] : 60;
  if (budget == 0) {
    return;
  }
  budget = FRAME_TICKS * 60 / budget;
#endif
  deadline = deadline + budget;
  late = timer_ticks() - deadline;
  if (late > budget || late < -budget) {      /* also the first call (and after fps_cap changed): deadline is anywhere relative to the timer */
    deadline = timer_ticks();
  }
  while (deadline - timer_ticks() >= TICKS_PER_MS / 2) {
    Sleep(1);
  }
}
#endif

/* Main loop: one frame per iteration, then sleep away what is left of the 50000-tick frame budget (whole ms; the sub-ms remainder is carried in g_sleep_remainder). */
int app_main_loop(void)
{
  int start;
  int frame_ticks;
  int sleep_ms;

  main_loop_init();
  while (g_quit_requested == 0 && pump_windows_messages() == 0) {
    start = timer_ticks();
    frontend_init_menus();
    input_update();
    app_state_frontend_frame();
    app_state_menu_render();
    debug_keys_and_profile();
    app_frame_wait();
    frame_ticks = timer_ticks() - start;
    if (frame_ticks < FRAME_TICKS) {
      sleep_ms = (FRAME_TICKS - frame_ticks) / TICKS_PER_MS;
      g_sleep_remainder = g_sleep_remainder + (FRAME_TICKS - frame_ticks) % TICKS_PER_MS;
      if (g_sleep_remainder >= TICKS_PER_MS) {
        sleep_ms = sleep_ms + 1;
        g_sleep_remainder = g_sleep_remainder - TICKS_PER_MS;
      }
#if !MFL_FIX_FPS_CAP
      Sleep(sleep_ms);      /* with MFL_FIX_FIXED_STEP alone this cap still holds the frame rate at <= 60 */
#endif
      g_app_main_76e840 = sleep_ms;      /* with MFL_FIX_FPS_CAP only the statistic; the wait is below */
    } else {
      g_app_main_76e840 = 0;
    }
#if MFL_FIX_FPS_CAP
    frame_wait_schedule();
#endif
    g_app_main_76e844 = g_sleep_remainder;
    g_app_main_76e848 = frame_ticks;
  }
  return 1;
}

