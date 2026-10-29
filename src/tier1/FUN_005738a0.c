/* compile: /G5 */
#include "mfl_tier2.h"
/*0063a548*/ extern int g_data_63a548;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Reads the int at the plugin offset g_data_63a548 of an object; 0 when the plugin is not registered (-1). (Rwp library, proposed name rwp_pipeline_005738a0) */
int FUN_005738a0(void *o)
{
  int off = g_data_63a548;
  if (off == -1) return 0;
  return FLD(o, int, off);
}
