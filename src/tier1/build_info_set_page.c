#include "mfl_tier2.h"
/*0069bb30*/ extern undefined4 g_build_info_69bb30;


/* build_info_set_page */

/* "g_build_info page index (0 / 1 ...)" */

/* Select the build-info page (0 or 1, ...). */
void build_info_set_page(undefined4 page)
{
  g_build_info_69bb30 = page;
  return;
}
