/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*004d2e20*/ extern int __cdecl rwDeviceSystemRequest(void *engine, int op, void *a, void *b, int c);
/* Number of device sub-systems (device system request 0xd). */
int RwEngineGetNumSubSystems(void)
{
  int n = 1;
  rwDeviceSystemRequest(RwEngineInstance + 0x10, 0xd, &n, 0, 0);
  return n;
}
