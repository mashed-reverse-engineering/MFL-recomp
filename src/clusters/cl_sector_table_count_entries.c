// cluster-entries: sector_table_count_entries
/* Installed cluster sector_table_count_entries (tools/cluster_install.py from output/cluster_pilot10/sector_table_count_entries.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00702268*/ extern undefined4 g_sector_table_702268;
/*00702668*/ extern undefined g_sector_table_702668;
/* sector_table_count_entries */

/* WARNING: Unknown calling convention */
/* "stores the table pointer for the sector and counts its entries up to the zero terminator" */

void sector_table_count_entries_n(int sector, char *table_cursor)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  int *count_ptr;
  char *pcVar5;
  
  count_ptr = &g_sector_table_702268 + sector;
  *(char **)((char *)&g_sector_table_702668 + sector * 4) = table_cursor;
  *count_ptr = 0;
  if (((table_cursor[3] != '\0' || table_cursor[2] != '\0') || table_cursor[1] != '\0') || *table_cursor != '\0') {
    do {
      pcVar5 = table_cursor + 4;
      *count_ptr = *count_ptr + 1;
      pcVar1 = table_cursor + 6;
      pcVar2 = table_cursor + 7;
      pcVar3 = table_cursor + 5;
      table_cursor = pcVar5;
    } while (((*pcVar2 != '\0' || *pcVar1 != '\0') || *pcVar3 != '\0') || *pcVar5 != '\0');
  }
  return;
}

/* entry thunk at the original address of sector_table_count_entries: register inputs (edx,eax) -> natural C sector_table_count_entries_n; ret 0 */
__declspec(naked) void sector_table_count_entries(void)
{
    __asm {
        push eax
        push edx
        call sector_table_count_entries_n
        add esp, 8
        ret
    }
}
