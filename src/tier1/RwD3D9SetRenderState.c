/* compile: /G5 */
#include "mfl_tier2.h"
/*007d4d04*/ extern int g_rw_7d4d04;
/*007d3258*/ extern int g_rw_7d3258[];
typedef struct { int value; int dirty; } RwStateEntry;
/*007d38e8*/ extern RwStateEntry g_rw_7d38e8[];
/* Sets a cached D3D9 render state; first change queues the state in the dirty list. */
void RwD3D9SetRenderState(int state, int value)
{
  if (g_rw_7d38e8[state].value != value) { g_rw_7d38e8[state].value = value; if (g_rw_7d38e8[state].dirty == 0) { g_rw_7d38e8[state].dirty = 1; g_rw_7d3258[g_rw_7d4d04] = state; g_rw_7d4d04++; } }
}
