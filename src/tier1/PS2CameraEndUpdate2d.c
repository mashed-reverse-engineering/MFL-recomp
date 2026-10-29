/* compile: /G5 */
#include "mfl_tier2.h"
typedef void (__cdecl *V0)(void);
/*009482b8*/ extern V0 g_rt_9482b8;
/* Forwards to the registered 2D camera end-update callback. */
void PS2CameraEndUpdate2d(void)
{
  g_rt_9482b8();
}
