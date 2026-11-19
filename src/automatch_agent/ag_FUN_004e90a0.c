#include "mfl_tier2.h"
/*0062e9bc*/ extern undefined4 g_freelistcreate_62e9bc;
/* FUN_004e90a0: same machine code as build_info_set_page (tools/propagate_dups.py), byte gate checked */
void FUN_004e90a0(undefined4 page)
{
  g_freelistcreate_62e9bc = page;
  return;
}
