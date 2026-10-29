/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Address of the engine memory function table (malloc/free hooks) inside the engine instance. */
void *RwOsGetMemoryInterface(void)
{
  return RwEngineInstance + 0x108;
}
