/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6b8*/ extern int g_rp_7da6b8;
/*004d2f20*/ extern int __cdecl RwEngineRegisterPlugin(int size, int id, void *ctor, void *dtor);
/*00544f60*/ extern void *__cdecl RandomClose(void *);
/*00544fa0*/ extern void *__cdecl RandomOpen(void *);
/* Registers the random generator engine plugin (id 0x10c, 16 bytes) and stores its offset; TRUE on success. */
int RpRandomPluginAttach(void)
{
  int off = RwEngineRegisterPlugin(0x10, 0x10c, RandomOpen, RandomClose);
  g_rp_7da6b8 = off;
  return off >= 0;
}
