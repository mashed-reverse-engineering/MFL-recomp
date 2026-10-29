/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rx_7d53c4)(void);
typedef struct RwNode53c8 { void *obj; struct RwNode53c8 *next; } RwNode53c8;
/*007d53c8*/ extern RwNode53c8 *g_rp_7d53c8;
/* Device restore: calls the registered callback, then resets the cached state word of every registered D3D9 object. */
void D3D9WorldRestoreCallback(void)
{
  RwNode53c8 *n;
  if (g_rx_7d53c4) g_rx_7d53c4();
  for (n = g_rp_7d53c8; n; n = n->next) *(short *)((char *)n->obj + 0xc) = 0xfff;
}
