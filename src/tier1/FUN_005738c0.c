/* compile: /G5 */
#include "mfl_tier2.h"
/*0063a54c*/ extern int g_data_63a54c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Reads the int at plugin offset g_data_63a54c of an object; 0 when the plugin is not registered (-1). (Rwp library, proposed name rwp_pipeline_005738c0) */
int FUN_005738c0(void *o)
{
  int off = g_data_63a54c;
  if (off == -1) return 0;
  return FLD(o, int, off);
}
