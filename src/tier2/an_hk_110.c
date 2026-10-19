#include "mfl_tier2.h"
/*004c6c90*/ extern int __cdecl __stricmp_004c6c90(char *p0, char *p1);
/*008deec0*/ extern undefined4 g_piz_fs_8deec0;
/*008df008*/ extern undefined4 g_piz_8df008;
/*0093f120*/ extern undefined4 g_piz_93f120;

/* Size in bytes of one fixed-width name entry in the table at g_piz_93f120 (inferred). */
#define PIZ_FS_ENTRY_SIZE (0x80)

/* piz_fs_exists
 *
 * Checks whether a file name is in the piz file table: a flat array of
 * PIZ_FS_ENTRY_SIZE-byte name entries starting at g_piz_93f120, with
 * g_piz_8df008 entries in total. Names are compared case-insensitively.
 *
 * param_1: unused (inferred: kept for the calling convention).
 * search_name: file name to look for.
 * Returns 1 if an entry matches search_name, 0 otherwise.
 * Also bumps the call counter g_piz_fs_8deec0 on every call (inferred: a call counter).
 */
undefined4 piz_fs_exists(char *param_1,char *search_name)

{
  int cmp_result;
  uint entry_index;
  char *entry_name;

  g_piz_fs_8deec0 = g_piz_fs_8deec0 + 1;
  entry_index = 0;
  if (g_piz_8df008 != 0) {
    /* Walk the table entry by entry, comparing each name with search_name. */
    entry_name = (char *)&g_piz_93f120;
    do {
      cmp_result = __stricmp_004c6c90(entry_name,search_name);
      if (cmp_result == 0) {
        return 1;
      }
      entry_index = entry_index + 1;
      entry_name = entry_name + PIZ_FS_ENTRY_SIZE;
    } while (entry_index < g_piz_8df008);
  }
  return 0;
}


