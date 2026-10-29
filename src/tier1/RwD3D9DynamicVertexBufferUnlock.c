/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/* Unlocks a dynamic vertex buffer (virtual call +0x30); TRUE on success. */
int RwD3D9DynamicVertexBufferUnlock(ComObj *vb)
{
  return ((int (__stdcall *)(ComObj *))vb->vtbl[0x30 / 4])(vb) >= 0;
}
