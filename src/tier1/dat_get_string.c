#include "mfl_tier2.h"
/*0068b9f8*/ extern undefined g_dat_68b9f8;

#define DAT_OFFSET_ENTRY_SIZE 4 /* bytes per entry in the leading offset table */


/* dat_get_string */

/* "(index): entry of the DAT string table at 0x68B9F8 (offset table followed by the strings)" */

/* Return the DAT string table entry at index; each table slot holds an offset from the table base. */
undefined * dat_get_string(int index)
{
  return &g_dat_68b9f8 + *(int *)(&g_dat_68b9f8 + index * DAT_OFFSET_ENTRY_SIZE);
}
