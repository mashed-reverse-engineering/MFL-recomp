#include "mfl_tier2.h"
/*007daf30*/ extern undefined4 g_data_7daf30;
/*007daf40*/ extern undefined4 g_data_7daf40;
#define FALLBACK_VALUE (1000) /* used when g_data_7daf40 != 1 (inferred) */
/* Returns g_data_7daf30 as the value when g_data_7daf40 == 1, otherwise FALLBACK_VALUE. */
/* FUN_005bb600
 * Returns g_data_7daf30 as the value when g_data_7daf40 == 1, otherwise FALLBACK_VALUE.
 * Takes no parameters; returns the selected value (inferred meaning of the globals). */
undefined4 FUN_005bb600(void)
{
  undefined4 value;

  value = g_data_7daf30;
  if (g_data_7daf40 != 1) {
    value = FALLBACK_VALUE;
  }
  return value;
}
