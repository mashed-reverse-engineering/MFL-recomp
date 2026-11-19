#include "mfl_tier2.h"
/*007da8ac*/ extern undefined4 g_data_7da8ac;
/* FUN_00560cf0: same machine code as build_info_set_page (tools/propagate_dups.py), byte gate checked */
void FUN_00560cf0(undefined4 page)
{
  g_data_7da8ac = page;
  return;
}
