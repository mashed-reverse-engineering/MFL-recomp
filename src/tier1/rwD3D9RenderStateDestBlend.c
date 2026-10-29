/* compile: /G5 */
#include "mfl_tier2.h"
/*007d4d04*/ extern int g_rw_7d4d04;
/*007d4ce4*/ extern int g_rw_7d4ce4;
/*007d3988*/ extern int g_data_7d3988;
/*007d398c*/ extern int g_data_7d398c;
/*005ea164*/ extern int g_rw_5ea164[];
/*007d3258*/ extern int g_rw_7d3258[];
/* Caches the destination blend factor (mapped to the D3D blend enum) and queues state 0x14. */
int rwD3D9RenderStateDestBlend(int v)
{
  if (v != g_rw_7d4ce4) { g_data_7d3988 = g_rw_5ea164[v]; if (g_data_7d398c == 0) { g_data_7d398c = 1; g_rw_7d3258[g_rw_7d4d04] = 0x14; g_rw_7d4d04++; } g_rw_7d4ce4 = v; }
  return 1;
}
