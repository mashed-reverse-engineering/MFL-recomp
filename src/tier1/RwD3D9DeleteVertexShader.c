/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/* Releases a D3D9 vertex shader (virtual Release call). */
int RwD3D9DeleteVertexShader(ComObj *s)
{
  return ((int (__stdcall *)(ComObj *))s->vtbl[2])(s);
}
