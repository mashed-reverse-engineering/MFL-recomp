#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
typedef signed char sbyte;
/*005738a0*/ extern void * FUN_005738a0(undefined4 p0);
/*00771a20*/ extern undefined4 g_os_771a20;
#define CONTACT_RECORD_SIZE (0x4c)
#define NIBBLE_MASK (0xf)
#define POINT_SET_ENTRY_STRIDE (8)
#define POINT_SET_ENTRY_ID_OFF (6)
#define POINT_SET_ENTRY_COUNT_OFF (0x84)
#define HIGH_WORD_UNIT (0x10000)

/* rwp_contact_collect */

/* "appends a 0x4C byte contact record from the contact point pointers"
 *
 * Appends one CONTACT_RECORD_SIZE byte record to record_list (base at [0], count at [1]),
 * filled from the three contact vectors (contact[7..9]) and the contact position (contact[0..2]).
 * point_set: the contact point set (inferred), used for the lookup table and the entry
 * table bounds. contact[6]: index into the point set table (inferred).
 * unused_1 and unused_4 are not read.
 * Returns contact.
 */

undefined4 * rwp_contact_collect(char *unused_1,char *point_set,undefined4 *contact,char *unused_4,
                   int *record_list)

{
  undefined4 *src_vec;
  uint nibble_index;
  int table_index;
  int *lookup;
  undefined4 *record;

  record = (undefined4 *)(record_list[1] * CONTACT_RECORD_SIZE +*record_list);
  record_list[1] = record_list[1] + 1;
  /* copy the three contact vectors (3 words each) into the record */
  src_vec = (undefined4 *)contact[7];
  *record = *src_vec;
  record[1] = src_vec[1];
  record[2] = src_vec[2];
  src_vec = (undefined4 *)contact[8];
  record[4] = *src_vec;
  record[5] = src_vec[1];
  record[6] = src_vec[2];
  src_vec = (undefined4 *)contact[9];
  record[8] = *src_vec;
  record[9] = src_vec[1];
  record[10] = src_vec[2];
  record[0xc] = *contact;
  record[0xd] = contact[1];
  record[0xe] = contact[2];
  record[0x10] = point_set;
  record[0x11] = contact[6];
  /* branch: g_os_771a20 is 1 or 5..6 (inferred: game mode check). Fills the lookup pointer
     and the 4-bit value from the nibble table at lookup */
  if (((int)g_os_771a20 == 1) ||((4 < (int)g_os_771a20 && ((int)g_os_771a20 < 7)))) {
    nibble_index = contact[6];
    lookup = (int *)FUN_005738a0(point_set);
    record_list[4] = (int)lookup;
    record[0x12] = *(uint *)(*lookup + (nibble_index >> 3) * 4) >> (sbyte)((nibble_index & 7) << 2) & NIBBLE_MASK;
  }
  /* branch: index must be inside the point set table; ORs the high 16 bits of record[0x11]
     with the table entry id plus the point set base (inferred) */
  if (((point_set != 0) &&(table_index = contact[6], -1 < table_index)) &&
     (table_index < (int)(uint)*(ushort *)(point_set + POINT_SET_ENTRY_COUNT_OFF))) {
    record[0x11] = record[0x11] |
                   ((uint)*(ushort *)(*(char **)(point_set + 4) + POINT_SET_ENTRY_ID_OFF + table_index * POINT_SET_ENTRY_STRIDE) +
                   (uint)*(ushort *)(point_set + 0x80)) * HIGH_WORD_UNIT;
  }
  return contact;
}
