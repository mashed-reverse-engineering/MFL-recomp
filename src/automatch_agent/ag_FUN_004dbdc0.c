#include "mfl_tier2.h"
/*007d26cc*/ extern undefined4 g_rw_7d26cc;
/* FUN_004dbdc0: same machine code as build_info_set_page (tools/propagate_dups.py), byte gate checked */
void FUN_004dbdc0(undefined4 page)
{
  g_rw_7d26cc = page;
  return;
}
