#include "mfl_tier2.h"
/*007d26c8*/ extern undefined4 g_d3d9devicereleasevideomemory_7d26c8;
/* FUN_004dbda0: same machine code as build_info_set_page (tools/propagate_dups.py), byte gate checked */
void FUN_004dbda0(undefined4 page)
{
  g_d3d9devicereleasevideomemory_7d26c8 = page;
  return;
}
