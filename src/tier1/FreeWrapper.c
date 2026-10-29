/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *FnV1)(void *);
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Device free wrapper: frees the block through the engine free callback; returns the device. */
void *FreeWrapper(void *dev, void *p)
{
  ((FnV1)ENG(0x10c, void *))(p);
  return dev;
}
