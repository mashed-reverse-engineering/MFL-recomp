/* compile: /G5 */
#include "mfl_tier2.h"
/*007d54a0*/ extern int g_rw_7d54a0;
/*004f4cf0*/ extern int __cdecl RpLightRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy);
/*00505150*/ extern void *__cdecl rpD3D9LightConstructor(void *);
/*00505180*/ extern void *__cdecl rpD3D9LightDestructor(void *);
/*00505270*/ extern void *__cdecl rpD3D9LightCopy(void *, void *);
/* Registers the D3D9 light plugin (id 0x505, 16 bytes) and stores its offset; TRUE on success. */
int rpD3D9LightPluginAttach(void)
{
  int off = RpLightRegisterPlugin(0x10, 0x505, rpD3D9LightConstructor, rpD3D9LightDestructor, rpD3D9LightCopy);
  g_rw_7d54a0 = off;
  return off >= 0;
}
