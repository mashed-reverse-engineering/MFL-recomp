#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CoCreateInstance(int, int, int, int, int);
/*005e200c*/ extern undefined g_data_5e200c;
/*005e210c*/ extern undefined g_data_5e210c;
#define CLSCTX_INPROC_SERVER (1)
/* Creates an in-process COM object (inferred) via CoCreateInstance: the class id is g_data_5e210c and the interface id is g_data_5e200c, both globals. No outer aggregate (NULL). The ... */
/* FUN_004aebb0
 * Creates an in-process COM object (inferred) via CoCreateInstance: the class id is
 * g_data_5e210c and the interface id is g_data_5e200c, both globals. No outer
 * aggregate (NULL). The created interface pointer is written to instance_out.
 * Return value is ignored (the HRESULT is dropped).
 */
void __stdcall FUN_004aebb0(LPVOID *instance_out)
{
  CoCreateInstance((IID *)&g_data_5e210c,(LPUNKNOWN)0x0,CLSCTX_INPROC_SERVER,(IID *)&g_data_5e200c,instance_out);
  return;
}
