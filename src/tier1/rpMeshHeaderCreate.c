/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Allocates a mesh header of the given size from the engine heap. */
void *rpMeshHeaderCreate(unsigned size)
{
  return RwMalloc(size, 0x30502);
}
