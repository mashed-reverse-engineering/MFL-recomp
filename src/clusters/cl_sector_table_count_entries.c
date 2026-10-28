// cluster-entries: sector_table_count_entries
/* Installed cluster sector_table_count_entries (tools/cluster_install.py from output/cluster_pilot10/sector_table_count_entries.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00702268*/ extern undefined4 g_sector_table_702268;
/*00702668*/ extern undefined g_sector_table_702668;
/* sector_table_count_entries */

/* "stores the table pointer for the sector and counts its entries up to the zero terminator" */

void sector_table_count_entries_n(int sector, char *table_cursor)

{
  char *b6_ptr;
  char *b7_ptr;
  char *b5_ptr;
  int *count_ptr;
  char *next_entry;
  
  count_ptr = &g_sector_table_702268 + sector;
  *(char **)((char *)&g_sector_table_702668 + sector * 4) = table_cursor;
  *count_ptr = 0;
  if (((table_cursor[3] != '\0' || table_cursor[2] != '\0') || table_cursor[1] != '\0') || *table_cursor != '\0') {
    do {
      next_entry = table_cursor + 4;
      *count_ptr = *count_ptr + 1;
      b6_ptr = table_cursor + 6;
      b7_ptr = table_cursor + 7;
      b5_ptr = table_cursor + 5;
      table_cursor = next_entry;
    } while (((*b7_ptr != '\0' || *b6_ptr != '\0') || *b5_ptr != '\0') || *next_entry != '\0');
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
