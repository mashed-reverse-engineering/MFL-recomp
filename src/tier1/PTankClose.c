/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007da6c4*/ extern ComObj *g_data_7da6c4;
/*007da6c8*/ extern int g_data_7da6c8;
/* Module close: releases the shared D3D9 object and clears the pointers; returns its argument. */
void *PTankClose(void *x)
{
  if (g_data_7da6c4) { ((void (__stdcall *)(ComObj *))g_data_7da6c4->vtbl[2])(g_data_7da6c4); g_data_7da6c4 = 0; }
  g_data_7da6c8 = 0;
  return x;
}
