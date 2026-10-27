#include "mfl_tier2.h"
/*00486c40*/ extern int particles_flush();
/*0049b9c0*/ extern int fx_ribbon_flush_spline();
/*00759700*/ extern undefined4 g_fx_system_759700;

/* byte offsets into struct fx_system (names inferred from use) */
#define FX_SYSTEM_ACTIVE_OFF (0x54)          /* nonzero = system is live, flush it */
#define FX_SYSTEM_PARTICLES_OFF (0x10)       /* particle sub-state passed to particles_flush */
#define FX_SYSTEM_PENDING_COUNT_OFF (0x8)    /* pending particle count, copied out then zeroed */
#define FX_SYSTEM_FLUSHED_OFF (0xc)          /* set to 1 once flushed */

/* fx_system_flush */

/* "flushes the pending particles of the system and stores its count" */
/* Flushes a live FX system: ribbon spline and particles, then moves the pending
   particle count into g_fx_system_759700 and zeroes it (inferred). fx_system is the
   system struct pointer. Returns nothing. */

void fx_system_flush(char *fx_system)

{
  /* only systems with the active flag set are flushed (inferred) */
  if (*(int *)(fx_system + FX_SYSTEM_ACTIVE_OFF) != 0) {
    fx_ribbon_flush_spline(fx_system);
    particles_flush(fx_system + FX_SYSTEM_PARTICLES_OFF);
    g_fx_system_759700 = *(undefined4 *)(fx_system + FX_SYSTEM_PENDING_COUNT_OFF);
    *(undefined4 *)(fx_system + FX_SYSTEM_PENDING_COUNT_OFF) = 0;
    *(undefined4 *)(fx_system + FX_SYSTEM_FLUSHED_OFF) = 1;
  }
  return;
}


