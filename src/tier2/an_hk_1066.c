#include "mfl_tier2.h"
/*006c3348*/ extern undefined1 g_text_pages_6c3348;
/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/*006c3d9c*/ extern undefined4 g_text_pages_6c3d9c;
/* course_helper_4801f0 */

/* callers course_egypt_cb12,course_rouabout_cb12 */

void course_helper_4801f0(char *param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = g_text_pages_6c3d9c;
  iVar2 = g_text_pages_6c3d9c * 0x84 - (int)param_1;
  do {
    cVar1 = *param_1;
    param_1[(int)((char *)&g_text_pages_6c3348 + iVar2)] = cVar1;
    param_1 = param_1 + 1;
  } while (cVar1 != '\0');
  (&g_gfx_6c33c8)[iVar3 * 0x21] = param_2;
  g_text_pages_6c3d9c = g_text_pages_6c3d9c + 1;
  return;
}


