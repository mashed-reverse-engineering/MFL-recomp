/* compile: /G5 */
#include "mfl_tier2.h"
/*004d2f20*/ extern int __cdecl RwEngineRegisterPlugin(int size, int id, void *ctor, void *dtor);
/*00556b90*/ extern void *__cdecl SplineClose(void *);
/*00556bf0*/ extern void *__cdecl SplineOpen(void *);
/* Registers the spline engine plugin (id 0x102); TRUE on success. */
int RpSplinePluginAttach(void)
{
  return RwEngineRegisterPlugin(0, 0x102, SplineOpen, SplineClose) >= 0;
}
