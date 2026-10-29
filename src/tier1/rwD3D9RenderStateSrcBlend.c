/* compile: /G5 */
#include "mfl_tier2.h"
/*007d4d04*/ extern int g_rw_7d4d04;
/*007d4ce0*/ extern int g_rw_7d4ce0;
/*007d3980*/ extern int g_data_7d3980;
/*007d3984*/ extern int g_data_7d3984;
/*005ea164*/ extern int g_rw_5ea164[];
/*007d3258*/ extern int g_rw_7d3258[];
/* Caches the source blend factor (mapped to the D3D blend enum) and queues state 0x13. */
int rwD3D9RenderStateSrcBlend(int v)
{
  if (v != g_rw_7d4ce0) { g_data_7d3980 = g_rw_5ea164[v]; if (g_data_7d3984 == 0) { g_data_7d3984 = 1; g_rw_7d3258[g_rw_7d4d04] = 0x13; g_rw_7d4d04++; } g_rw_7d4ce0 = v; }
  return 1;
}
