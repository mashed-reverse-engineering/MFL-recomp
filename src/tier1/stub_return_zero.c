#include "mfl_tier2.h"


/* stub_return_zero */

/* "xor eax,eax; ret" - registered as Lua Course_Id and Course_Name (no-ops) and called from many
   unrelated places (identical code folded by the linker) */

undefined4 stub_return_zero(void)
{
  return 0;
}
