#include "mfl_tier2.h"
/*008deed4*/ extern undefined4 g_piz_fs_8deed4;
#define PIZ_FS_SUB_PTR_OFS (0x20064)   /* piz_fs -> sub-object pointer (inferred) */
#define PIZ_FS_SUB_POS_OFS (0x78)      /* sub-object -> uint counter (inferred) */
#define PIZ_FS_LIMIT_OFS (0x20070)     /* piz_fs -> uint limit (inferred) */
/* piz_fs_eof
 * Counts one call in g_piz_fs_8deed4, then tests the end-of-file condition (inferred).
 * piz_fs: piz_fs state block; the sub-object pointer and the limit are read from it.
 * Returns true when the sub-object's uint counter is <= the uint limit (inferred meaning: EOF).
 */

bool piz_fs_eof(char *piz_fs)

{
  /* bump the global call counter */
  g_piz_fs_8deed4 = g_piz_fs_8deed4 + 1;
  /* compare sub-object position against the limit (inferred) */
  return *(uint *)(*(char **)(piz_fs + PIZ_FS_SUB_PTR_OFS) + PIZ_FS_SUB_POS_OFS) <= *(uint *)(piz_fs + PIZ_FS_LIMIT_OFS);
}


