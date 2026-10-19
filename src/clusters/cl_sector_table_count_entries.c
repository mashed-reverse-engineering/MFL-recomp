// cluster-entries: sector_table_count_entries
/* Installed cluster sector_table_count_entries (tools/cluster_install.py from output/cluster_pilot10/sector_table_count_entries.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00702268*/ extern undefined4 g_sector_table_702268;
/*00702668*/ extern undefined g_sector_table_702668;
/* sector_table_count_entries */

/* WARNING: Unknown calling convention */
/* "stores the table pointer for the sector and counts its entries up to the zero terminator" */

void sector_table_count_entries_n(int in_edx, char *in_eax)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  int *piVar4;
  char *pcVar5;
  
  piVar4 = &g_sector_table_702268 + in_edx;
  *(char **)((char *)&g_sector_table_702668 + in_edx * 4) = in_eax;
  *piVar4 = 0;
  if (((in_eax[3] != '\0' || in_eax[2] != '\0') || in_eax[1] != '\0') || *in_eax != '\0') {
    do {
      pcVar5 = in_eax + 4;
      *piVar4 = *piVar4 + 1;
      pcVar1 = in_eax + 6;
      pcVar2 = in_eax + 7;
      pcVar3 = in_eax + 5;
      in_eax = pcVar5;
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
