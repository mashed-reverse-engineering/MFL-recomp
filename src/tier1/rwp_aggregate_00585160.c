/* compile: /G5 */
#include "mfl_tier2.h"
extern int g_rwp_simd_enabled;
/* Returns the SIMD-enabled flag set by the CPU feature check. (Rwp library) */
int rwp_aggregate_00585160(void)
{
  return g_rwp_simd_enabled;
}
