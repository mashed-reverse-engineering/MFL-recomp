/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*004d2e20*/ extern int __cdecl rwDeviceSystemRequest(void *engine, int op, void *a, void *b, int c);
/* Selects the device sub-system; TRUE on success. */
int RwEngineSetSubSystem(int n)
{
  return rwDeviceSystemRequest(RwEngineInstance + 0x10, 0x10, 0, 0, n) != 0;
}
