/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*00947778*/ extern int g_tristripstriptris_947778;
/* Returns the next mesh serial number (16 bit counter in the mesh plugin data). */
unsigned short rpMeshGetNextSerialNumber(void)
{
  unsigned short *p = (unsigned short *)(RwEngineInstance + g_tristripstriptris_947778);
  unsigned short n = *p;
  *p = n + 1;
  return n;
}
