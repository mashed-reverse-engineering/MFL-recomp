// cluster-entries: sector_table_clear
/* Installed cluster sector_table_clear (tools/cluster_install.py from output/cluster_pilot_zx/sector_table_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern char PTR_g_ai_data_tile_pool_32124__00629f88[];
/*00629a08*/ extern undefined g_sector_table_629a08;
/*00629dc8*/ extern undefined g_sector_table_629dc8;
/*0062a208*/ extern undefined g_sector_table_62a208;
/*00702268*/ extern undefined4 g_sector_table_702268;
#define SECTOR_TABLE_DWORD_COUNT (0x100)
#define SECTOR_ENTRY_STRIDE (0x40)
#define SECTOR_GROUP_A_FIRST_ID (0x21)
#define SECTOR_GROUP_A_COUNT (0xf)
#define SECTOR_GROUP_B_FIRST_ID (0x30)
#define SECTOR_GROUP_B_COUNT (10)
#define SECTOR_GROUP_C_FIRST_ID (0x3a)
#define SECTOR_GROUP_C_COUNT (7)
#define SECTOR_GROUP_D_FIRST_ID (0x41)
#define SECTOR_GROUP_D_COUNT (0x1a)
#define SECTOR_GROUP_D_PAIR_OFFSET (0x20)

/* sector_table_clear */

/* "clears 0x100 dwords at 0x702268" */

/*
 * Resets the sector table: zeroes SECTOR_TABLE_DWORD_COUNT dwords at g_sector_table_702268, then
 * calls sector_table_count_entries(id, entry) for every entry of the four sector groups
 * (A, B, C singles; D as pairs of id and id + SECTOR_GROUP_D_PAIR_OFFSET (inferred)).
 * Takes no parameters. Always returns 1 (inferred: no failure path in the body).
 */
static int sector_table_count_entries_r(int p0, char * p1);
extern void sector_table_count_entries(void);
undefined4 sector_table_clear(void)

{
  int remaining;
  uint entry_index;
  undefined4 *dword_ptr;
  undefined *entry_ptr;
  undefined **pool_entry_ptr;
  int sector_id;

  /* Zero the dword table at g_sector_table_702268. */
  dword_ptr = &g_sector_table_702268;
  for (remaining = SECTOR_TABLE_DWORD_COUNT; remaining != 0; remaining = remaining + -1) {
    *dword_ptr = 0;
    dword_ptr = dword_ptr + 1;
  }
  /* Group A: SECTOR_GROUP_A_COUNT entries from g_sector_table_629a08, SECTOR_ENTRY_STRIDE bytes apart. */
  entry_index = 0;
  entry_ptr = &g_sector_table_629a08;
  do {
    sector_table_count_entries_r(entry_index + SECTOR_GROUP_A_FIRST_ID,entry_ptr);
    entry_index = entry_index + 1;
    entry_ptr = entry_ptr + SECTOR_ENTRY_STRIDE;
  } while (entry_index < SECTOR_GROUP_A_COUNT);
  /* Group B: SECTOR_GROUP_B_COUNT entries from the tile pool pointer table (0x10 pointers apart). */
  entry_index = 0;
  pool_entry_ptr = &PTR_g_ai_data_tile_pool_32124__00629f88;
  do {
    sector_table_count_entries_r(entry_index + SECTOR_GROUP_B_FIRST_ID,pool_entry_ptr);
    entry_index = entry_index + 1;
    pool_entry_ptr = pool_entry_ptr + 0x10;
  } while (entry_index < SECTOR_GROUP_B_COUNT);
  /* Group C: SECTOR_GROUP_C_COUNT entries from g_sector_table_629dc8, SECTOR_ENTRY_STRIDE bytes apart. */
  entry_index = 0;
  entry_ptr = &g_sector_table_629dc8;
  do {
    sector_table_count_entries_r(entry_index + SECTOR_GROUP_C_FIRST_ID,entry_ptr);
    entry_index = entry_index + 1;
    entry_ptr = entry_ptr + SECTOR_ENTRY_STRIDE;
  } while (entry_index < SECTOR_GROUP_C_COUNT);
  /* Group D: SECTOR_GROUP_D_COUNT rows from g_sector_table_62a208; each row registers its id and id + SECTOR_GROUP_D_PAIR_OFFSET (inferred). */
  sector_id = SECTOR_GROUP_D_FIRST_ID;
  entry_ptr = &g_sector_table_62a208;
  remaining = SECTOR_GROUP_D_COUNT;
  do {
    sector_table_count_entries_r(sector_id + SECTOR_GROUP_D_PAIR_OFFSET,entry_ptr);
    sector_table_count_entries_r(sector_id,entry_ptr);
    entry_ptr = entry_ptr + SECTOR_ENTRY_STRIDE;
    sector_id = sector_id + 1;
    remaining = remaining + -1;
  } while (remaining != 0);
  return 1;
}

/* sector_table_count_entries_r: C -> original sector_table_count_entries (registers edx,eax) */
__declspec(naked) int sector_table_count_entries_r(int p0, char * p1)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        mov eax, dword ptr [esp + 8]
        call sector_table_count_entries
        ret
    }
}
