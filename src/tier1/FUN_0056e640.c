/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *RwMallocFn)(unsigned, unsigned);
/*0056e670*/ extern void __cdecl SUB_0056e670(void *o);
/* Allocates 0xb4 bytes from the engine heap and constructs an object (SUB_0056e670). (Rwp library, proposed name rwp_obj_0056e640) */
void *FUN_0056e640(void)
{
  void *mem = (*(RwMallocFn *)(RwEngineInstance + 0x108))(0xb4, 0x30900);
  SUB_0056e670(mem);
  return mem;
}
