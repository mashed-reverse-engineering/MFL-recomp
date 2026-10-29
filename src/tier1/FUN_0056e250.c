/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void *(__cdecl *RwMallocFn)(unsigned, unsigned);
/*0056e280*/ extern int __cdecl FUN_0056e280(void *mem, int a, int b, int c);
/* Allocates 0x20 bytes from the engine heap and forwards to FUN_0056e280. (Rwp library, proposed name rwp_obj_0056e250) */
int FUN_0056e250(int a, int b, int c)
{
  void *mem = (*(RwMallocFn *)(RwEngineInstance + 0x108))(0x20, 0x30900);
  return FUN_0056e280(mem, a, b, c);
}
