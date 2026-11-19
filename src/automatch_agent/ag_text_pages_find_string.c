#include "mfl_tier2.h"
/*004b3850*/ extern char * __cdecl _strstr(char *p0, char *p1);
/*006c3348*/ extern undefined1 g_text_pages_6c3348;
/*006c3d98*/ extern char g_physics_scenario_count[];


/* text_pages_find_string */

/* "strstr over the text pages, returns the page index" */

int text_pages_find_string(char *_SubStr)

{
  char *match;
  char *_Str;
  int page_index;
  
  page_index = 0;
  _Str = &g_text_pages_6c3348;
  do {
    match = _strstr(_Str,_SubStr);
    if (match != (char *)0x0) {
      return page_index + 2000;
    }
    _Str = _Str + 0x84;
    page_index = page_index + 1;
  } while ((int)_Str < ((int)&g_physics_scenario_count));
  return -1;
}
