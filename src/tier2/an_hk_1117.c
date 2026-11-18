#include "mfl_tier2.h"
/* gap_4c4a80 */

/* no context */

/* source record: three leading words, the word at +0x18, and three pointers to 3-word vectors at +0x1c / +0x20 / +0x24 */
typedef struct SrcRecord {
  uint head_first;
  uint head_second;
  uint head_third;
  uint unused_0c[3];
  uint extra_word;
  uint *vec_first;
  uint *vec_second;
  uint *vec_third;
} SrcRecord;

/* growing array of 0x40-byte records: records, a pointer to the live count, the capacity and a tag word stored in each record */
typedef struct RecordSink {
  union {
    uint (*rows)[16];
    uint *words;
  } records;
  int *count;
  int capacity;
  int tag;
} RecordSink;

uint gap_4c4a80(char *unused_1,undefined4 value_first,SrcRecord *src,undefined4 value_second,
               RecordSink *sink)

{
  uint (*rows)[16];
  uint *words;
  int capacity;
  int *count_ptr;
  int tag;
  int index;
  int dword_offset;
  int count;

  /* appends one 0x40-byte record built from src to sink->buffer[*count]: src[0..2] copied, src[6], three vectors through the pointers src[7..9], then value_first/value_second/tag; *count is incremented; returns src when the buffer is full (count == capacity), else 0 */
  rows = sink->records.rows;
  words = sink->records.words;
  capacity = sink->capacity;
  count_ptr = sink->count;
  tag = sink->tag;
  index = *count_ptr;
  dword_offset = index * 0x10;
  if (rows != 0) {
    rows[index][0] = src->head_first;
    rows[index][1] = src->head_second;
    words[dword_offset + 2] = src->head_third;
    words[dword_offset + 3] = src->extra_word;
    words[dword_offset + 4] = src->vec_first[0];
    words[dword_offset + 5] = src->vec_first[1];
    words[dword_offset + 6] = src->vec_first[2];
    words[dword_offset + 7] = src->vec_second[0];
    words[dword_offset + 8] = src->vec_second[1];
    words[dword_offset + 9] = src->vec_second[2];
    words[dword_offset + 10] = src->vec_third[0];
    words[dword_offset + 11] = src->vec_third[1];
    words[dword_offset + 12] = src->vec_third[2];
    words[dword_offset + 13] = value_first;
    words[dword_offset + 14] = value_second;
    words[dword_offset + 15] = tag;
  }
  count = *count_ptr;
  *count_ptr = count + 1;
  return (count + 1 == capacity) - 1 & (uint)src;
}
