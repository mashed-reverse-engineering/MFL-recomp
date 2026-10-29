/* compile: /G5 */
#include "mfl_tier2.h"
/*004deb60*/ extern void __cdecl rxPipelineClose(void);
/* Module close: shuts the render pipeline module down; returns its argument. */
void *rwRenderPipelineClose(void *p)
{
  rxPipelineClose();
  return p;
}
