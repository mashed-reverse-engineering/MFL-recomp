/* rwa_soft_dsp_set_param_005da0d0 (0x005da0d0): software dsp: parameter callback, forwards the value to 0x5da030 */
#include "mfl_tier2.h"

/*005da030*/ extern void FUN_005da030(void *dsp, int value);

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_dsp_set_param_005da0d0(void *dsp, int param, const int *value)
{
  (void)param;
  FUN_005da030(dsp, *value);
  return dsp;
}
