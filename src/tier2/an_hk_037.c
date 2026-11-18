#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
typedef signed char sbyte;
/*00771a20*/ extern undefined4 g_os_771a20;
#define CONTACT_RECORD_SIZE (0x4c)
#define NIBBLE_MASK (0xf)
#define POINT_SET_ENTRY_STRIDE (8)
#define POINT_SET_ENTRY_ID_OFF (6)
#define POINT_SET_ENTRY_COUNT_OFF (0x84)
#define HIGH_WORD_UNIT (0x10000)

/* entry of the point set table (8 bytes): the id word at +6 */
typedef struct PointEntry {
  char unknown_00[6];
  ushort id;
} PointEntry;

/* contact point set: entry table at +4, id base at +0x80, entry count at +0x84 */
typedef struct PointSet {
  char unknown_00[4];
  PointEntry *entries;
  char unknown_08[0x78];
  ushort id_base;
  char unknown_82[2];
  ushort entry_count;
} PointSet;

/* nibble lookup table: pointer to 32-bit words holding eight 4-bit values each */
typedef struct NibbleTable {
  uint *words;
} NibbleTable;

/*005738a0*/ extern NibbleTable * FUN_005738a0(PointSet *p0);

/* one contact as produced by the point collision: position, point index and three pointers to 3-word vectors */
typedef struct Contact {
  undefined4 pos[3];
  undefined4 unused_0c[3];
  uint point_index;
  undefined4 *vec_first;
  undefined4 *vec_second;
  undefined4 *vec_third;
} Contact;

/* 0x4c byte record appended to the record list */
typedef struct ContactRecord {
  undefined4 vec_first[3];
  undefined4 unused_0c;
  undefined4 vec_second[3];
  undefined4 unused_1c;
  undefined4 vec_third[3];
  undefined4 unused_2c;
  undefined4 pos[3];
  undefined4 unused_3c;
  PointSet *point_set;
  uint point_index;
  uint nibble;
} ContactRecord;

/* record list: base at +0, count at +4, lookup table pointer at +0x10 */
typedef struct ContactList {
  ContactRecord *records;
  int count;
  int unused_08[2];
  NibbleTable *lookup;
} ContactList;

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

Contact * rwp_contact_collect(char *unused_1,PointSet *point_set,Contact *contact,char *unused_4,
                   ContactList *record_list)

{
  undefined4 *src_vec;
  uint nibble_index;
  int table_index;
  NibbleTable *lookup;
  ContactRecord *record;

  record = record_list->records + record_list->count;
  record_list->count = record_list->count + 1;
  /* copy the three contact vectors (3 words each) into the record */
  src_vec = contact->vec_first;
  record->vec_first[0] = src_vec[0];
  record->vec_first[1] = src_vec[1];
  record->vec_first[2] = src_vec[2];
  src_vec = contact->vec_second;
  record->vec_second[0] = src_vec[0];
  record->vec_second[1] = src_vec[1];
  record->vec_second[2] = src_vec[2];
  src_vec = contact->vec_third;
  record->vec_third[0] = src_vec[0];
  record->vec_third[1] = src_vec[1];
  record->vec_third[2] = src_vec[2];
  record->pos[0] = contact->pos[0];
  record->pos[1] = contact->pos[1];
  record->pos[2] = contact->pos[2];
  record->point_set = point_set;
  record->point_index = contact->point_index;
  /* branch: g_os_771a20 is 1 or 5..6 (inferred: game mode check). Fills the lookup pointer
     and the 4-bit value from the nibble table at lookup */
  if (((int)g_os_771a20 == 1) ||((4 < (int)g_os_771a20 && ((int)g_os_771a20 < 7)))) {
    nibble_index = contact->point_index;
    lookup = FUN_005738a0(point_set);
    record_list->lookup = lookup;
    record->nibble = lookup->words[nibble_index >> 3] >> (sbyte)((nibble_index & 7) << 2) & NIBBLE_MASK;
  }
  /* branch: index must be inside the point set table; ORs the high 16 bits of record[0x11]
     with the table entry id plus the point set base (inferred) */
  if (((point_set != 0) &&(table_index = contact->point_index, -1 < table_index)) &&
     (table_index < (int)(uint)point_set->entry_count)) {
    record->point_index = record->point_index |
                   ((uint)point_set->entries[table_index].id +
                   (uint)point_set->id_base) * HIGH_WORD_UNIT;
  }
  return contact;
}
