/* compile: /G5 */
#include "mfl_tier2.h"
/*00947140*/ extern int g_rp_947140;
/*004debc0*/ extern int __cdecl rxPipelineOpen(void);
/* Module open: stores the plugin offset and opens the render pipeline module; returns the argument or NULL. */
void *rwRenderPipelineOpen(void *p, int v)
{
  g_rp_947140 = v;
  return rxPipelineOpen() ? p : 0;
}
