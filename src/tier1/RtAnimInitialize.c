/* compile: /G5 */
#include "mfl_tier2.h"
/*004d2f20*/ extern int __cdecl RwEngineRegisterPlugin(int size, int id, void *ctor, void *dtor);
/*0053de60*/ extern void *__cdecl LAB_0053de60(void *);
/*0053dea0*/ extern void *__cdecl LAB_0053dea0(void *);
/* Registers the animation engine plugin (id 0x1b7); TRUE when the offset is positive. */
int RtAnimInitialize(void)
{
  return RwEngineRegisterPlugin(0, 0x1b7, LAB_0053de60, LAB_0053dea0) > 0;
}
