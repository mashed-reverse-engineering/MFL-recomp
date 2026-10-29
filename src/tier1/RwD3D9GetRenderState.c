/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { int value; int pad; } RwD3D9StateEntry;
/*007d38e8*/ extern RwD3D9StateEntry g_rw_7d38e8[];
/* Cached value of a D3D9 render state. */
void RwD3D9GetRenderState(int s, int *out)
{
  *out = g_rw_7d38e8[s].value;
}
