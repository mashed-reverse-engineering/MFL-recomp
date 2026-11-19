#include "mfl_tier2.h"
extern void * memcpy(void *data_a, void *data_b, unsigned int p2);


/* replay_record_save_frame */

/* "copies a 0x67 dword record and its array into the record list" */

undefined4 replay_record_save_frame(int *record,undefined4 *record_list)

{
  int *dest_record;
  int *record_array;
  
  record_array = (int *)record[5];
  for (dest_record = (int *)*record_list;
      ((dest_record[2] != record[2] || (*dest_record != 0x10b)) && ((dest_record[2] != 0 || (*dest_record != 0))));
      dest_record = (int *)((char *)dest_record + dest_record[1])) {
  }
  memcpy(dest_record,record,0x19c);
  memcpy(dest_record + 0x67,record_array,record[0x5b] * 0x24);
  return 1;
}
