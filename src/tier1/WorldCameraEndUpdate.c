/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *V1)(void *);
/*007d529c*/ extern int g_rp_7d529c;
/* Clears the engine's current world and calls the camera's world end-update callback (plugin +0x14). */
void WorldCameraEndUpdate(void *cam)
{
  char *e = RwEngineInstance;
  char *p = (char *)cam + g_rp_7d529c;
  *(int *)(e + 4) = 0;
  (*(V1 *)(p + 0x14))(cam);
}
